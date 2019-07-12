#include <experimental/coroutine>
#include <future>
#include <iostream>

// built-in Future
std::future<int> StdFuture(int x) {
  co_return x;
}

// user-defined Future
template <typename T>
struct Future {
  struct promise_type;
  using handle = std::experimental::coroutine_handle<promise_type>;

  struct promise_type {
    T promise_value;

    auto get_return_object() {
      std::cout << "get_return_object" << std::endl;
      return Future<T>{handle::from_promise(*this)};
    }

    auto initial_suspend() {
      std::cout << "initial_suspend" << std::endl;
      return std::experimental::suspend_never{};
    }

    auto final_suspend() {
      std::cout << "final_suspend" << std::endl;
      return std::experimental::suspend_always{};
    }

    void unhandled_exception() {
      std::cout << "unhandled_exception" << std::endl;
      std::terminate();
    }

    void return_value(const T& value) {
      std::cout << "return_value: " << value << std::endl;
      promise_value = value;
    }
  };

  const T& get() const { return coro_.promise().promise_value; }

 private:
  Future(handle h) : coro_(h) {}
  handle coro_;
};

// user-defined Generator
template <typename T>
struct Generator {
  struct promise_type;
  using handle = std::experimental::coroutine_handle<promise_type>;

  struct promise_type {
    T promise_value;

    auto get_return_object() {
      std::cout << "get_return_object" << std::endl;
      return Generator{handle::from_promise(*this)};
    }

    auto initial_suspend() {
      std::cout << "initial_suspend" << std::endl;
      return std::experimental::suspend_always{};
    }

    auto final_suspend() {
      std::cout << "final_suspend" << std::endl;
      return std::experimental::suspend_always{};
    }

    void unhandled_exception() {
      std::cout << "unhandled_exception" << std::endl;
      std::terminate();
    }

    auto yield_value(const T& value) {
      std::cout << "yield_value: " << value << std::endl;
      promise_value = value;
      return std::experimental::suspend_always{};
    }
  };

  const T& get() const { return coro_.promise().promise_value; }

  bool next() {
    if (!coro_)
      return false;

    coro_.resume();
    return !coro_.done();
  }

 private:
  Generator(handle h) : coro_(h) {}
  handle coro_;
};

Future<int> MyFuture() {
  std::cout << "MyFuture" << std::endl;
  co_return co_await StdFuture(42);
}

Generator<int> MyGenerator() {
  std::cout << "MyGenerator" << std::endl;
  co_yield co_await StdFuture(1);
  co_yield co_await StdFuture(2);
  co_yield co_await StdFuture(3);
}

int main() {
  std::cout << "Future:" << std::endl;
  std::cout << "> " << MyFuture().get() << std::endl;

  std::cout << std::endl;

  std::cout << "Generator:" << std::endl;
  auto generator = MyGenerator();
  while (generator.next()) {
    std::cout << "> " << generator.get() << std::endl;
  }

  return 0;
}
