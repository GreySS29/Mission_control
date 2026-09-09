# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Rpi_block LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Rpi_block
    src/main.cpp
)

target_link_libraries(Rpi_block PRIVATE
    Boost::system
    Boost::thread
    nlohmann_json::nlohmann_json
)
