#pragma once

#include "connection.h"

namespace onion {

class Quic : public Connection {
  public:
    Quic(const std::string &address, unsigned short port, bool server)
        : Connection(address, port, server){};

    virtual void SendData(const buffer_t &data) const;
};

} // namespace onion
