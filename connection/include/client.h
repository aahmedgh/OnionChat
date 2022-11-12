#pragma once

#include <memory>
#include <string>
#include <utility>

#include "connection.h"
#include "datatypes.h"

namespace onion {

template <ClientType T> class Client : private T {
  public:
    template <typename... Args>
    Client(const std::string &address, unsigned short port, Args... args)
        : T(std::forward<Args>(args)...), m_address(address), m_port(port){};

    void Send(const buffer_t &data) { T::SendData(m_address, m_port, data); }

  private:
    std::string m_address;
    unsigned short m_port;
};

} // namespace onion
