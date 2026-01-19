cmake_minimum_required(VERSION 3.20)
project(so_projekt2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare( ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)

add_executable(so_projekt2 main.cpp
        Server.cpp
        Server.h
        Client.cpp
        Client.h
        tests/Timer.h
        tests/Timer.cpp
        tests/FileManager.cpp
        tests/FileManager.h)

target_link_libraries(so_projekt2
        PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
)
