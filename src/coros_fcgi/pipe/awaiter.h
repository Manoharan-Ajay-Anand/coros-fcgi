#ifndef COROS_FCGI_PIPE_AWAITER_H
#define COROS_FCGI_PIPE_AWAITER_H

#include <optional>
#include <coroutine>

namespace coros::fcgi {
    std::coroutine_handle<> get_resume_handle(std::optional<std::coroutine_handle<>>& handle_opt);
}

#endif
