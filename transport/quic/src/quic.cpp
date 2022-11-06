#include "quic.h"
#include "logger.h"
#include "quic_engine.h"

namespace onion {

void Quic::SendData(const onion::buffer_t &data) const {
    // Get the client engine
    lsquic_engine_t *engine_ptr{quic::Engine::Get().GetEngine(TransportMode::CLIENT)};
    DEBUG_LOG(QUIC, engine_ptr)

    DEBUG_LOG(QUIC, "Sending data of size " << data.size() << " to " << m_address << ":" << m_port)
}

} // namespace onion
