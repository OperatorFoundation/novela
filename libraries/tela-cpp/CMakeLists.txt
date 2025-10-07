project(tela-cpp)

enable_testing()

include_directories(${TELA_LIB_SOURCE_DIR}
  ${VTERM_LIB_SOURCE_DIR}
  ${TRANSMISSION_LIB_DIR}/src
)

file(GLOB_RECURSE TELA_FILES 
  ${TELA_LIB_SOURCE_DIR}/*.cpp
)
add_library(tela-cpp STATIC ${TELA_FILES})

target_include_directories(tela-cpp PUBLIC
  ${TELA_LIB_SOURCE_DIR}
  ${VTERM_LIB_SOURCE_DIR}
  ${TRANSMISSION_LIB_DIR}/src
)

