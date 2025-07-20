cmake_minimum_required(VERSION 3.25)
project(novela-arduino-protocol-server)

set(CMAKE_CXX_STANDARD 17)
set(NOVELA_LIB_DIR ${CMAKE_SOURCE_DIR}/novela-cpp)
set(NOVELA_ARDUINO_DIR ${CMAKE_SOURCE_DIR}/novela-arduino)
set(NOVELA_ARDUINO_1BIT_DIR ${CMAKE_SOURCE_DIR}/novela-arduino-protocol-server)
include_directories(${NOVELA_LIB_DIR} ${NOVELA_ARDUINO_DIR})

# Define your sketch file
set(SKETCH_FILE ${NOVELA_ARDUINO_1BIT_DIR}/novela-arduino-protocol-server.ino)

set(BOARD "rp2040:rp2040:adafruit_feather_rp2350_hstx")
set(PORT "/dev/cu.usbmodem1101")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/novela-arduino-protocol-server)
file(GLOB_RECURSE NOVELA_FILES ${NOVELA_LIB_DIR}/*.cpp ${NOVELA_ARDUINO_DIR}/*.cpp ${NOVELA_ARDUINO_1BIT_DIR}/*.cpp ${NOVELA_ARDUINO_1BIT_DIR}/*.ino)

add_executable(novela-arduino-protocol-server ${NOVELA_FILES})
add_dependencies(novela-arduino-protocol-server build_novela_arduino_1bit)

add_custom_target(build_novela_arduino_1bit ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "build.extra_flags=-std=c++17\ -I${CMAKE_SOURCE_DIR}"
            --libraries ${CMAKE_SOURCE_DIR}
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${NOVELA_ARDUINO_1BIT_DIR}
        COMMENT "Building novela-arduino-protocol-server with arduino-cli"
)

add_custom_target(upload_novela_arduino_1bit ALL
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        --output-dir ${OUTPUT_DIR}
        --build-property "build.extra_flags=-std=c++17\ -I${CMAKE_SOURCE_DIR}"
        --libraries ${CMAKE_SOURCE_DIR}
        ${SKETCH_FILE}
        WORKING_DIRECTORY ${NOVELA_ARDUINO_1BIT_DIR}
        COMMENT "Uploading novela-arduino-protocol-server with arduino-cli"
)

# Generate ctags for vim
set(TAGS_FILE ${NOVELA_ARDUINO_1BIT_DIR}/tags)
add_custom_target(
        GenerateArduinoProtocolServerTags
        COMMAND ctags -R -f ${TAGS_FILE} ${NOVELA_LIB_DIR} ${NOVELA_ARDUINO_DIR} ${NOVELA_ARDUINO_1BIT_DIR}
        WORKING_DIRECTORY ${NOVELA_ARDUINO_1BIT_DIR}
        COMMENT "Generating ctags"
)
add_dependencies(build_novela_arduino_1bit GenerateArduinoProtocolServerTags)