project(novela-arduino)

set(NOVELA_ARDUINO_EXAMPLES_DIR ${NOVELA_ARDUINO_DIR}/examples)
set(NOVELA_ARDUINO_1BIT_DIR ${NOVELA_ARDUINO_EXAMPLES_DIR}/novela-arduino-1bit)

include_directories(${NOVELA_LIB_SOURCE_DIR} ${GFX_DIR} ${DVHSTX_DIR})

# Define your sketch file
set(SKETCH_FILE ${NOVELA_ARDUINO_1BIT_DIR}/novela-arduino-1bit.ino)

set(BOARD "rp2040:rp2040:adafruit_feather_rp2350_hstx")
set(PORT "/dev/cu.usbmodem1101")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/novela-arduino)
file(GLOB_RECURSE NOVELA_FILES ${NOVELA_LIB_SOURCE_DIR}/*.cpp ${NOVELA_ARDUINO_SOURCE_DIR}/*.cpp ${NOVELA_ARDUINO_SOURCE_DIR}/*.ino)

add_custom_target(build_novela_arduino ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "compiler.cpp.extra_flags=-std=c++17 -D_BSD_SOURCE -D_DEFAULT_SOURCE"
            --build-property "compiler.cpp.extra_flags=${CMAKE_CXX__FLAGS_DEBUG}"
            --library ${GFX_DIR}
            --library ${VTERM_DIR}
            --library ${TRANSMISSION_LIB_DIR}
            --library ${TRANSMISSION_ARDUINO_DIR}
            --library ${TRANSMISSION_ESP32_DIR}
            --library ${NOVELA_LIB_DIR}
            --library ${NOVELA_ARDUINO_DIR}
            --library ${NOVELA_ESP_DIR}
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