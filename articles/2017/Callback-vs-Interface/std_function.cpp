// Shahar Mike, 2016
// https://shaharmike.com/cpp/naive-std-function/

#include <memory>

template <typename T>
class naive_function;

template <typename ReturnValue, typename... Args>
class naive_function<ReturnValue (Args...)> {
public:
    template <typename T>
    naive_function& operator=(T t) {
        callable_ = std::make_unique<CallableT<T>> (t);
        return *this;
    }

    ReturnValue operator()(Args... args) const {
        if (callable_)
            return callable_->Invoke (std::forward<Args> (args)...);
    }

private:
    class ICallable {
    public:
        virtual ~ICallable () = default;
        virtual ReturnValue Invoke (Args...) = 0;
    };

    template <typename T>
    class CallableT : public ICallable {
    public:
        CallableT (const T& t)
            : t_ (t) {}

        ReturnValue Invoke (Args... args) override {
            return t_ (std::forward<Args> (args)...);
        }

    private:
        T t_;
    };

    std::unique_ptr<ICallable> callable_;
};

#include <iostream>
#include <functional>

void func () {
    std::cout << "func" << std::endl;
}

struct container {
    void mem_func () {
        std::cout << "mem_func" << std::endl;
    }
};

struct functor {
    void operator()() {
        std::cout << "functor" << std::endl;
    }
};

int main () {
    naive_function<void ()> f;
    f = func;
    f ();
    f = std::bind (&container::mem_func, new container);
    f ();
    f = functor ();
    f ();
    f = [] { std::cout << "lambda" << std::endl; };
    f ();
    return 0;
}
