add_library(coros-fcgi STATIC 
    src/coros_fcgi/channel/channel.cpp
    src/coros_fcgi/pipe/pipe.cpp
    src/coros_fcgi/pipe/awaiter.cpp
    src/coros_fcgi/pipe/receive_awaiter.cpp
    src/coros_fcgi/pipe/send_awaiter.cpp
    src/coros_fcgi/channel/response.cpp
    src/coros_fcgi/commons/integer.cpp
    src/coros_fcgi/record/header.cpp
    src/coros_fcgi/record/processor.cpp
    src/coros_fcgi/app.cpp
    src/coros_fcgi/handler.cpp)
target_include_directories(coros-fcgi PUBLIC src)
target_link_libraries(coros-fcgi PRIVATE coros)
