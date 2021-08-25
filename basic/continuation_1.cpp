#include "common.hpp"

#include <string>

struct person {
    std::string name_;
    static_thread_pool pool_{4};

    explicit person(const char* name)
        : name_(name) {}

    auto sms(const char* from, const char* message) {
        printf("%s is being texted by %s (message: %s)\n", name_.c_str(), from, message);
        printf("    continuing current work...\n");

        // Continue working, and then call back
        auto f = [=] {
            // Finish current work first
            std::this_thread::sleep_for(100ms);

            printf("%s is now ready to respond to %s\n", name_.c_str(), from);
        };
        auto sched = pool_.get_scheduler();
        return schedule(sched) | then(std::move(f));
    }
};

int main() {
    person p1("Alice");

    // Try contacting
    auto callback = []() {
        printf("\nCalling back\n");
        std::this_thread::sleep_for(10ms);
    };
    auto s = p1.sms("Bob", "Hey, are you free?") | then(callback);
    start_detached(std::move(s));

    // Do something else while waiting for the message
    {
        for (int i = 0; i < 10; i++) {
            printf(".");
            std::this_thread::sleep_for(15ms);
        }
    }

    printf("\n");
    return 0;
}
