#include <iostream>

#include "coros/async/thread_pool.h"
#include "coros/event/monitor.h"
#include "coros/network/server.h"

#include "coros_fcgi/app.h"
#include "coros_fcgi/channel/channel.h"
#include "coros_fcgi/channel/response.h"

class HelloHandler : public coros::fcgi::FcgiHandler {
    public:
        coros::base::Future handle_request(coros::fcgi::Channel& channel) {
            coros::fcgi::Response& response = channel.response;
            std::string content_len("content-length: 18");
            std::string content_type("content-type: text/html\r\n");
            std::string content("welcome to fastcgi");
            co_await response.println(content_len);
            co_await response.println(content_type);
            co_await response.print(content);
            co_await response.flush();
        }
};

void start_server(coros::base::Server& server) {
    try {
        server.setup();
        server.start(true);
        std::getchar();
        server.shutdown();
    } catch (std::runtime_error error) {
        std::cerr << error.what() << std::endl;
    }
}

int main() {
    coros::base::ThreadPool thread_pool;
    coros::base::SocketEventMonitor event_monitor;
    HelloHandler handler;
    coros::fcgi::FcgiApplication fcgi_app(thread_pool, handler);
    coros::base::Server hello_server(9000, fcgi_app, event_monitor, thread_pool);
    std::cout << "Starting FCGI Server..." << std::endl;
    start_server(hello_server);
    return 0;
}
