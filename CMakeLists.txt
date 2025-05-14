cmake_minimum_required(VERSION 3.31)
project(novela)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory("libraries/novela-cpp")
add_subdirectory("libraries/novela-arduino")
add_subdirectory("novela-stdio")
add_subdirectory("tests/")
