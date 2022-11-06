#pragma once

#include <memory>
#include <string>
#include <utility>

#include "datatypes.h"
#include "transport.h"

namespace onion {

template <typename T>
concept TransportType = std::is_base_of<Transport, T>::value;

template <TransportType T> class Client {
  public:
    template <typename... Args>
    Client(const std::string &address, unsigned short port, Args... args)
        : m_transport(address, port, false, std::forward<Args>(args)...){};

    void SendData(const buffer_t &data) { m_transport.SendData(data); }

  private:
    T m_transport;
};

} // namespace onion
