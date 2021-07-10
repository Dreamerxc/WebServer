cmake_minimum_required(VERSION 3.17)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)
#file(GLOB SRC_LIST "common/*.cpp" "common/*.h" "http/*.h" "http/*.cpp" "pool/*.h" "pool/*.cpp" "webserver.h" "config.*")
#add_library(demo ${SRC_LIST})

add_executable(Webserver main.cpp Webserver.cpp config.cpp pool/connection_pool.cpp pool/threadpool.h common/list_timer.cpp http/http_conn.cpp)
find_package(Threads)
include_directories("/usr/include/mysql")
link_directories("usr/lib/x86_64-linux-gnu")

set(MYSQL_LIBS
        mysqlclient pthread z m rt atomic ssl crypto dl
        )
target_link_libraries(Webserver ${CMAKE_THREAD_LIBS_INIT} ${MYSQL_LIBS})