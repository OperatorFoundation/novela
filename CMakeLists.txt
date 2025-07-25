cmake_minimum_required(VERSION 3.25)
project(novela)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

set(LIB ${CMAKE_SOURCE_DIR}/libraries)
set(GFX_DIR ${LIB}/GFX)
set(VTERM_DIR ${LIB}/vterm)
set(VTERM_LIB_SOURCE_DIR ${VTERM_DIR}/src)
set(TRANSMISSION_LIB_DIR ${LIB}/transmission-cpp)
set(TRANSMISSION_ARDUINO_DIR ${LIB}/transmission-arduino)
set(TRANSMISSION_ESP32_DIR ${LIB}/transmission-esp32)

set(NOVELA_LIB_DIR ${LIB}/novela-cpp)
set(NOVELA_LIB_SOURCE_DIR ${NOVELA_LIB_DIR}/src)

set(NOVELA_ARDUINO_DIR ${LIB}/novela-arduino)
set(NOVELA_ARDUINO_SOURCE_DIR ${NOVELA_ARDUINO_DIR}/src)

set(NOVELA_ESP_DIR ${CMAKE_SOURCE_DIR}/libraries/novela-esp)
set(NOVELA_ESP_SOURCE_DIR ${NOVELA_ESP_DIR}/src)

add_subdirectory("libraries/novela-cpp")
add_subdirectory("libraries/novela-arduino")
add_subdirectory("libraries/novela-esp")
add_subdirectory("libraries/vterm")
add_subdirectory("novela-stdio")
add_subdirectory("tests/")
