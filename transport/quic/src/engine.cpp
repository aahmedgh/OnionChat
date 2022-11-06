#ifndef _WIN32
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#error "Not implemented for Windows"
#endif

#include "logger.h"
#include "quic_engine.h"

namespace onion {
namespace quic {

#define UDP 17

static SocketType OpenSocket() {
    // TODO: Error checking
#ifndef _WIN32
    int sock{socket(AF_INET, SOCK_DGRAM, UDP)};
#endif

    DEBUG_LOG(QUIC, "Socket: " << sock);
    return sock;
}

static void CloseSocket(SocketType sock) {
#ifndef _WIN32
    close(sock);
#endif
}

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

        if (sendmsg(context->socket, &msg, 0) < 0) {
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

        if (!context->stopped)
            event_add(context->timer, &timeout);
    }
}

static void TimerFunc(int file_descriptor, short event_flags, void *arg) {
    auto *context = reinterpret_cast<CEngineContext *>(arg);
    if (!context->stopped)
        ProcessConnections(context);
}

static lsquic_conn_ctx_t *ClientOnNewConnection(void *stream_if_ctx, lsquic_conn_t *conn) {
    auto *context = reinterpret_cast<ClientContext *>(stream_if_ctx);
    context->conn_ctx = std::make_unique<lsquic_conn_ctx_t>(conn, static_cast<CEngineContext *>(context));
    lsquic_conn_make_stream(conn);

    return context->conn_ctx.get();
}

static lsquic_conn_ctx_t *ServerOnNewConnection(void *stream_if_ctx, lsquic_conn_t *conn) {
    auto *context = reinterpret_cast<ServerContext *>(stream_if_ctx);
    auto &ptr = context->conn_ctx.emplace_back(
        std::make_unique<lsquic_conn_ctx_t>(conn, static_cast<CEngineContext *>(context)));

    DEBUG_LOG(QUIC, "Received new connection")
    return ptr.get();
}
// END LSQUIC ENGINE FUNCTIONS/CALLBACKS

CEngineContext::CEngineContext(event_base &event_base, TransportMode mode)
    : socket(OpenSocket()), engine_mode(mode),
      timer(event_new(&event_base, -1, 0, TimerFunc, reinterpret_cast<void *>(this))) {}

CEngineContext::~CEngineContext() {
    TRACE_LOG(QUIC, "Destroying context")
    stopped = true;
    CloseSocket(socket);
}

void CEngineContext::InitEngine(const lsquic_engine_api &engine_api) {
    engine.reset(lsquic_engine_new(engine_mode == TransportMode::CLIENT ? 0 : LSENG_SERVER, &engine_api));
}

Engine::Engine() : m_event_base(EventBasePtr{event_base_new()}), m_client(*m_event_base), m_server(*m_event_base) {
    // TODO: Check for error conditions
    // TODO: Defines for flags (SERVER_ONLY, CLIENT_ONLY)
    DEBUG_LOG(QUIC, "Initializing QUIC Engine")
    lsquic_global_init(LSQUIC_GLOBAL_CLIENT | LSQUIC_GLOBAL_SERVER);

    // Initialize the client engine
    struct lsquic_stream_if client_if {
        .on_new_conn = ClientOnNewConnection,
    };

    lsquic_engine_api engine_api{.ea_stream_if = &client_if,
                                 .ea_stream_if_ctx = reinterpret_cast<void *>(&m_client),
                                 .ea_packets_out = SendPacketsOut,
                                 .ea_packets_out_ctx = reinterpret_cast<void *>(&m_client)};
    m_client.InitEngine(engine_api);

    // Initialize the server engine
    struct lsquic_stream_if server_if {
        .on_new_conn = ServerOnNewConnection,
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

lsquic_engine_t *Engine::GetEngine(TransportMode mode) const {
    return mode == TransportMode::CLIENT ? m_client.engine.get() : m_server.engine.get();
}

} // namespace quic
} // namespace onion

lsquic_conn_ctx::lsquic_conn_ctx(lsquic_conn_t *conn, struct onion::quic::CEngineContext *ctx)
    : connection(conn), context(ctx) {}
