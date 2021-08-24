#include "common.hpp"

int main() {
    static_thread_pool my_pool{4};

    {
        auto s = run_on(my_pool.get_scheduler(), [] { printf("Hello, concurrent world!\n"); });
        start_detached(std::move(s));
    }

    return 0;
}
