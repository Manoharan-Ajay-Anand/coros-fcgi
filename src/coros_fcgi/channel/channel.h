#ifndef COROS_FCGI_CHANNEL_CHANNEL_H
#define COROS_FCGI_CHANNEL_CHANNEL_H

#include "pipe.h"

#include <string>
#include <unordered_map>

namespace coros::base {
    class ThreadPool;
}

namespace coros::fcgi {
    struct Channel {
        std::unordered_map<std::string, std::string> variables;
        Pipe fcgi_stdin;
        Pipe fcgi_data;
        Channel(base::ThreadPool& thread_pool);
    };
}

#endif
