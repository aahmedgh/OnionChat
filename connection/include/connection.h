#pragma once

#include <string>

#include "datatypes.h"
#include "singleton.h"

namespace onion {

enum class ConnectionMode { CLIENT, SERVER };

template <typename T>
concept ClientType = requires(T client) {
    requires requires(const std::string &address, unsigned short port, const buffer_t &data) {
        { client.SendData(address, port, data) } -> std::same_as<void>;
    };
};
} // namespace onion
