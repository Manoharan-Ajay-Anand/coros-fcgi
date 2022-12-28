#ifndef COROS_FCGI_RECORD_HEADER_H
#define COROS_FCGI_RECORD_HEADER_H

#include "coros/async/future.h"

namespace coros::base {
    class Socket;
}

namespace coros::fcgi {
    enum ProtocolVersion {
        FCGI_VERSION_1 = 1
    };

    enum ProtocolLength {
        FCGI_HEADER_LEN  = 8,
        FCGI_BEGIN_REQUEST_LEN = 8,
        FCGI_END_REQUEST_LEN = 8,
        FCGI_ALIGNMENT_LEN = 8
    };

    enum BeginRequestFlag {
        FCGI_KEEP_CONN = 1
    };

    enum RecordType {
        FCGI_BEGIN_REQUEST = 1,
        FCGI_ABORT_REQUEST,
        FCGI_END_REQUEST,
        FCGI_PARAMS,
        FCGI_STDIN,
        FCGI_STDOUT,
        FCGI_STDERR,
        FCGI_DATA,
        FCGI_GET_VALUES,
        FCGI_GET_VALUES_RESULT,
        FCGI_UNKNOWN_TYPE
    };

    enum ProtocolRole {
        FCGI_RESPONDER = 1,
        FCGI_AUTHORIZER,
        FCGI_FILTER
    };

    enum ProtocolStatus {
        FCGI_REQUEST_COMPLETE,
        FCGI_CANT_MPX_CONN,
        FCGI_OVERLOADED,
        FCGI_UNKNOWN_ROLE
    };

    struct RecordHeader {
        ProtocolVersion version;
        RecordType type;
        int request_id;
        int content_length;
        int padding_length;

        RecordHeader();
        RecordHeader(ProtocolVersion version, RecordType type, int request_id, int content_length);
        base::AwaitableFuture parse(base::Socket& socket);
        base::AwaitableFuture serialize(base::Socket& socket);
        base::AwaitableFuture pad(base::Socket& socket);
    };
}

#endif
