#include "common.hpp"
#include <unifex/typed_via.hpp>

#include <string>
#include <string_view>

struct HttpResponse {
    int status_code_;
    std::string status_line_{};
    std::string body_{};
};

struct http_client {
    static_thread_pool pool_{4};

    auto start_async(const char* url) {
        return schedule(pool_.get_scheduler()) |
               then([s = std::string{url}] { return process_http_request_impl(s); });
    }

    static HttpResponse process_http_request_impl(const std::string& url) {
        if (url.find("mutex") != std::string::npos) {
            // Minimal processing
            std::this_thread::sleep_for(10ms);
            // Respond with failure
            return HttpResponse{404, "Not found", "Mutexes are not found in user code."};
        } else {
            // Do some processing
            std::this_thread::sleep_for(100ms);
            // Respond with success
            return HttpResponse{200, "OK", "There is a better way do do concurrency."};
        }
    }
};

void print_response(HttpResponse&& resp) {
    printf("Response received: %d %s => %s\n", resp.status_code_, resp.status_line_.c_str(),
            resp.body_.c_str());
}

void do_something_else() {
    for (int i = 0; i < 10; i++) {
        printf(".");
        std::this_thread::sleep_for(10ms);
    }
    printf("\n");
}

int main() {
    {
        static_thread_pool my_pool{4};
        auto sched = my_pool.get_scheduler();

        http_client cln;

        const char* uri = "google.com?q=mutex";
        printf("Starting request: %s\n", uri);
        auto s = cln.start_async(uri) | typed_via(sched) | then(print_response);
        start_detached(std::move(s));
        spawn(sched, do_something_else);
    }

    printf("---\n");

    {
        static_thread_pool my_pool{4};
        auto sched = my_pool.get_scheduler();

        http_client cln;

        const char* uri = "google.com?q=concurrency";
        printf("Starting request: %s\n", uri);
        auto s = cln.start_async(uri) | typed_via(sched) | then(print_response);
        start_detached(std::move(s));
        spawn(sched, do_something_else);
    }

    printf("\n");
    return 0;
}
