cmake_minimum_required(VERSION 3.15...4.0)
project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(LIB_SOURCES
    "${PROJECT_SOURCE_DIR}/saura/include/saura/app/app.cpp"
    "${PROJECT_SOURCE_DIR}/saura/include/saura/core/os/os.cpp"
    "${PROJECT_SOURCE_DIR}/saura/include/saura/core/utils/utils.cpp"
    "${PROJECT_SOURCE_DIR}/saura/include/saura/ui/ui.cpp"
)
set(APP_SOURCES
    "${PROJECT_SOURCE_DIR}/saura/cmd/app/main.cpp"
)

# Создаем библиотеку
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${APP_SOURCES})

# Пути
set(SRC_RES_DIR "${CMAKE_SOURCE_DIR}/res")
set(RES_SYMLINK "${OUTPUT_DIR}/res")

# Создание ссылки — кросс-платформенно
if(WIN32)
    # Исправление для путей с пробелами и кириллицей
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cmd /c "if exist \"${RES_SYMLINK}\" rmdir \"${RES_SYMLINK}\""
        COMMAND cmd /c "mklink /J \"${RES_SYMLINK}\" \"${SRC_RES_DIR}\""
        COMMENT "Создание жёсткой ссылки (junction): ${RES_SYMLINK} -> ${SRC_RES_DIR}"
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${RES_SYMLINK}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${SRC_RES_DIR}" "${RES_SYMLINK}"
        COMMENT "Создание символической ссылки: ${RES_SYMLINK} -> ${SRC_RES_DIR}"
    )
endif()


# Добавляем include-пути
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/saura/include
    ${PROJECT_SOURCE_DIR}/vendor/imgui
    ${PROJECT_SOURCE_DIR}/vendor/json/include
    ${PROJECT_SOURCE_DIR}/vendor/SDL3/include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/saura/include
)

# Vendor libraries
## IMGUI
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/vendor/imgui)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${PROJECT_SOURCE_DIR}/vendor/SDL3/include
)

## SDL3
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/SDL3 EXCLUDE_FROM_ALL)

## JSON
add_library(json INTERFACE)
target_include_directories(json INTERFACE
    ${PROJECT_SOURCE_DIR}/vendor/json/include
)

## Catch2
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/Catch2)

# Связываем зависимости
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_lib
    imgui
    SDL3::SDL3
    json
)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    imgui
    SDL3::SDL3
    json
)
