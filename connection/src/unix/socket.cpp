#include "socket.h"
#include "logger.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>

namespace onion {

Socket::Socket(int family, int type, Protocol protocol)
    : m_family(family), m_type(type), m_protocol(protocol),
      m_descriptor(socket(family, type, static_cast<int>(protocol))){
          TRACE_LOG(CONNECTION, "Socket: " << m_descriptor)
          // TODO: Error checking on m_socket
      }

      Socket::~Socket() {
    TRACE_LOG(CONNECTION, "Closing socket")
    close(m_descriptor);
}

int Socket::Connect(const std::string &address, unsigned short port) const {
    struct sockaddr_in sock_addr {
        .sin_family = AF_INET, .sin_port = htons(port)
    };

    if (inet_aton(address.c_str(), reinterpret_cast<in_addr *>(&sock_addr.sin_addr.s_addr)) == -1) {
        DEBUG_LOG(UDP, "inet_aton failed")
        return -1;
    }

    if (connect(m_descriptor, reinterpret_cast<const sockaddr *>(&sock_addr), sizeof(sock_addr)) == -1) {
        DEBUG_LOG(CONNECTION, "connect failed with " << errno)
        return -1;
    }

    return 0;
}

int Socket::SendMsg(buffer_t &data) const {
    struct sockaddr name {};
    struct iovec iov {
        .iov_base = reinterpret_cast<void *>(data.data()), .iov_len = data.size()
    };
    // TODO: Other fields
    struct msghdr msg {
        .msg_namelen = sizeof(name), .msg_iov = &iov, .msg_iovlen = 1
    };

    if (getpeername(m_descriptor, &name, &msg.msg_namelen) == -1) {
        DEBUG_LOG(CONNECTION, "getpeername failed with " << errno);
        return -1;
    }
    msg.msg_name = &name;

    if (sendmsg(m_descriptor, &msg, 0) == -1) {
        DEBUG_LOG(CONNECTION, "sendmsg failed with " << errno);
        return -1;
    }

    return 0;
}

} // namespace onion
