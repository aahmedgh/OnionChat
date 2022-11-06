#pragma once

#include "singleton.h"
#include "transport.h"
#include <event2/event.h>
#include <lsquic.h>
#include <memory>
#include <vector>

namespace onion {
namespace quic {

struct EngineDeleter {
    void operator()(lsquic_engine_t *ptr);
};

struct EventBaseDeleter {
    void operator()(event_base *ptr);
};

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
    TransportMode engine_mode;
    EnginePtr engine;
    struct event *timer;
    bool stopped;

    CEngineContext(event_base &event_base, TransportMode mode);
    ~CEngineContext();
    void InitEngine(const lsquic_engine_api &engine_api);
};
using ConnCtxPtr = std::unique_ptr<lsquic_conn_ctx_t>;

template <TransportMode mode> struct EngineContext : CEngineContext {
    EngineContext(event_base &event_base) : CEngineContext(event_base, mode){};
};

struct ClientContext : EngineContext<TransportMode::CLIENT> {
    using EngineContext::EngineContext;
    ConnCtxPtr conn_ctx;
};

struct ServerContext : EngineContext<TransportMode::SERVER> {
    using EngineContext::EngineContext;
    std::vector<ConnCtxPtr> conn_ctx;
};

class Engine : public Singleton<Engine> {
  public:
    Engine();
    ~Engine();

    lsquic_engine_t *GetEngine(TransportMode mode) const;

  private:
    EventBasePtr m_event_base;
    ClientContext m_client;
    ServerContext m_server;
};

} // namespace quic
} // namespace onion

struct lsquic_conn_ctx {
    lsquic_conn_t *connection;
    struct onion::quic::CEngineContext *context;

    lsquic_conn_ctx(lsquic_conn_t *conn, struct onion::quic::CEngineContext *ctx);
};
