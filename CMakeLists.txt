cmake_minimum_required(VERSION 3.5)

project(serverAu)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server main.cpp
src/server.cpp src/epoll.cpp src/http_request.cpp
${TS_FILES}    
)

INCLUDE_DIRECTORIES(./src)


target_link_libraries(server pthread mysqlcppconn mysqlclient)