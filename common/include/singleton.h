#pragma once

// Implementation of Scott Meyer's singleton (thread safe on C++11 and newer)
// Thread safe as of C++11
namespace onion {

template <typename T> class Singleton {
  public:
    static T &Get() {
        static T instance;
        return instance;
    }

  protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
};

} // namespace onion
