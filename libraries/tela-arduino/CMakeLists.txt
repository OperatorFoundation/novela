project(tela-arduino)

set(TELA_ARDUINO_EXAMPLES_DIR ${TELA_ARDUINO_DIR}/examples)
set(TELA_ARDUINO_1BIT_DIR ${TELA_ARDUINO_EXAMPLES_DIR}/tela-arduino-1bit)

include_directories(${TELA_LIB_SOURCE_DIR} ${GFX_DIR} ${DVHSTX_DIR})

# Define your sketch file
set(SKETCH_FILE ${TELA_ARDUINO_1BIT_DIR}/tela-arduino-1bit.ino)

set(BOARD "esp32:esp32:esp32s3")
set(PORT "/dev/cu.usbmodem1101")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/tela-arduino)
file(GLOB_RECURSE TELA_FILES ${TELA_LIB_SOURCE_DIR}/*.cpp ${TELA_ARDUINO_SOURCE_DIR}/*.cpp ${TELA_ARDUINO_SOURCE_DIR}/*.cpp)

add_custom_target(build_tela_arduino ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "compiler.cpp.extra_flags=-std=c++17 -D_BSD_SOURCE -D_DEFAULT_SOURCE"
            --build-property "compiler.cpp.extra_flags=${CMAKE_CXX__FLAGS_DEBUG}"
            --library ${GFX_DIR}
            --library ${VTERM_DIR}
            --library ${TRANSMISSION_LIB_DIR}
            --library ${TRANSMISSION_ARDUINO_DIR}
            --library ${TELA_LIB_DIR}
	    --library ${TELA_ARDUINO_DIR}
            --library ${TELA_ESP_DIR}
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${TELA_ARDUINO_SOURCE_DIR}
        COMMENT "Building tela-arduino with arduino-cli"
)

add_custom_target(upload_tela_arduino_1bit
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${TELA_ARDUINO_1BIT_DIR}
        COMMENT "Uploading tela-arduino-1bit with arduino-cli"
)
add_dependencies(upload_tela_arduino_1bit build_tela_arduino)
