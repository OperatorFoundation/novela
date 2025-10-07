cmake_minimum_required(VERSION 3.25)
project(tela-arduino-protocol-server)

set(CMAKE_CXX_STANDARD 17)
set(TELA_LIB_DIR ${CMAKE_SOURCE_DIR}/tela-cpp)
set(TELA_ARDUINO_DIR ${CMAKE_SOURCE_DIR}/tela-arduino)
set(TELA_ARDUINO_1BIT_DIR ${CMAKE_SOURCE_DIR}/tela-arduino-protocol-server)

# Define your sketch file
set(SKETCH_FILE ${TELA_ARDUINO_1BIT_DIR}/tela-arduino-protocol-server.ino)

set(BOARD "rp2040:rp2040:adafruit_feather_rp2350_hstx")
set(PORT "/dev/cu.usbmodem1101")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/tela-arduino-protocol-server)
file(GLOB_RECURSE TELA_FILES ${TELA_LIB_DIR}/*.cpp ${TELA_ARDUINO_DIR}/*.cpp ${TELA_ARDUINO_1BIT_DIR}/*.cpp ${TELA_ARDUINO_1BIT_DIR}/*.ino)

add_executable(tela-arduino-protocol-server ${TELA_FILES})
add_dependencies(tela-arduino-protocol-server build_tela_arduino_1bit)

add_custom_target(build_tela_arduino_1bit ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "build.extra_flags=-std=c++17\ -I${CMAKE_SOURCE_DIR}"
            --libraries ${CMAKE_SOURCE_DIR}
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${TELA_ARDUINO_1BIT_DIR}
        COMMENT "Building tela-arduino-protocol-server with arduino-cli"
)

add_custom_target(upload_tela_arduino_1bit ALL
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        --output-dir ${OUTPUT_DIR}
        --build-property "build.extra_flags=-std=c++17\ -I${CMAKE_SOURCE_DIR}"
        --libraries ${CMAKE_SOURCE_DIR}
        ${SKETCH_FILE}
        WORKING_DIRECTORY ${TELA_ARDUINO_1BIT_DIR}
        COMMENT "Uploading tela-arduino-protocol-server with arduino-cli"
)

# Generate ctags for vim
set(TAGS_FILE ${TELA_ARDUINO_1BIT_DIR}/tags)
add_custom_target(
        GenerateArduinoProtocolServerTags
        COMMAND ctags -R -f ${TAGS_FILE} ${TELA_LIB_DIR} ${TELA_ARDUINO_DIR} ${TELA_ARDUINO_1BIT_DIR}
        WORKING_DIRECTORY ${TELA_ARDUINO_1BIT_DIR}
        COMMENT "Generating ctags"
)
add_dependencies(build_tela_arduino_1bit GenerateArduinoProtocolServerTags)