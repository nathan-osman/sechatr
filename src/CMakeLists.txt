set(SRC
    main.cpp
    resources.qrc
    websocketserver.cpp
)

add_executable(sechatr ${SRC})
set_target_properties(sechatr PROPERTIES CXX_STANDARD 11)

target_link_libraries(sechatr Qt5::Network Qt5::WebSockets)

install(TARGETS sechatr RUNTIME DESTINATION bin)
