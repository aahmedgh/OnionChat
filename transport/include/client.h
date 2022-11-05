#pragma once

#include <memory>
#include <string>
#include <utility>

#include "connection.h"
#include "datatypes.h"

namespace onion {

template <typename T>
concept ConnectionType = std::is_base_of<Connection, T>::value;

template <ConnectionType T> class Client {
  public:
    template <typename... Args>
    Client(const std::string &address, unsigned short port, Args... args)
        : m_transport(address, port, false, std::forward<Args>(args)...){};

    void SendData(const buffer_t &data) { m_transport.SendData(); }

  private:
    T m_transport;
};

} // namespace onion
