#include "quic.h"
#include <iostream>
#include <lsquic.h>

namespace onion {

void Quic::SendData(const onion::buffer_t &data) const {
    std::cout << "Sending data of size " << data.size() << " to " << m_address
              << ":" << m_port << std::endl;
}

} // namespace onion
