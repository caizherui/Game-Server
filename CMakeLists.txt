cmake_minimum_required(VERSION 3.5)

project(serverAu)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server main.cpp
src/proto/dataParse.pb.cc src/server.cpp src/epoll.cpp src/http_request.cpp src/sql_pool.cpp src/room.cpp src/player.cpp src/battle.cpp src/skill.cpp
)

INCLUDE_DIRECTORIES(./src ./src/proto)

find_package(Protobuf REQUIRED)

# 手动指定 JsonCpp 的路径
set(JsonCpp_INCLUDE_DIR /home/czr/jsonc/jsoncpp/include)
set(JsonCpp_LIBRARY_DIR /home/czr/jsonc/jsoncpp/build/lib)
# 查找 JsonCpp 头文件
find_path(JsonCpp_INCLUDE_DIRS
  NAMES json/json.h
  HINTS ${JsonCpp_INCLUDE_DIR}
)

# 查找 JsonCpp 库
find_library(JsonCpp_LIBRARIES
  NAMES jsoncpp
  HINTS ${JsonCpp_LIBRARY_DIR}
)

find_package(OpenSSL REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR}  ${JsonCpp_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

target_link_libraries(server pthread mysqlcppconn mysqlclient ${PROTOBUF_LIBRARIES}  ${JsonCpp_LIBRARIES} ${OPENSSL_LIBRARIES})