#include "channel.h"
#include "coros/async/thread_pool.h"

coros::fcgi::Channel::Channel(int request_id, base::Socket& socket, bool keep_conn, 
                              base::ThreadPool& thread_pool)
        : fcgi_variables(thread_pool, socket), fcgi_stdin(thread_pool, socket), 
          fcgi_data(thread_pool, socket), response(request_id, socket, keep_conn) {
}
