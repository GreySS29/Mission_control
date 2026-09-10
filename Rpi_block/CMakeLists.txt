# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Rpi_block LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS system thread)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Rpi_block
    src/main.cpp
)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/../external_lib/json"
     "${CMAKE_BINARY_DIR}/json-build"
   
)

target_include_directories(Rpi_block PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(Rpi_block PUBLIC
    ${CMAKE_SOURCE_DIR}/../external_lib/json/include
)

target_link_libraries(Rpi_block PRIVATE
    Boost::system
    Boost::thread
    nlohmann_json::nlohmann_json
)
