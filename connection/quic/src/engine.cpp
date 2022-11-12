#ifndef _WIN32
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#error "Not implemented for Windows"
#endif

#include "engine.h"
#include "logger.h"

namespace onion {
namespace quic {

#define UDP 17

void EngineDeleter::operator()(lsquic_engine_t *ptr) {
    if (ptr != nullptr) {
        TRACE_LOG(QUIC, "Deleting engine ptr")
        lsquic_engine_destroy(ptr);
    }
}

void EventBaseDeleter::operator()(event_base *ptr) {
    TRACE_LOG(QUIC, "Deleting event base ptr")
    event_base_free(ptr);
}

void EventDeleter::operator()(event *ptr) {
    if (ptr != nullptr) {
        TRACE_LOG(QUIC, "Deleting event ptr")
        event_free(ptr);
    }
}

// BEGIN LSQUIC ENGINE FUNCTIONS/CALLBACKS
static int SendPacketsOut(void *packets_out_ctx, const struct lsquic_out_spec *out_spec, unsigned int n_packets_out) {
    DEBUG_LOG(QUIC, "Sending packets")

#ifndef _WIN32
    struct msghdr msg {};
#endif
    auto *context = reinterpret_cast<CEngineContext *>(packets_out_ctx);
    unsigned count;

    // TODO: Look into sendmmsg (Linux kernel 3.0+ only)
    // TODO: Need to add ancillary information (cmsghdr)
    for (count = 0; count < n_packets_out; ++count) {
#ifndef _WIN32
        msg.msg_name = reinterpret_cast<void *>(const_cast<struct sockaddr *>(out_spec[count].dest_sa));
        msg.msg_namelen = sizeof(struct sockaddr_in);
        msg.msg_iov = out_spec[count].iov;
        msg.msg_iovlen = out_spec[count].iovlen;

        if (context->socket->SendMsg(&msg, 0) < 0) {
            ERROR_LOG(QUIC, "sendmsg failed with code " << errno)
            break;
        }
#endif
    }

    return static_cast<int>(count);
}

static void ProcessConnections(struct CEngineContext *context) {
    int diff;

    DEBUG_LOG(QUIC, "Processing connections")

    lsquic_engine_process_conns(context->engine.get());

    if (lsquic_engine_earliest_adv_tick(context->engine.get(), &diff) != 0) {
        struct timeval timeout {}; // Default values are 0
        if (diff > 0) {
            timeout.tv_sec = static_cast<unsigned>(diff / 1000000);
            timeout.tv_usec = static_cast<unsigned>(diff % 1000000);
        }

        if (context->have_connection)
            event_add(context->timer.get(), &timeout);
    }
}

static void TimerFunc(int descriptor, short event_flags, void *arg) {
    auto *context = reinterpret_cast<CEngineContext *>(arg);
    if (context->have_connection)
        ProcessConnections(context);
}

static lsquic_conn_ctx_t *ClientOnNewConnection(void *stream_if_ctx, lsquic_conn_t *conn) {
    DEBUG_LOG(QUIC, "Opening new client connection")
    auto *context = static_cast<ClientContext *>(stream_if_ctx);
    context->have_connection = true;
    lsquic_conn_make_stream(conn);
    return new lsquic_conn_ctx_t(conn, static_cast<CEngineContext *>(context));
}

static void ClientOnConnClosed(lsquic_conn_t *conn) {
    DEBUG_LOG(QUIC, "Closing client connection")
    lsquic_conn_ctx_t *conn_context = lsquic_conn_get_ctx(conn);
    auto *context = static_cast<ClientContext *>(conn_context->engine_context);
    delete conn_context;
    context->have_connection = false;
}

static lsquic_stream_ctx_t *ClientOnNewStream(void *ea_stream_if_ctx, lsquic_stream_t *stream) {
    DEBUG_LOG(QUIC, "Creating new client stream")
    auto *context = static_cast<ClientContext *>(ea_stream_if_ctx);
    lsquic_stream_wantwrite(stream, 1);
    return new lsquic_stream_ctx_t(stream, static_cast<CEngineContext *>(context));
}

static void ClientOnStreamClose(lsquic_stream_t *stream, lsquic_stream_ctx_t *stream_ctx) {
    DEBUG_LOG(QUIC, "Closing client stream")
    delete stream_ctx;
}

static lsquic_conn_ctx_t *ServerOnNewConnection(void *stream_if_ctx, lsquic_conn_t *conn) {
    auto *context = static_cast<ServerContext *>(stream_if_ctx);
    auto pair =
        context->conn_ctx.emplace(reinterpret_cast<uintptr_t>(conn),
                                  std::make_unique<lsquic_conn_ctx_t>(conn, static_cast<CEngineContext *>(context)));
    context->have_connection = true;

    DEBUG_LOG(QUIC, "Received new connection")
    return pair.first->second.get();
}

static void ServerOnConnClosed(lsquic_conn_t *conn) {
    DEBUG_LOG(QUIC, "Closing connection to server")
    auto *context = static_cast<ServerContext *>(lsquic_conn_get_ctx(conn)->engine_context);
    context->conn_ctx.erase(reinterpret_cast<uintptr_t>(conn));
    if (context->conn_ctx.empty())
        context->have_connection = false;
}

static lsquic_stream_ctx_t *ServerOnNewStream(void *ea_stream_if_ctx, lsquic_stream_t *stream) {
    auto *context = static_cast<ServerContext *>(ea_stream_if_ctx);
    return nullptr;
}
// END LSQUIC ENGINE FUNCTIONS/CALLBACKS

CEngineContext::CEngineContext(event_base &event_base, ConnectionMode mode)
    : socket(std::make_unique<Socket>(AF_INET, SOCK_DGRAM, UDP)), engine_mode(mode),
      timer(event_new(&event_base, -1, 0, TimerFunc, reinterpret_cast<void *>(this))) {}

CEngineContext::~CEngineContext() { TRACE_LOG(QUIC, "Destroying context") }

void CEngineContext::InitEngine(const lsquic_engine_api &engine_api) {
    engine.reset(lsquic_engine_new(engine_mode == ConnectionMode::CLIENT ? 0 : LSENG_SERVER, &engine_api));
}

Engine::Engine() : m_event_base(EventBasePtr{event_base_new()}), m_client(*m_event_base), m_server(*m_event_base) {
    // TODO: Check for error conditions
    // TODO: Defines for flags (SERVER_ONLY, CLIENT_ONLY)
    DEBUG_LOG(QUIC, "Initializing QUIC Engine")
    lsquic_global_init(LSQUIC_GLOBAL_CLIENT | LSQUIC_GLOBAL_SERVER);

    // Initialize the client engine
    struct lsquic_stream_if client_if {
        .on_new_conn = ClientOnNewConnection, .on_conn_closed = ClientOnConnClosed, .on_new_stream = ClientOnNewStream,
    };

    lsquic_engine_api engine_api{.ea_stream_if = &client_if,
                                 .ea_stream_if_ctx = reinterpret_cast<void *>(&m_client),
                                 .ea_packets_out = SendPacketsOut,
                                 .ea_packets_out_ctx = reinterpret_cast<void *>(&m_client)};
    m_client.InitEngine(engine_api);

    // Initialize the server engine
    struct lsquic_stream_if server_if {
        .on_new_conn = ServerOnNewConnection, .on_conn_closed = ServerOnConnClosed, .on_new_stream = ServerOnNewStream,
    };
    engine_api.ea_stream_if = &server_if;
    engine_api.ea_stream_if_ctx = reinterpret_cast<void *>(&m_server);
    engine_api.ea_packets_out_ctx = reinterpret_cast<void *>(&m_server);
    m_server.InitEngine(engine_api);
}

Engine::~Engine() {
    TRACE_LOG(QUIC, "Destroying engine")
    lsquic_global_cleanup();
}

lsquic_engine_t *Engine::GetEngine(ConnectionMode mode) const {
    return mode == ConnectionMode::CLIENT ? m_client.engine.get() : m_server.engine.get();
}

} // namespace quic
} // namespace onion

lsquic_conn_ctx::lsquic_conn_ctx(lsquic_conn_t *conn, struct onion::quic::CEngineContext *ctx)
    : connection(conn), engine_context(ctx) {}

lsquic_stream_ctx::lsquic_stream_ctx(lsquic_stream_t *strm, struct onion::quic::CEngineContext *ctx)
    : stream(strm), engine_context(ctx) {}
