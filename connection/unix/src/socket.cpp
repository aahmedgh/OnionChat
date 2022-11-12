#include "socket.h"
#include "logger.h"
#include <unistd.h>

namespace onion {

Socket::Socket(int family, int type, int protocol) : m_socket(socket(family, type, protocol)) {
    TRACE_LOG(CONNECTION, "Socket: " << m_socket)
    // TODO: Error checking on m_socket
}

Socket::~Socket() {
    TRACE_LOG(CONNECTION, "Closing socket")
    close(m_socket);
}

ssize_t Socket::SendMsg(const struct msghdr *msg, int flags) const { return sendmsg(m_socket, msg, flags); }

} // namespace onion
