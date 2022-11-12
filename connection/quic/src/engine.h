#pragma once

#include "connection.h"
#include "singleton.h"
#include "socket.h"
#include <event2/event.h>
#include <lsquic.h>
#include <memory>
#include <unordered_map>

namespace onion {
namespace quic {

struct EngineDeleter {
    void operator()(lsquic_engine_t *ptr);
};

struct EventBaseDeleter {
    void operator()(event_base *ptr);
};

struct EventDeleter {
    void operator()(event *ptr);
};

using EnginePtr = std::unique_ptr<lsquic_engine_t, EngineDeleter>;
using EventBasePtr = std::unique_ptr<event_base, EventBaseDeleter>;
using EventPtr = std::unique_ptr<event, EventDeleter>;

// C style struct that is not templated for easy casting
struct CEngineContext {
    std::unique_ptr<Socket> socket;
    ConnectionMode engine_mode;
    EnginePtr engine;
    EventPtr timer;
    bool have_connection;

    CEngineContext(event_base &event_base, ConnectionMode mode);
    ~CEngineContext();
    void InitEngine(const lsquic_engine_api &engine_api);
};
using ConnCtxPtr = std::unique_ptr<lsquic_conn_ctx_t>;

template <ConnectionMode mode> struct EngineContext : CEngineContext {};

template <> struct EngineContext<ConnectionMode::CLIENT> : CEngineContext {
    EngineContext(event_base &event_base) : CEngineContext(event_base, ConnectionMode::CLIENT){};
};

template <> struct EngineContext<ConnectionMode::SERVER> : CEngineContext {
    EngineContext(event_base &event_base) : CEngineContext(event_base, ConnectionMode::SERVER){};
    std::unordered_map<uintptr_t, ConnCtxPtr> conn_ctx;
};

using ClientContext = struct EngineContext<ConnectionMode::CLIENT>;
using ServerContext = struct EngineContext<ConnectionMode::SERVER>;

class Engine : public Singleton<Engine> {
  public:
    lsquic_engine_t *GetEngine(ConnectionMode mode) const;

  private:
    friend class Singleton<Engine>;
    Engine();
    ~Engine();

    EventBasePtr m_event_base;
    ClientContext m_client;
    ServerContext m_server;
};

} // namespace quic
} // namespace onion

struct lsquic_conn_ctx {
    lsquic_conn_t *connection;
    struct onion::quic::CEngineContext *engine_context;

    lsquic_conn_ctx(lsquic_conn_t *conn, struct onion::quic::CEngineContext *ctx);
};

struct lsquic_stream_ctx {
    lsquic_stream_t *stream;
    struct onion::quic::CEngineContext *engine_context;
    onion::buffer_t buf;

    lsquic_stream_ctx(lsquic_stream_t *strm, struct onion::quic::CEngineContext *ctx);
};
