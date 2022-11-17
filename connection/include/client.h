#pragma once

#include <memory>
#include <string>
#include <utility>

#include "connection.h"
#include "datatypes.h"

namespace onion {

template <ConnectionType T> class Client {
  public:
    template <typename... Args>
    Client(const std::string &address, unsigned short port, Args... args)
        : m_connection(address, port, std::forward<Args>(args)...) {
        m_connection.Connect();
    }

    int SendData(buffer_t &data) { return m_connection.Write(data); }

  private:
    T m_connection;
};

} // namespace onion
