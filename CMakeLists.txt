cmake_minimum_required(VERSION 3.26)
project(so_projekt2)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_FLAGS "-static")
add_executable(so_projekt2 main.cpp
        #temp_z_pcurses.cpp
        Server.cpp
        Server.h
        Client.cpp
        Client.h)

target_link_libraries(so_projekt2 PRIVATE ws2_32)
#target_link_libraries(so_projekt2 "C:/Program Files/JetBrains/CLion 2023.2.2/bin/mingw/lib/pdcurses.a")