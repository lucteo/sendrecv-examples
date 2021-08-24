#include "common.hpp"

#include <string>
#include <string_view>

template <typename Sched, typename F>
void spawn(Sched&& sched, F&& f) {
    auto snd = run_on(forward<Sched>(sched), forward<F>(f));
    start_detached(std::move(snd));
}

void print_message_task(const char* msg) {

    printf(" %s", msg);

    std::this_thread::sleep_for(100ms);
}

struct HttpResponse {
    int status_code_;
    std::string status_line_{};
    std::string body_{};
};

using response_callback = std::function<void(HttpResponse)>;

template <typename Sched>
void invoke_callback(Sched&& sched, response_callback cb, HttpResponse resp) {
    spawn(forward<Sched>(sched), [cb = std::move(cb), resp = std::move(resp)]() mutable {
        std::move(cb)(std::move(resp));
    });
}

template <typename Sched>
void make_http_request(std::string url, Sched&& sched, response_callback cb) {
    spawn(sched, [=] {
        if (url.find("mutex") != std::string::npos) {
            // Minimal processing
            std::this_thread::sleep_for(10ms);
            // Respond with failure
            auto resp = HttpResponse{404, "Not found", "Mutexes are not found in user code."};
            invoke_callback(std::forward<Sched>(sched), std::move(cb), std::move(resp));
        } else {
            // Do some processing
            std::this_thread::sleep_for(100ms);
            // Respond with success
            auto resp = HttpResponse{200, "OK", "There is a better way do do concurrency."};
            invoke_callback(std::forward<Sched>(sched), std::move(cb), std::move(resp));
        }
    });
}

void my_callback(HttpResponse&& resp) {
    printf("Response received: %d %s => %s\n", resp.status_code_, resp.status_line_.c_str(),
            resp.body_.c_str());
}

void do_something_else() {
    for (int i = 0; i < 10; i++) {
        printf(".");
        std::this_thread::sleep_for(100us);
    }
    printf("\n");
}

int main() {
    {
        static_thread_pool my_pool{4};
        auto sched = my_pool.get_scheduler();

        // Make a first request
        auto f1 = [=] {
            const char* uri = "google.com?q=mutex";
            printf("Making request: %s\n", uri);

            make_http_request(uri, sched, my_callback);
        };
        spawn(sched, f1);
        spawn(sched, do_something_else);
    }

    printf("---\n");

    {
        static_thread_pool my_pool{4};
        auto sched = my_pool.get_scheduler();

        auto f2 = [=] {
            const char* uri = "google.com?q=concurrency";
            printf("Making request: %s\n", uri);

            make_http_request(uri, sched, my_callback);
        };
        spawn(sched, f2);
        spawn(sched, do_something_else);
    }

    printf("\n");
    return 0;
}
