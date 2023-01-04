cmake_minimum_required(VERSION 3.22)
project(RestAPI)

set(CMAKE_CXX_STANDARD 20)

# TODO this should be updated to the libraries are located dynamically. This approach does not offer cross-platform support
include_directories(/usr/local/mysql-connector-c++-8.0.31/include)
include_directories(/usr/local/include)
link_directories(/usr/local/mysql-connector-c++-8.0.31/lib64)

add_executable(RestAPI main.cpp database.cpp database.h requesthandler.cpp requesthandler.h)
target_link_libraries(${PROJECT_NAME} mysqlcppconn crypto ssl)
