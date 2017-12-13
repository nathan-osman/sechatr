set(SRC
    client.h
    client.cpp
    coordinator.h
    coordinator.cpp
    httpserver.h
    httpserver.cpp
    poll.h
    poll.cpp
    pollmanager.h
    pollmanager.cpp
    room.h
    room.cpp
    websocketserver.h
    websocketserver.cpp
    main.cpp
    resources.qrc
)

add_executable(sechatr ${SRC})
set_target_properties(sechatr PROPERTIES CXX_STANDARD 11)

target_link_libraries(sechatr Qt5::Network Qt5::WebSockets qhttpengine)

install(TARGETS sechatr RUNTIME DESTINATION bin)
