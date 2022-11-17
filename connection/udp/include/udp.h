#pragma once

#include "connection.h"

namespace onion {

class UDP : public Connection {
  public:
    UDP(const std::string &address, unsigned short port);
    virtual int Connect() const override;
    virtual long Write(buffer_t &data) const override;
    virtual buffer_t Read() const override;
};

} // namespace onion
