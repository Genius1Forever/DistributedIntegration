add_executable(client
    main.cpp
    Client.cpp
    ../common/Logger.cpp
)

target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)