#pragma once

#include "connection.h"
#include "singleton.h"
#include <lsquic.h>

namespace onion {
namespace quic {

struct EngineDeleter {
    void operator()(lsquic_engine_t *ptr) {
        if (ptr != nullptr)
            lsquic_engine_destroy(ptr);
    }
};

using EnginePtr = std::unique_ptr<lsquic_engine_t, EngineDeleter>;

enum class EngineType { CLIENT, SERVER };

class Engine : public Singleton<Engine> {
  public:
    Engine();
    ~Engine();

    lsquic_engine_t *GetEngine(EngineType type);

  private:
    EnginePtr m_client;
    EnginePtr m_server;
};

} // namespace quic

class Quic : public Connection {
  public:
    Quic(const std::string &address, unsigned short port, bool server)
        : Connection(address, port, server){};

    virtual void SendData(const buffer_t &data) const;
};

} // namespace onion
