project(novela-cpp-lib)

enable_testing()

include_directories(${NOVELA_LIB_SOURCE_DIR}, ${VTERM_LIB_SOURCE_DIR})
file(GLOB_RECURSE NOVELA_FILES ${NOVELA_LIB_SOURCE_DIR}/*.cpp)
add_library(novela-cpp-lib STATIC ${NOVELA_FILES})

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
