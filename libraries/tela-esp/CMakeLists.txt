project(tela-esp)

include_directories(${TELA_LIB_SOURCE_DIR} ${GFX_DIR})

# Define your sketch file
set(TELA_ESP_SKETCH_NAME tela-esp)
set(SKETCH_FILE ${TELA_ESP_DIR}/examples/${TELA_ESP_SKETCH_NAME}/${TELA_ESP_SKETCH_NAME}.ino)

set(BOARD "esp32:esp32:esp32s3")
set(PORT "/dev/cu.wchusbserial110")
set(REMOTE_PORT "/dev/ttyUSB0")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esp32s3/${TELA_ESP_SKETCH_NAME})
set(OUTPUT_FILE ${OUTPUT_DIR}/${TELA_ESP_SKETCH_NAME}.ino.bin)

set(REMOTE_HOST "brandon@bonnie.local")
set(REMOTE_TEMP_DIR "/tmp/esp32_upload")

add_custom_target(build_tela_esp ALL
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
            --library ${TELA_LIB_DIR}
            --library ${TELA_ARDUINO_DIR}
            --library ${TELA_ESP_DIR}
            ${SKETCH_FILE}
        WORKING_DIRECTORY ${TELA_ESP_SOURCE_DIR}
        COMMENT "Building tela-esp with arduino-cli"
)

add_custom_target(upload_tela_esp
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${SKETCH_FILE}
        COMMENT "Uploading tela-esp with arduino-cli"
)
add_dependencies(upload_tela_esp build_tela_esp)

# Create remote upload target
add_custom_target(remote_upload_tela_esp
  # Create temp directory on Pi
  COMMAND ssh ${REMOTE_HOST} "mkdir -p ${REMOTE_TEMP_DIR}"

  # Copy binary to Pi
  COMMAND scp ${OUTPUT_DIR}/${TELA_ESP_SKETCH_NAME}.ino.bootloader.bin ${REMOTE_HOST}:${REMOTE_TEMP_DIR}/${TELA_ESP_SKETCH_NAME}.bootloader.bin
  COMMAND scp ${OUTPUT_DIR}/${TELA_ESP_SKETCH_NAME}.ino.partitions.bin ${REMOTE_HOST}:${REMOTE_TEMP_DIR}/${TELA_ESP_SKETCH_NAME}.partitions.bin
  COMMAND scp ${OUTPUT_DIR}/${TELA_ESP_SKETCH_NAME}.ino.bin ${REMOTE_HOST}:${REMOTE_TEMP_DIR}/${TELA_ESP_SKETCH_NAME}.bin

  # Upload using arduino-cli on Pi
  COMMAND ssh ${REMOTE_HOST} "arduino-cli upload --fqbn ${BOARD} --port ${REMOTE_PORT} --input-file ${REMOTE_TEMP_DIR}/${TELA_ESP_SKETCH_NAME}.bin"

  # Clean up temp file
  COMMAND ssh ${REMOTE_HOST} "rm -f ${REMOTE_TEMP_DIR}/${TELA_ESP_SKETCH_NAME}.bin"

  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Uploading ${TELA_ESP_SKETCH_NAME} to ESP32 via ${REMOTE_HOST}"
  VERBATIM
)
add_dependencies(remote_upload_tela_esp build_tela_esp)