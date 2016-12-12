set(SRC
    client.cpp
    coordinator.cpp
    httpserver.cpp
    proxy.cpp
    room.cpp
    websocketserver.cpp
    main.cpp
    resources.qrc
)

add_executable(sechatr ${SRC})
set_target_properties(sechatr PROPERTIES CXX_STANDARD 11)

target_link_libraries(sechatr Qt5::Network Qt5::WebSockets QHttpEngine)

install(TARGETS sechatr RUNTIME DESTINATION bin)
