cmake_minimum_required(VERSION 3.15)
project(RemMux)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(RemMux
        src/HighResolutionTimer.cpp
        src/RemMux.cpp
        src/UIHeader.cpp
        src/UIText.cpp
        src/main.cpp)

target_link_libraries(RemMux ${CURSES_LIBRARIES})
