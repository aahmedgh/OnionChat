#pragma once

#ifndef _WIN32
#include <sys/socket.h>
#else
#include <Windows.h>
#include <Winsock2.h>
#endif

#include "datatypes.h"
#include <memory>
#include <string>

namespace onion {

#ifdef _WIN32
using SocketType = SOCKET;
#else
using SocketType = int;
#endif

enum class Protocol { UDP = 17 };

class Socket {
  public:
    Socket(int family, int type, Protocol protocol);
    ~Socket();

    int Connect(const std::string &address, unsigned short port) const;
    int SendMsg(buffer_t &data) const;

  private:
    int m_family;
    int m_type;
    Protocol m_protocol;
    SocketType m_descriptor;

#ifdef _WIN32
    static WSAData *InitWSA();
    std::unique_ptr<WSAData, void (*)(WSAData *)> m_wsadata{InitWSA(), [](WSAData *ptr) {
                                                                if (ptr != nullptr) {
                                                                    WSACleanup();
                                                                    delete ptr;
                                                                }
                                                            }};
#endif
};

} // namespace onion
