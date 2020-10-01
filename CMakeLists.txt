cmake_minimum_required(VERSION 2.8)

project(joystick)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

find_package (Threads)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES src/joystick.c
        src/main.cpp
        src/commander.cpp
        src/can_commander.cpp
        src/can_monitor.cpp
        )

add_executable(joystick ${SOURCES})

target_include_directories(
        joystick
        PRIVATE
        include
        /usr/local/include/
        ${SDL2_INCLUDE_DIRS})

target_link_libraries(
        joystick
        PRIVATE
        canlib
        ${SDL2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        m)

