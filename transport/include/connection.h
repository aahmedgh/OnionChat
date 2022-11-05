#pragma once

#include <string>

#include "datatypes.h"

namespace onion {

template <typename Protocol> class Connection {
  public:
    template <typename... Args>
    Connection(const std::string &address, unsigned short port, bool server)
        : m_address(address), m_port(port), m_server(server){};

    void SendData(const buffer_t &data) const {
        static_cast<Protocol const &>(*this).Send(data);
    }

  protected:
    std::string m_address;
    unsigned short m_port;
    bool m_server;
};

} // namespace onion
