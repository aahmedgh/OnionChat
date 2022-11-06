#pragma once

#include <mutex>
#include <sstream>
#include <string>

#include "singleton.h"

namespace onion {

class Logger : public Singleton<Logger> {
  public:
    void Log(const std::string &level, const std::string &component,
             const std::string &file, const std::string &func,
             const std::string &msg);

  private:
    static unsigned int GetPid();
    static unsigned int GetTid();

    std::mutex m_lock;
};

// Logging macros
#ifdef DEBUG
#define LOG(level, component, msg)                                             \
    Logger::Get().Log(                                                         \
        level, component, __FILE__, __func__,                                  \
        (static_cast<std::stringstream &>(std::stringstream().flush() << msg)) \
            .str());
#define TRACE_LOG(component, msg) LOG("TRACE", #component, msg)
#define DEBUG_LOG(component, msg) LOG("DEBUG", #component, msg)
#define INFO_LOG(component, msg) LOG("INFO", #component, msg)
#define WARNING_LOG(component, msg) LOG("WARNING", #component, msg)
#define ERROR_LOG(component, msg) LOG("ERROR", #component, msg)
#else
#define LOG(level, component, msg)
#define TRACE_LOG(component, msg)
#define DEBUG_LOG(component, msg)
#define INFO_LOG(component, msg)
#define WARNING_LOG(component, msg)
#define ERROR_LOG(component, msg)
#endif

} // namespace onion
