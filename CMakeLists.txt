cmake_minimum_required(VERSION 3.15...4.0)
project(program)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Установка выходных директорий для исполняемых файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")

add_executable(${PROJECT_NAME} src/main.cpp)

# Добавляем корневую директорию проекта в include-пути
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

# Vendor

## IMGUI

set(IMGUI_DIR vendor/imgui)

file(GLOB IMGUI_SOURCES 
  ${IMGUI_DIR}/*.cpp 
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp 
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

# Указываем, где искать заголовки для imgui
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/vendor/imgui)

# Указываем, где искать заголовки SDL3 при сборке imgui
target_include_directories(imgui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/SDL3/include)

set_target_properties(imgui PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

## SDL3

add_subdirectory(vendor/SDL3 EXCLUDE_FROM_ALL)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)