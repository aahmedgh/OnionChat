#pragma once

#include <string>
#include <type_traits>

#include "datatypes.h"
#include "singleton.h"
#include "socket.h"

namespace onion {

class Connection {
  public:
    Connection(const std::string &address, unsigned short port, int family, int type, Protocol protocol)
        : m_address(address), m_port(port), m_socket(family, type, protocol){};
    virtual int Connect() const = 0;
    virtual long Write(buffer_t &data) const = 0;
    virtual buffer_t Read() const = 0;

  protected:
    std::string m_address;
    unsigned short m_port;
    Socket m_socket;
};

template <typename T>
concept ConnectionType = std::is_base_of<Connection, T>::value;

} // namespace onion
