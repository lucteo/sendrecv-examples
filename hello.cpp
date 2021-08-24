#include <unifex/then.hpp>
#include <unifex/static_thread_pool.hpp>
#include <unifex/sync_wait.hpp>

#include <iostream>
#include <utility>

using namespace unifex;
using std::forward;

template<typename Sched, typename F>
auto run_on(Sched &&sched, F &&f) {
    return then(schedule(forward<Sched>(sched)), forward<F>(f));
}

int main() {
    static_thread_pool my_pool{4};

    auto s = run_on(my_pool.get_scheduler(), [] { printf("Hello, concurrent world!\n"); });
    sync_wait(s);

    return 0;
}
