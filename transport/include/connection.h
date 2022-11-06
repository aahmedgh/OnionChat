#pragma once

#include <string>

#include "datatypes.h"

namespace onion {

class Connection {
  public:
    Connection(const std::string &address, unsigned short port, bool server)
        : m_address(address), m_port(port), m_server(server){};
    virtual ~Connection() = default;

    virtual void SendData(const buffer_t &data) const = 0;

  protected:
    std::string m_address;
    unsigned short m_port;
    bool m_server;
};

} // namespace onion
