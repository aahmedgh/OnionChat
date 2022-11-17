#include "socket.h"
#include "logger.h"
#include <WS2tcpip.h>

namespace onion {

WSAData *Socket::InitWSA() {
    // TODO: Error checking
    TRACE_LOG(CONNECTION, "Initializing WSA")
    auto *wsadata{new WSAData()};
    WSAStartup(MAKEWORD(2, 2), wsadata);
    return wsadata;
}

Socket::Socket(int family, int type, onion::Protocol protocol) : m_family(family), m_type(type), m_protocol(protocol) {

    // Note: InitWSA may not have been called yet if we assign m_descriptor before the body
    m_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_descriptor == INVALID_SOCKET) {
        DEBUG_LOG(CONNECTION, "socket failed with " << WSAGetLastError());
        // TODO: Throw exception
    }
    TRACE_LOG(CONNECTION, "Socket: " << m_descriptor)
}

Socket::~Socket() {
    TRACE_LOG(CONNECTION, "Closing socket")
    closesocket(m_descriptor);
}

int Socket::Connect(const std::string &address, unsigned short port) const {
    // TODO: Assume IPV4 for now, use m_family later
    SOCKADDR_IN sock_addr{.sin_family = static_cast<ADDRESS_FAMILY>(m_family), .sin_port = htons(port)};

    if (inet_pton(m_family, address.c_str(), reinterpret_cast<PVOID>(&sock_addr.sin_addr)) != 1) {
        DEBUG_LOG(CONNECTION, "inet_pton failed with " << WSAGetLastError())
        return 0;
    }

    if (connect(m_descriptor, reinterpret_cast<const sockaddr *>(&sock_addr), sizeof(sock_addr)) == SOCKET_ERROR) {
        DEBUG_LOG(CONNECTION, "connect failed with " << WSAGetLastError())
        return SOCKET_ERROR;
    }

    return 0;
}

int Socket::SendMsg(buffer_t &data) const {
    SOCKADDR sock_addr{};
    INT sock_len;

    if (getpeername(m_descriptor, &sock_addr, &sock_len) == SOCKET_ERROR) {
        DEBUG_LOG(CONNECTION, "getpeername failed with " << WSAGetLastError());
        return SOCKET_ERROR;
    }

    WSABUF buf{
        .len = static_cast<ULONG>(data.size()),
        .buf = reinterpret_cast<PCHAR>(data.data()),
    };
    // TODO: Other fields
    WSAMSG msg{.name = &sock_addr, .namelen = sock_len, .lpBuffers = &buf, .dwBufferCount = 1};

    DWORD bytes_sent;
    auto ret = WSASendMsg(m_descriptor, &msg, 0, &bytes_sent, nullptr, nullptr);
    if (ret == SOCKET_ERROR)
        DEBUG_LOG(CONNECTION, "WSASendMsg failed with " << WSAGetLastError());
    return ret;
}

} // namespace onion
