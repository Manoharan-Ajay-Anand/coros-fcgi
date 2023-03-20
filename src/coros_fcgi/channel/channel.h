#ifndef COROS_FCGI_CHANNEL_CHANNEL_H
#define COROS_FCGI_CHANNEL_CHANNEL_H

#include "coros_fcgi/pipe/pipe.h"
#include "coros_fcgi/application/response.h"

#include <string>
#include <unordered_map>

namespace coros::base {
    class ThreadPool;

    class Socket;
}

namespace coros::fcgi {
    struct Channel {
        Pipe fcgi_variables;
        Pipe fcgi_stdin;
        Pipe fcgi_data;
        Response response;
        Channel(int request_id, base::Socket& socket,
                bool keep_conn, base::ThreadPool& thread_pool);
    };
}

#endif
