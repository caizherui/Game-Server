cmake_minimum_required(VERSION 3.5)

project(serverAu)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server main.cpp
src/proto/example.pb.cc src/server.cpp src/epoll.cpp src/http_request.cpp src/sql_pool.cpp
${TS_FILES}    
)

INCLUDE_DIRECTORIES(./src ./src/proto)

find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})

target_link_libraries(server pthread mysqlcppconn mysqlclient ${PROTOBUF_LIBRARIES})