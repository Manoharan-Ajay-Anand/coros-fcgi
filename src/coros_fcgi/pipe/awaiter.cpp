#include "awaiter.h"

#include <optional>
#include <coroutine>

std::coroutine_handle<> 
        coros::fcgi::get_resume_handle(std::optional<std::coroutine_handle<>>& handle_opt) {
    std::coroutine_handle<> resume_handle = std::noop_coroutine();
    if (handle_opt) {
        resume_handle = handle_opt.value();
        handle_opt.reset();
    }
    return resume_handle;
}