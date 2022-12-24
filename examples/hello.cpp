#include <iostream>

#include "coros_fcgi/app.h"
#include "coros_fcgi/channel/channel.h"
#include "coros/async/thread_pool.h"
#include "coros/event/monitor.h"
#include "coros/network/server.h"

class HelloHandler : public coros::fcgi::FcgiHandler {
    public:
        coros::base::Future handle_request(coros::fcgi::Channel& channel) {
            std::cout << "Received request..." << std::endl;
            auto& variables = channel.variables;
            for (auto it = variables.begin(); it != variables.end(); ++it) {
                std::cout << it->first << ": " << it->second << std::endl;
            }
            co_return;
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
