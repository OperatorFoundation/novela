project(vterm)

set(CMAKE_C_STANDARD 99)
set(VTERM_EXAMPLES_DIR ${VTERM_DIR}/examples)
set(VTERM_SKETCH_NAME vterm-test)
set(SKETCH_FILE ${VTERM_DIR}/${VTERM_SKETCH_NAME}/${VTERM_SKETCH_NAME}.ino)

set(BOARD "esp32:esp32:esp32s3")
set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arduino/esp32s3/vterm)
file(GLOB_RECURSE VTERM_FILES ${VTERM_LIB_SOURCE_DIR}/*.c)

add_library(vterm STATIC ${VTERM_FILES})
target_include_directories(vterm PUBLIC
  ${VTERM_LIB_SOURCE_DIR}
  src/
)

add_custom_target(build_vterm arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
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