project(novela-cpp)

enable_testing()

include_directories(${NOVELA_LIB_SOURCE_DIR}
  ${VTERM_LIB_SOURCE_DIR}
  ${TRANSMISSION_LIB_DIR}/src
)

file(GLOB_RECURSE NOVELA_FILES 
  ${NOVELA_LIB_SOURCE_DIR}/*.cpp
  ${VTERM_LIB_SOURCE_DIR}
  ${TRANSMISSION_LIB_DIR}/src
)
add_library(novela-cpp STATIC ${NOVELA_FILES})

target_include_directories(novela-cpp PUBLIC
  ${NOVELA_LIB_SOURCE_DIR}
  ${VTERM_LIB_SOURCE_DIR}
  ${TRANSMISSION_LIB_DIR}/src
)

