#include "quic.h"
#include "engine.h"
#include "logger.h"

namespace onion {

void QuicClient::SendData(const std::string &address, unsigned short port, const buffer_t &data) {
    // Get the client engine
    lsquic_engine_t *engine_ptr{quic::Engine::Get().GetEngine(ConnectionMode::CLIENT)};
    DEBUG_LOG(QUIC, engine_ptr)

    DEBUG_LOG(QUIC, "Sending data of size " << data.size() << " to " << address << ":" << port)
}

} // namespace onion
