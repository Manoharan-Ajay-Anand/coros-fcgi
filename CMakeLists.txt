cmake_minimum_required(VERSION 3.1)

project(Coros-FCGI
    VERSION 1.0
    DESCRIPTION "FCGI server based on Coros"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_subdirectory(libraries/coros)

add_library(coros-fcgi STATIC 
    src/coros_http/app.cpp
    src/coros_http/common/commons.cpp)
target_include_directories(coros-fcgi PUBLIC src)
target_link_libraries(coros-fcgi PUBLIC coros)

add_executable(hello EXCLUDE_FROM_ALL examples/hello.cpp)
target_link_libraries(hello PRIVATE coros-fcgi)
