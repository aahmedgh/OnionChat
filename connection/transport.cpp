#include <algorithm>
#include <string>

#include "client.h"
#include "datatypes.h"
#include "logger.h"
#include "udp.h"

using namespace onion;

int main() {
    Client<UDP> client{"127.0.0.1", 12345};
    std::string message{"Hello World"};
    buffer_t buf{message.size()};
    std::transform(message.begin(), message.end(), buf.begin(), [](char c) { return std::byte(c); });
    auto ret = client.SendData(buf);
    DEBUG_LOG(CONNECTION, "Returned " << ret)
}
