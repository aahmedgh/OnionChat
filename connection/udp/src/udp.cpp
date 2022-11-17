#include "udp.h"

namespace onion {

UDP::UDP(const std::string &address, unsigned short port)
    : Connection(address, port, AF_INET, SOCK_DGRAM, Protocol::UDP) {}

int UDP::Connect() const { return m_socket.Connect(m_address, m_port); }

long UDP::Write(buffer_t &data) const { return m_socket.SendMsg(data); }

buffer_t UDP::Read() const { return {}; }

} // namespace onion
