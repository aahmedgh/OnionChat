#pragma once

#include <string>

#include "datatypes.h"

namespace onion {

enum class TransportMode { CLIENT, SERVER };

class Transport {
  public:
    Transport(const std::string &address, unsigned short port, TransportMode mode)
        : m_address(address), m_port(port), m_mode(mode){};
    virtual ~Transport() = default;

    virtual void SendData(const buffer_t &data) const = 0;

  protected:
    std::string m_address;
    unsigned short m_port;
    TransportMode m_mode;
};

} // namespace onion
