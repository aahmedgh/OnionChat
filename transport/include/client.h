#pragma once

#include <memory>
#include <string>
#include <utility>

#include "connection.h"
#include "datatypes.h"

namespace onion {

template <class Protocol> class Client {
  public:
    template <typename... Args>
    Client(const std::string &address, unsigned short port, Args... args)
        : m_transport(std::make_unique<Protocol>(
              address, port, false, std::forward<Args>(args)...)){};

    void SendData(const buffer_t &data) { m_transport->SendData(data); }

  private:
    std::unique_ptr<Protocol> m_transport;
};

} // namespace onion
