#include "logger.h"

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <pthread.h>
#endif

namespace onion {

void Logger::Log(const std::string &level, const std::string &component, const std::string &file,
                 const std::string &func, const std::string &msg) {
    // TODO: Use a more complex version later (i.e. Windows DbgView, Linux
    // syslog or stdout)
    std::lock_guard<std::mutex> guard(m_lock);
    std::cout << level << "|" << GetPid() << "|" << GetTid() << "|" << component << "|" << file << "|" << func << "|"
              << msg << std::endl;
}

unsigned int Logger::GetPid() {
#ifndef _WIN32
    return getpid();
#else
    return GetCurrentProcessId();
#endif
}

unsigned int Logger::GetTid() {
#ifdef _WIN32
    return GetCurrentThreadId();
#elif defined(__APPLE__)
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
#else
    return gettid();
#endif
}

} // namespace onion
