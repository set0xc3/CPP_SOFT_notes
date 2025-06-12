cmake_minimum_required(VERSION 3.15...4.0)
project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

file(GLOB_RECURSE APP_FILES "src/*.cpp")
add_executable(${PROJECT_NAME} ${APP_FILES})

# Создать символическую ссылку res/ внутри Debug/Release
set(SRC_RES_DIR ${CMAKE_SOURCE_DIR}/res)
set(RES_SYMLINK ${OUTPUT_DIR}/res)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f ${RES_SYMLINK}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC_RES_DIR} ${RES_SYMLINK}
    COMMENT "Создание символической ссылки ${RES_SYMLINK} -> ${SRC_RES_DIR}"
)


add_subdirectory(tests)

# Добавляем корневую директорию проекта в include-пути
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

# Vendor

## IMGUI

set(IMGUI_DIR vendor/imgui)

file(GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/*.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

## ImGui FileBrowser

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/vendor/imgui-filebrowser
)

# Указываем, где искать заголовки для imgui
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/vendor/imgui)

set_target_properties(imgui PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/vendor/imgui/$<CONFIG>"
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

## SDL3

add_subdirectory(vendor/SDL3 EXCLUDE_FROM_ALL)

target_include_directories(imgui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/SDL3/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

## JSON

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/vendor/json/include
)

## Catch2

add_subdirectory(vendor/Catch2)
