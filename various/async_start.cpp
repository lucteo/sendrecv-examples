#include <unifex/then.hpp>
#include <unifex/static_thread_pool.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/submit.hpp>

#include <iostream>
#include <utility>
#include <thread>
#include <chrono>

using namespace unifex;
using std::forward;
using namespace std::chrono_literals;

template<typename Sched, typename F>
auto run_on(Sched &&sched, F &&f) {
    return then(schedule(forward<Sched>(sched)), forward<F>(f));
}

template <typename Snd>
void run(Snd&& snd) {
    struct empty_recv {
        void set_value() {}
        void set_done() noexcept {}
        void set_error(std::exception_ptr) noexcept {}
    };
    submit(std::move(snd), empty_recv{});
}

int main() {
    static_thread_pool my_pool{4};

    {
        auto s = run_on(my_pool.get_scheduler(), [] { printf("Hello, concurrent world!\n"); });
        run(std::move(s));
    }

    return 0;
}