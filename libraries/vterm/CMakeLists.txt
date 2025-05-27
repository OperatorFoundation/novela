cmake_minimum_required(VERSION 3.31)
project(vterm)

set(CMAKE_C_STANDARD 99)
set(ARDUINO_DIR ~/Documents/Arduino/libraries)
set(VTERM_DIR ${CMAKE_SOURCE_DIR}/libraries/vterm)
set(VTERM_SOURCE_DIR ${VTERM_DIR}/src)
set(VTERM_EXAMPLES_DIR ${VTERM_DIR}/examples)
set(VTERM_EXAMPLES_TEST_DIR ${VTERM_EXAMPLES_DIR}/vterm-test)
set(SKETCH_FILE ${VTERM_EXAMPLES_TEST_DIR}/vterm-test.ino)

set(BOARD "rp2040:rp2040:adafruit_feather_rp2350_hstx")
set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/rp2350/vterm)
file(GLOB_RECURSE VTERM_FILES ${VTERM_SOURCE_DIR}/*.c)

add_executable(vterm ${VTERM_FILES})

add_custom_target(build_vterm arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --libraries ${CMAKE_SOURCE_DIR}/libraries
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${VTERM_SOURCE_DIR}
        COMMENT "Building vterm with arduino-cli"
)

add_custom_target(upload_vterm ALL
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${SKETCH_FILE}
        COMMENT "Uploading vterm with arduino-cli"
)
add_dependencies(upload_vterm build_vterm)

# Generate ctags for vim
set(TAGS_FILE ${VTERM_SOURCE_DIR}/tags)
add_custom_target(
        GenerateVtermTags
        COMMAND ctags -R -f ${TAGS_FILE} ${VTERM_SOURCE_DIR} ${VTERM_EXAMPLES_TEST_DIR}
        WORKING_DIRECTORY ${VTERM_SOURCE_DIR}
        COMMENT "Generating ctags"
)
add_dependencies(build_vterm GenerateVtermTags)