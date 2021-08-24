#include "common.hpp"

#include <string>

template <typename Sched, typename F>
void spawn(Sched&& sched, F&& f) {
    auto snd = run_on(forward<Sched>(sched), forward<F>(f));
    start_detached(std::move(snd));
}

struct person {
    std::string name_;
    static_thread_pool pool_{4};

    explicit person(const char* name, bool busy)
        : name_(name) {}

    void sms(const char* from, const char* message, std::function<void()> callback) {
        printf("%s is being texted by %s (message: %s)\n", name_.c_str(), from, message);
        printf("    continuing current work...\n");

        // Continue working, and then call back
        auto sched = pool_.get_scheduler();
        spawn(sched, [=] {
            // Finish current work first
            std::this_thread::sleep_for(100ms);

            printf("%s is now ready to respond to %s\n", name_.c_str(), from);
            // Now call back
            spawn(sched, callback);
        });
    }
};

int main() {
    person p1("Alice", true);

    // Try contacting
    auto callback = []() {
        printf("\nCalling back\n");
        std::this_thread::sleep_for(10ms);
    };
    p1.sms("Bob", "Hey, are you free?", callback);

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
