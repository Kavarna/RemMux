cmake_minimum_required(VERSION 3.15)
project(RemMuxServer)

set(CMAKE_CXX_STANDARD 17)


include_directories(include)


add_executable(RemMuxServer
        src/ServerLogger.cpp
        src/main.cpp)

install(TARGETS RemMuxServer DESTINATION "Executable")

