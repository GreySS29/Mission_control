# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Rpi_block LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Rpi_block
    src/main.cpp
)

target_link_libraries(rpi_data_hub PRIVATE
    nlohmann_json::nlohmann_json
)
