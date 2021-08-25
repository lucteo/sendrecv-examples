#include "common.hpp"

template <typename Sched, typename F>
void spawn(Sched&& sched, F&& f) {
    auto snd = run_on(forward<Sched>(sched), forward<F>(f));
    start_detached(std::move(snd));
}

int main() {
    static_thread_pool my_pool{4};

    auto sched = my_pool.get_scheduler();
    spawn(sched, [] { printf(" How"); });
    spawn(sched, [] { printf(" did"); });
    spawn(sched, [] { printf(" the"); });
    spawn(sched, [] { printf(" multi-threaded"); });
    spawn(sched, [] { printf(" chicken"); });
    spawn(sched, [] { printf(" cross"); });
    spawn(sched, [] { printf(" the"); });
    spawn(sched, [] { printf(" road"); });
    spawn(sched, [] { printf(" ?"); });

    return 0;
}
