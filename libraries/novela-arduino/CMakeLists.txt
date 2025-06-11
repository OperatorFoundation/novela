cmake_minimum_required(VERSION 3.31)
project(novela-arduino)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(NOVELA_LIB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libraries/novela-cpp/src)
set(NOVELA_ARDUINO_DIR ${CMAKE_SOURCE_DIR}/libraries/novela-arduino)
set(NOVELA_ARDUINO_SOURCE_DIR ${NOVELA_ARDUINO_DIR}/src)
set(NOVELA_ARDUINO_EXAMPLES_DIR ${NOVELA_ARDUINO_DIR}/examples)
set(NOVELA_ARDUINO_1BIT_DIR ${NOVELA_ARDUINO_EXAMPLES_DIR}/novela-arduino-1bit)
set(ARDUINO_DIR ~/Documents/Arduino/libraries)
set(GFX_DIR ${ARDUINO_DIR}/Adafruit_GFX_Library)
set(DVHSTX_DIR ${ARDUINO_DIR}/Adafruit_DVI_HSTX/src)
include_directories(${NOVELA_LIB_SOURCE_DIR} ${GFX_DIR} ${DVHSTX_DIR})

# Define your sketch file
set(SKETCH_FILE ${NOVELA_ARDUINO_1BIT_DIR}/novela-arduino-1bit.ino)

set(BOARD "rp2040:rp2040:adafruit_feather_rp2350_hstx")
set(PORT "/dev/cu.usbmodem1101")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/novela-arduino)
file(GLOB_RECURSE NOVELA_FILES ${NOVELA_LIB_SOURCE_DIR}/*.cpp ${NOVELA_ARDUINO_SOURCE_DIR}/*.cpp ${NOVELA_ARDUINO_SOURCE_DIR}/*.ino)

add_executable(novela-arduino ${NOVELA_FILES}
  src/soft_cursor.cpp
  src/soft_cursor.h
  src/ReliableConnectionUsbCdc.cpp
  src/ReliableConnectionUsbCdc.h
  src/serial_dma.cpp
  src/serial_dma.h)
add_dependencies(novela-arduino build_novela_arduino)
target_link_libraries(novela-arduino PRIVATE novela-cpp)

add_custom_target(build_novela_arduino ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "compiler.cpp.extra_flags=-std=c++17 -D_BSD_SOURCE -D_DEFAULT_SOURCE"
            --build-property "compiler.cpp.extra_flags=${CMAKE_CXX__FLAGS_DEBUG}"
            --libraries ${CMAKE_SOURCE_DIR}/libraries
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${NOVELA_ARDUINO_SOURCE_DIR}
        COMMENT "Building novela-arduino with arduino-cli"
)

add_custom_target(upload_novela_arduino_1bit ALL
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${NOVELA_ARDUINO_1BIT_DIR}
        COMMENT "Uploading novela-arduino-1bit with arduino-cli"
)
add_dependencies(upload_novela_arduino_1bit build_novela_arduino)

# Generate ctags for vim
set(TAGS_FILE ${NOVELA_ARDUINO_SOURCE_DIR}/tags)
add_custom_target(
        GenerateArduinoTags
        COMMAND ctags -R -f ${TAGS_FILE} ${NOVELA_LIB_SOURCE_DIR} ${NOVELA_ARDUINO_SOURCE_DIR}
        WORKING_DIRECTORY ${NOVELA_ARDUINO_SOURCE_DIR}
        COMMENT "Generating ctags"
)
add_dependencies(build_novela_arduino GenerateArduinoTags)