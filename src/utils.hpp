#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

fs::path get_appdata_path() {
    fs::path res;
    const char* appdata = nullptr;

    // Для MSVC и Clang-cl (режим MSVC)
    #if defined(_MSC_VER) || (defined(__clang__) && defined(_MSC_VER))
        char* buffer = nullptr;
        size_t len = 0;
        errno_t err = _dupenv_s(&buffer, &len, "APPDATA");
        if (err || buffer == nullptr) {
            if (buffer) free(buffer);
            throw std::runtime_error("APPDATA environment variable not found");
        }
        appdata = buffer;
    #else
        // Для MinGW и других компиляторов
        appdata = std::getenv("APPDATA");
        if (!appdata) {
            throw std::runtime_error("APPDATA environment variable not found");
        }
    #endif

    res = fs::path(appdata);

    // Освобождаем буфер для MSVC/Clang-cl
    #if defined(_MSC_VER) || (defined(__clang__) && defined(_MSC_VER))
        free(buffer);
    #endif

    return res;
}

#endif
