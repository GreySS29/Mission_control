# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Rpi_block LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Rpi_block
    src/main.cpp
)