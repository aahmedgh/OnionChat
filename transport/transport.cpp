#include <string>

#include "client.h"
#include "datatypes.h"
#include "quic.h"

using namespace onion;

int main() {
    Client<Quic> client{"localhost", 12345};
    std::string message{"Hello World"};
    buffer_t buf{message.size()};
    std::transform(message.begin(), message.end(), buf.begin(),
                   [](char c) { return std::byte(c); });
    client.SendData(buf);
}
