# qtPilot MCP Server (C++)

set(MCP_SERVER_SOURCES
    main.cpp
    mcp/mcp_server.cpp
    mcp/tool_registry.cpp
    client/probe_client.cpp
    client/discovery_listener.cpp
    state/server_state.cpp
    tools/native_tools.cpp
    tools/cu_tools.cpp
    tools/chrome_tools.cpp
    tools/discovery_tools.cpp
    tools/logging_tools.cpp
    tools/recording_tools.cpp
    logging/message_logger.cpp
    logging/event_recorder.cpp
    download/downloader.cpp
)

set(MCP_SERVER_HEADERS
    mcp/mcp_server.h
    mcp/tool_registry.h
    client/probe_client.h
    client/discovery_listener.h
    state/server_state.h
    tools/native_tools.h
    tools/cu_tools.h
    tools/chrome_tools.h
    tools/discovery_tools.h
    tools/logging_tools.h
    tools/recording_tools.h
    logging/message_logger.h
    logging/event_recorder.h
    download/downloader.h
)

add_executable(qtpilot-server
    ${MCP_SERVER_SOURCES}
    ${MCP_SERVER_HEADERS}
)

target_include_directories(qtpilot-server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(qtpilot-server
        PRIVATE
            Qt6::Core
            Qt6::Network
            Qt6::WebSockets
    )
else()
    target_link_libraries(qtpilot-server
        PRIVATE
            Qt5::Core
            Qt5::Network
            Qt5::WebSockets
    )
endif()

set_target_properties(qtpilot-server PROPERTIES
    OUTPUT_NAME "qtpilot-server"
)

install(TARGETS qtpilot-server
    RUNTIME DESTINATION bin
)
