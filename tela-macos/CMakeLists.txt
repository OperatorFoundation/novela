project(tela-macos)

include_directories(${TELA_LIB_SOURCE_DIR} ${TELA_SDL_SOURCE_DIR} ${GFX_DIR} ${TRANSMISSION_MACOS_DIR}/src ${TELA_SDL_SOURCE_DIR})

add_executable(tela-macos
  src/main.cpp
  src/system_clock.cpp
  src/system_clock.h
  src/console_logger.cpp
  src/console_logger.h
  ../libraries/tela-sdl/src/sdl_cursor.cpp
  ../libraries/tela-sdl/src/sdl_cursor.h
)

target_link_libraries(tela-macos
  PRIVATE
  tela-sdl
  tela-cpp
  vterm
  transmission-macos
  caneta-sdl
  sdl2
)

target_compile_features(tela-macos PUBLIC cxx_std_17)

# macOS-specific settings if needed
if(APPLE)
  set_target_properties(tela-macos PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.operatorfoundation.tela"
  )
endif()