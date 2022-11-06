#pragma once

#include "transport.h"

namespace onion {

class Quic : public Transport {
  public:
    Quic(const std::string &address, unsigned short port, TransportMode mode) : Transport(address, port, mode){};

    virtual void SendData(const buffer_t &data) const;
};

} // namespace onion
