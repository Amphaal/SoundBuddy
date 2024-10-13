#pragma once

#include <array>
#include <functional>
#include <initializer_list>
#include <stdexcept>

using CallbackT = std::function<void()>;

template <std::size_t stack_depth = 4> class Defer {
public:
  Defer() : callbacks{}, num_callbacks{0} {}
  template <typename T, typename... Rest>
  Defer(T arg, Rest... rest)
      : callbacks{arg, rest...}, num_callbacks{sizeof...(Rest) + 1} {}
  ~Defer() {
    if (!num_callbacks) {
      return;
    }
    for (auto idx = num_callbacks; idx > 0; --idx) {
      callbacks[idx - 1]();
    }
  }
  void defer(CallbackT callback) {
    if (num_callbacks >= callbacks.max_size()) {
      throw std::runtime_error("Number of deferred functions exceeded.");
    }
    callbacks[num_callbacks++] = callback;
  }

private:
  std::array<CallbackT, stack_depth> callbacks;
  std::size_t num_callbacks;
};
