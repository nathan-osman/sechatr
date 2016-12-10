set(SRC
    resources.qrc
    httpserver.cpp
    websocketserver.cpp
    main.cpp
)

add_executable(sechatr ${SRC})
set_target_properties(sechatr PROPERTIES CXX_STANDARD 11)

target_link_libraries(sechatr QHttpEngine Qt5::WebSockets)

install(TARGETS sechatr RUNTIME DESTINATION bin)
