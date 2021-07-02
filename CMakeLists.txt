cmake_minimum_required(VERSION 3.17)
project(Webserver)

set(CMAKE_CXX_STANDARD 11)

add_executable(Webserver main.cpp cmake-build-debug/lock.h cmake-build-debug/block_queue.h cmake-build-debug/threadpool.h cmake-build-debug/list_timer.h cmake-build-debug/list_timer.cpp cmake-build-debug/http_conn.h cmake-build-debug/http_conn.cpp cmake-build-debug/webserver.h cmake-build-debug/Webserver.cpp cmake-build-debug/connection_pool.h cmake-build-debug/connection_pool.cpp)