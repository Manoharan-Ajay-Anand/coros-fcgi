#ifndef COROS_FCGI_RECORD_HEADER_H
#define COROS_FCGI_RECORD_HEADER_H

#define FCGI_HEADER_LEN  8

#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

#define FCGI_NULL_REQUEST_ID     0

#include "coros/async/future.h"

namespace coros::base {
    class Socket;
}

namespace coros::fcgi {
    struct RecordHeader {
        int version;
        int type;
        int request_id;
        int content_length;
        int padding_length;

        base::AwaitableFuture parse(base::Socket& socket);
    };
}

#endif
