cmake_minimum_required(VERSION 3.25)
project(tela-stdio)

set(CMAKE_CXX_STANDARD 17)

set(TELA_STDIO_DIR ${CMAKE_SOURCE_DIR}/tela-stdio)
set(TELA_LIB_DIR ${CMAKE_SOURCE_DIR}/libraries/tela-cpp)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_executable(tela-stdio
  TelaMain.cpp
  main.cpp
)
target_link_libraries(tela-stdio tela-cpp-lib)

# Generate ctags for vim
set(TAGS_FILE ${TELA_STDIO_DIR}/tags)
add_custom_target(
        GenerateStdtelags
        COMMAND ctags -R -f ${TAGS_FILE} ${TELA_STDIO_DIR} ${TELA_LIB_DIR}
        WORKING_DIRECTORY ${TELA_DIR}
        COMMENT "Generating ctags"
)
add_dependencies(${PROJECT_NAME} GenerateStdtelags)
