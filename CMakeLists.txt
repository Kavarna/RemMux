cmake_minimum_required(VERSION 3.15)
project(RemMux)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_definitions(-DDEBUG)
    set(COMPILE_CONFIGURATION "debug")
elseif (CMAKE_BUILD_TYPE MATCHES RELEASE)
	set(COMPILE_CONFIGURATION "release")
else()
    add_definitions(-DDEBUG)
    set(COMPILE_CONFIGURATION "debug")
endif()


include_directories(include)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_subdirectory(client)
add_subdirectory(server)

