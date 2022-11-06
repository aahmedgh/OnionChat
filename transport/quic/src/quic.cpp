#include <memory>

#include "logger.h"
#include "quic.h"

namespace onion {
namespace quic {

Engine::Engine() {
    // TODO: Check for error conditions
    // TODO: Defines for flags
    DEBUG_LOG(QUIC, "Initializing QUIC Engine")
    lsquic_global_init(LSQUIC_GLOBAL_CLIENT | LSQUIC_GLOBAL_SERVER);

    lsquic_engine_api client_engine_api{.ea_stream_if = nullptr,
                                        .ea_stream_if_ctx = nullptr,
                                        .ea_packets_out = nullptr,
                                        .ea_packets_out_ctx = nullptr};

    m_client = EnginePtr{lsquic_engine_new(0, &client_engine_api)};

    // TODO: m_server
}

Engine::~Engine() { lsquic_global_cleanup(); }

lsquic_engine_t *Engine::GetEngine(EngineType type) {
    return type == EngineType::CLIENT ? m_client.get() : m_server.get();
}

} // namespace quic

void Quic::SendData(const onion::buffer_t &data) const {
    // Get the client engine
    lsquic_engine_t *engine_ptr{
        quic::Engine::Get().GetEngine(quic::EngineType::CLIENT)};
    DEBUG_LOG(QUIC, engine_ptr)

    DEBUG_LOG(QUIC, "Sending data of size " << data.size() << " to "
                                            << m_address << ":" << m_port)
}

} // namespace onion
