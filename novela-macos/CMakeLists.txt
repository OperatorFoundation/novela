project(novela-macos)

include_directories(${NOVELA_LIB_SOURCE_DIR} ${NOVELA_SDL_SOURCE_DIR} ${GFX_DIR} ${TRANSMISSION_MACOS_DIR}/src ${NOVELA_SDL_SOURCE_DIR})

add_executable(novela-macos
  src/main.cpp
  src/system_clock.cpp
  src/system_clock.h
  src/console_logger.cpp
  src/console_logger.h
  ../libraries/novela-sdl/src/sdl_cursor.cpp
  ../libraries/novela-sdl/src/sdl_cursor.h
)

target_link_libraries(novela-macos
  PRIVATE
  novela-sdl
  novela-cpp
  vterm
  transmission-macos
  caneta-sdl
  sdl2
)

target_compile_features(novela-macos PUBLIC cxx_std_17)

# macOS-specific settings if needed
if(APPLE)
  set_target_properties(novela-macos PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.operatorfoundation.novela"
  )
endif()