cmake_minimum_required(VERSION 3.15...4.0)
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    vault_manager.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/json/include
)

# Vendor

## Catch2

target_link_libraries(${PROJECT_NAME}
    PRIVATE Catch2::Catch2WithMain
)
