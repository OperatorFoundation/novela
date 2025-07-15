project(novela-esp)

include_directories(${NOVELA_LIB_SOURCE_DIR} ${GFX_DIR})

# Define your sketch file
set(NOVELA_ESP_SKETCH_NAME novela-esp)
set(SKETCH_FILE ${NOVELA_ESP_DIR}/examples/${NOVELA_ESP_SKETCH_NAME}/${NOVELA_ESP_SKETCH_NAME}.ino)

set(BOARD "esp32:esp32:esp32s3")
set(PORT "/dev/cu.wchusbserial110")

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esp32s3/${NOVELA_ESP_SKETCH_NAME})

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

add_custom_target(upload_novela_esp ALL
        COMMAND arduino-cli upload
        -p ${PORT}
        --fqbn ${BOARD}
        ${SKETCH_FILE}
        COMMENT "Uploading novela-esp with arduino-cli"
)
add_dependencies(upload_novela_esp build_novela_esp)