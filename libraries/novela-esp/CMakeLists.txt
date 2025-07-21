project(novela-esp)

include_directories(${NOVELA_LIB_SOURCE_DIR} ${GFX_DIR})

# Define your sketch file
set(NOVELA_ESP_SKETCH_NAME novela-esp)
set(SKETCH_FILE ${NOVELA_ESP_DIR}/examples/${NOVELA_ESP_SKETCH_NAME}/${NOVELA_ESP_SKETCH_NAME}.ino)

set(BOARD "esp32:esp32:esp32s3")
set(PORT "/dev/cu.wchusbserial110")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esp32s3/${NOVELA_ESP_SKETCH_NAME})

set(REMOTE_HOST "brandon@bonnie.local")
set(REMOTE_TEMP_DIR "/tmp/esp32_upload")

add_custom_target(build_novela_esp ALL
        COMMAND arduino-cli compile
            --fqbn ${BOARD}
            --output-dir ${OUTPUT_DIR}
            --build-property "compiler.cpp.extra_flags=-std=c++17 -D_BSD_SOURCE -D_DEFAULT_SOURCE"
            --build-property "compiler.cpp.extra_flags=${CMAKE_CXX__FLAGS_DEBUG}"
            --board-options "PSRAM=opi"
            --library ${GFX_DIR}
            --library ${VTERM_DIR}
            --library ${TRANSMISSION_LIB_DIR}
            --library ${TRANSMISSION_ARDUINO_DIR}
            --library ${TRANSMISSION_ESP32_DIR}
            --library ${NOVELA_LIB_DIR}
            --library ${NOVELA_ARDUINO_DIR}
            --library ${NOVELA_ESP_DIR}
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${NOVELA_ESP_SOURCE_DIR}
        COMMENT "Building novela-esp with arduino-cli"
)

add_custom_target(upload_novela_esp
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${SKETCH_FILE}
        COMMENT "Uploading novela-esp with arduino-cli"
)
add_dependencies(upload_novela_esp build_novela_esp)

# Create remote upload target
add_custom_target(remote_upload_novela_esp
  # Create temp directory on Pi
  COMMAND ssh ${REMOTE_HOST} "mkdir -p ${REMOTE_TEMP_DIR}"

  # Copy binary to Pi
  COMMAND scp ${OUTPUT_DIR} ${REMOTE_HOST}:${REMOTE_TEMP_DIR}/${NOVELA_ESP_SKETCH_NAME}.bin

  # Upload using arduino-cli on Pi
  COMMAND ssh ${REMOTE_HOST} "arduino-cli upload --fqbn ${BOARD} --port ${PORT} --input-file ${REMOTE_TEMP_DIR}/${NOVELA_ESP_SKETCH_NAME}.bin"

  # Clean up temp file
  COMMAND ssh ${REMOTE_HOST} "rm -f ${REMOTE_TEMP_DIR}/${NOVELA_ESP_SKETCH_NAME}.bin"

  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Uploading ${NOVELA_ESP_SKETCH_NAME} to ESP32 via ${REMOTE_HOST}"
  VERBATIM
)
add_dependencies(remote_upload_novela_esp build_novela_esp)