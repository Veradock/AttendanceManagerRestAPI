cmake_minimum_required(VERSION 3.22)
project(RestAPI)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_INSTALL_PREFIX}/mysql-connector-c++/include ${CMAKE_INSTALL_PREFIX}/include)

find_library(CRYPTO_PATH crypto HINTS ${CMAKE_INSTALL_PREFIX}/mysql-connector-c++/lib REQUIRED)
find_library(SQL_PATH mysqlcppconn HINTS ${CMAKE_INSTALL_PREFIX}/mysql-connector-c++/lib REQUIRED)
find_library(SSL_PATH ssl HINTS ${CMAKE_INSTALL_PREFIX}/mysql-connector-c++/lib REQUIRED)

add_executable(RestAPI main.cpp database.cpp database.h requesthandler.cpp requesthandler.h config.cpp config.h)
target_link_libraries(${PROJECT_NAME} ${SQL_PATH} ${CRYPTO_PATH} ${SSL_PATH})
