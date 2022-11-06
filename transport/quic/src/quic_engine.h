#pragma once

#include "singleton.h"
#include <event2/event.h>
#include <lsquic.h>
#include <memory>
#include <vector>

namespace onion {
namespace quic {

struct EngineDeleter {
    void operator()(lsquic_engine_t *ptr) {
        if (ptr != nullptr)
            lsquic_engine_destroy(ptr);
    }
};

struct EventBaseDeleter {
    void operator()(event_base *ptr) { event_base_free(ptr); }
};

enum class EngineType { CLIENT, SERVER };

using EnginePtr = std::unique_ptr<lsquic_engine_t, EngineDeleter>;
using EventBasePtr = std::unique_ptr<event_base, EventBaseDeleter>;

#ifdef _WIN32
using SocketType = SOCKET;
#else
using SocketType = int;
#endif

// C style struct that is not templated for easy casting
struct CEngineContext {
    SocketType socket;
    EngineType engine_type;
    EnginePtr engine;
    struct event *timer;
    bool stopped;

    CEngineContext(event_base &event_base, EngineType type);
    ~CEngineContext();
    void InitEngine(const lsquic_engine_api &engine_api);
};

struct ConnContext {
    lsquic_conn_t *connection;
    struct CEngineContext *context;

    ConnContext(lsquic_conn_t *conn, struct CEngineContext *ctx);
};

using lsquic_conn_ctx_t = struct ConnContext;
using ConnCtxPtr = std::unique_ptr<lsquic_conn_ctx_t>;

template <EngineType type> struct EngineContext : CEngineContext {
    EngineContext(event_base &event_base) : CEngineContext(event_base, type){};
};

struct ClientContext : EngineContext<EngineType::CLIENT> {
    using EngineContext::EngineContext;
    ConnCtxPtr conn_ctx;
};

struct ServerContext : EngineContext<EngineType::SERVER> {
    using EngineContext::EngineContext;
    std::vector<ConnCtxPtr> conn_ctx;
};

class Engine : public Singleton<Engine> {
  public:
    Engine();
    ~Engine();

    lsquic_engine_t *GetEngine(EngineType type) const;

  private:
    EventBasePtr m_event_base;
    ClientContext m_client;
    ServerContext m_server;
};

} // namespace quic
} // namespace onion
