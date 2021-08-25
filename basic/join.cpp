#include "common.hpp"
#include <unifex/when_all.hpp>

void do_work_1() {
    std::this_thread::sleep_for(100ms);
    printf("We have some work\n");
}

void do_work_2() {
    std::this_thread::sleep_for(100ms);
    printf("Competition is also doing work\n");
}

void do_work_3() {
    std::this_thread::sleep_for(100ms);
    printf("And there are some other things to be done\n");
}

void done_work() {
    printf("-\n");
    printf("All the work is done. Have a good night!\n");
    std::this_thread::sleep_for(100ms);
}

int main() {
    static_thread_pool my_pool{4};
    auto sched = my_pool.get_scheduler();

    auto s1 = schedule(sched) | then(do_work_1);
    auto s2 = schedule(sched) | then(do_work_2);
    auto s3 = schedule(sched) | then(do_work_3);

    auto s = when_all(s1, s2, s3) | then([](...) { done_work(); });
    start_detached(std::move(s));

    return 0;
}
