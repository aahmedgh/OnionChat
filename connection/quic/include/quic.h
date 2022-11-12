#pragma once

#include "connection.h"

namespace onion {

class QuicClient {
  public:
    static void SendData(const std::string &address, unsigned short port, const buffer_t &data);
};

class QuicServer {
    //
};

} // namespace onion
