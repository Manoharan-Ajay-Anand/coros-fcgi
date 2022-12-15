#ifndef COROS_FCGI_RECORD_PROCESSOR_H
#define COROS_FCGI_RECORD_PROCESSOR_H

#include "coros/async/future.h"

#include <memory>
#include <unordered_map>

namespace coros::base {
    class Socket;
}

namespace coros::fcgi {
    struct RecordHeader;

    class ApplicationChannel;

    class RecordProcessor {
        private:
            std::unordered_map<int, std::unique_ptr<ApplicationChannel>> channel_map;
            base::AwaitableFuture begin_request(RecordHeader& header, base::Socket& socket);
            base::AwaitableFuture set_param(RecordHeader& header, base::Socket& socket);
        public:
            base::AwaitableFuture process(RecordHeader& header, base::Socket& socket);
    };
}

#endif
