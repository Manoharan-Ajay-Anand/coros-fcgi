#ifndef COROS_FCGI_CHANNEL_RESPONSE_H
#define COROS_FCGI_CHANNEL_RESPONSE_H

#include "coros/async/future.h"

#include <cstddef>
#include <string>

namespace coros::base {
    class Socket;
}

namespace coros::fcgi {
    class Response {
        private:
            int request_id;
            base::Socket& socket;
            bool keep_conn;
        public:
            Response(int request_id, base::Socket& socket, bool keep_conn);
            base::AwaitableFuture write(std::byte* data, int size);
            base::AwaitableFuture println(std::string s);
            base::AwaitableFuture print(std::string s);
            base::AwaitableFuture flush();
    };
}

#endif
