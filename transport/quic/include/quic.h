#pragma once

#include "connection.h"

namespace onion {

class Quic : public Connection<Quic> {
  public:
    Quic(const std::string &address, unsigned short port, bool server)
        : Connection<Quic>(address, port, server){};

    void Send(const buffer_t &data) const;
};

} // namespace onion
