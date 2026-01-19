cmake_minimum_required(VERSION 3.26)
project(so_projekt2)

set(CMAKE_CXX_STANDARD 23)

add_executable(so_projekt2 main.cpp
        Server.cpp
        Server.h
        Client.cpp
        Client.h
        tests/Timer.h
        tests/Timer.cpp
        tests/FileManager.cpp
        tests/FileManager.h)

include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v6.1.9  # Replace with a version, tag, or commit hash
)

FetchContent_MakeAvailable(ftxui)

target_link_libraries(so_projekt2
        PRIVATE ws2_32
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)