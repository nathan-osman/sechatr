cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(sechatr)

find_package(Qt5WebSockets 5.4 REQUIRED)

set(CMAKE_AUTOMOC ON)

set(SRC
    main.cpp
    websocketserver.cpp
)

add_executable(sechatr ${SRC})
set_target_properties(sechatr PROPERTIES CXX_STANDARD 11)

target_link_libraries(sechatr Qt5::Network Qt5::WebSockets)

install(TARGETS sechatr RUNTIME DESTINATION bin)
