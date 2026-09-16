cmake_minimum_required(VERSION 3.16)
project(CSOPESY_Emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(csopesy
    src/main.cpp
    src/Console.cpp
    src/Marquee.cpp
)

target_include_directories(csopesy PRIVATE include)

# Link thread library.
find_package(Threads REQUIRED)
target_link_libraries(csopesy PRIVATE Threads::Threads)

