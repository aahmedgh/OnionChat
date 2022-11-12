#pragma once

#ifndef _WIN32
#include <sys/socket.h>
#endif

namespace onion {

#ifdef _WIN32
using SocketType = SOCKET;
#else
using SocketType = int;
#endif

class Socket {
  public:
    Socket(int family, int type, int protocol);
    ~Socket();
#ifndef _WIN32
    ssize_t SendMsg(const struct msghdr *msg, int flags) const;
#endif

  private:
    SocketType m_socket;
};

} // namespace onion
