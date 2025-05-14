cmake_minimum_required(VERSION 3.31)
project(novela-cpp-lib)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

# Compile novela-cpp library for local build machine architecture
set(NOVELA_LIB_DIR ${CMAKE_SOURCE_DIR}/libraries/novela-cpp)
set(NOVELA_LIB_SOURCE_DIR ${NOVELA_LIB_DIR}/src)
include_directories(${NOVELA_LIB_SOURCE_DIR})
file(GLOB_RECURSE NOVELA_FILES ${NOVELA_LIB_SOURCE_DIR}/*.cpp)
add_library(novela-cpp-lib STATIC ${NOVELA_FILES}
  src/canvas.cpp
  src/canvas.h
  src/novela.cpp
  src/novela.h)

target_include_directories(novela-cpp-lib PUBLIC ${NOVELA_LIB_SOURCE_DIR})

# Generate ctags for vim
set(TAGS_FILE ${NOVELA_LIB_SOURCE_DIR}/tags)
add_custom_target(
  GenerateLibTags
  COMMAND ctags -R -f ${TAGS_FILE} ${NOVELA_LIB_SOURCE_DIR}
  WORKING_DIRECTORY ${NOVELA_LIB_SOURCE_DIR}
  COMMENT "Generating ctags"
)
add_dependencies(${PROJECT_NAME} GenerateLibTags)
