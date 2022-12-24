#include "channel.h"
#include "coros/async/thread_pool.h"

coros::fcgi::Channel::Channel(coros::base::ThreadPool& thread_pool): fcgi_stdin(thread_pool), 
                                                                     fcgi_data(thread_pool) {
}

