project(tela-sdl)

include_directories(${TELA_LIB_SOURCE_DIR} ${GFX_DIR})

set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sdl)

add_definitions(-DLGFX_SDL)

# Library source files - only your terminal emulator SDL backend code
file(GLOB TELA_SDL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  src/*.cpp
)

# LovyanGFX SDL platform files
file(GLOB LOVYANGFX_SDL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  ${GFX_DIR}/src/lgfx/Fonts/efont/*.c
  ${GFX_DIR}/src/lgfx/Fonts/IPA/*.c
  ${GFX_DIR}/src/lgfx/utility/*.c
  ${GFX_DIR}/src/lgfx/v1/*.cpp
  ${GFX_DIR}/src/lgfx/v1/misc/*.cpp
  ${GFX_DIR}/src/lgfx/v1/panel/Panel_Device.cpp
  ${GFX_DIR}/src/lgfx/v1/panel/Panel_FrameBufferBase.cpp
  ${GFX_DIR}/src/lgfx/v1/platforms/sdl/*.cpp
)

# Create the library
add_library(tela-sdl ${TELA_SDL_SOURCES} ${LOVYANGFX_SDL_SOURCES})

# Link dependencies
target_link_libraries(tela-sdl
  PUBLIC
  tela-cpp
)

# Include directories
target_include_directories(tela-sdl
  PUBLIC
  include
  ${GFX_DIR}/src/
)

# Platform-specific SDL2 linking
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  find_package(SDL2 REQUIRED SDL2)
  target_include_directories(tela-sdl PUBLIC ${SDL2_INCLUDE_DIRS})
  target_link_libraries(tela-sdl PUBLIC -lpthread ${SDL2_LIBRARIES})
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  find_package(SDL2 REQUIRED COMPONENTS SDL2)
  target_link_libraries(tela-sdl PUBLIC SDL2::SDL2)
endif()

target_compile_features(tela-sdl PUBLIC cxx_std_17)