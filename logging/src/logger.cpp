#include "logger.h"

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace onion {

void Logger::Log(const std::string &level, const std::string &component,
                 const std::string &file, const std::string &func,
                 const std::string &msg) {
    // TODO: Use a more complex version later (i.e. Windows DbgView, Linux
    // syslog or stdout)
    std::lock_guard<std::mutex> guard(m_lock);
    std::cout << level << "|" << GetPid() << "|" << GetTid() << "|" << component
              << "|" << file << "|" << func << "|" << msg << std::endl;
}

unsigned int Logger::GetPid() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

unsigned int Logger::GetTid() {
#ifdef _WIN32
    return GetCurrentThreadId() :
#else
    return gettid();
#endif
}

} // namespace onion
