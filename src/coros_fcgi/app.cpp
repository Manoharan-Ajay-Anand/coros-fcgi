#include "app.h"
#include "handler.h"

#include "coros/async/future.h"
#include "coros/async/thread_pool.h"
#include "coros/network/server.h"
#include "coros/network/socket.h"

#include "coros_fcgi/channel/channel.h"
#include "coros_fcgi/record/header.h"
#include "coros_fcgi/record/processor.h"

#include <iostream>
#include <stdexcept>

coros::fcgi::FcgiApplication::FcgiApplication(base::ThreadPool& thread_pool, 
                                              FcgiHandler& fcgi_handler): processor(thread_pool, 
                                                                                    fcgi_handler) {
}

coros::base::Future coros::fcgi::FcgiApplication::on_request(base::Server& server, 
                                                             std::shared_ptr<base::Socket> socket) {
    try {
        std::cout << "Received connection" << std::endl;
        while (true) {
            RecordHeader header;
            co_await header.parse(*socket);
            co_await processor.process(header, *socket);
        }
    } catch (std::runtime_error error) {
        std::cout << error.what() << std::endl;
    }
    socket->close_socket();
}
