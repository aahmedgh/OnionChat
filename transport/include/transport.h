#pragma once

#include <string>

#include "datatypes.h"

namespace onion {

class Transport {
  public:
    Transport(const std::string &address, unsigned short port, bool server)
        : m_address(address), m_port(port), m_server(server){};
    virtual ~Transport() = default;

    virtual void SendData(const buffer_t &data) const = 0;

  protected:
    std::string m_address;
    unsigned short m_port;
    bool m_server;
};

} // namespace onion
