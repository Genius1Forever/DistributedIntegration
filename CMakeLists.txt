cmake_minimum_required(VERSION 3.15)

project(DistributedIntegration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предупреждения
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Подпроекты
add_subdirectory(server)
add_subdirectory(client)
