cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(sechatr)

find_package(QHttpEngine 1.0.0 REQUIRED)
find_package(Qt5WebSockets 5.4 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)
