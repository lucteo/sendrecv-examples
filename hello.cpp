#include "common.hpp"

int main() {
    static_thread_pool my_pool{4};

    auto s = run_on(my_pool.get_scheduler(), [] { printf("Hello, concurrent world!\n"); });
    sync_wait(s);

    return 0;
}
