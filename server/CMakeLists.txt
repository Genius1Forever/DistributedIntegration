add_executable(server
    main.cpp
    Server.cpp
    ../common/Logger.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)