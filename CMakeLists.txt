cmake_minimum_required(VERSION 3.1)

project(Coros-FCGI
    VERSION 1.0
    DESCRIPTION "FCGI server based on Coros"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_library(coros-fcgi STATIC 
    src/coros_fcgi/channel/channel.cpp
    src/coros_fcgi/channel/pipe.cpp
    src/coros_fcgi/channel/response.cpp
    src/coros_fcgi/commons/integer.cpp
    src/coros_fcgi/record/header.cpp
    src/coros_fcgi/record/processor.cpp
    src/coros_fcgi/app.cpp
    src/coros_fcgi/handler.cpp)
target_include_directories(coros-fcgi PUBLIC src)
target_link_libraries(coros-fcgi PRIVATE coros)
