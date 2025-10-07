cmake_minimum_required(VERSION 3.25)
project(tela-cpp-lib-tests)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

set(CTEST_PROJECT_NAME tela-cpp)
set(CTEST_NIGHTLY_START_TIME 01:00:00 UTC)
set(CTEST_SUBMIT_URL https://my.cdash.org/submit.php?project=tela-cpp)
set(CTEST_DROP_SITE_CDASH TRUE)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.0
)

FetchContent_MakeAvailable(Catch2)

set(TELA_CPP_LIB_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
set(TELA_LIB_DIR ${CMAKE_SOURCE_DIR}/libraries/tela-cpp)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_executable(tests main.cpp)
target_link_libraries(tests PRIVATE tela-cpp-lib Catch2::Catch2WithMain)

# Generate ctags for vim
set(TAGS_FILE ${TELA_CPP_LIB_TEST_DIR}/tags)
add_custom_target(
  GenerateTestTags
  COMMAND ctags -R -f ${TAGS_FILE} ${TELA_CPP_LIB_TEST_DIR} ${TELA_LIB_DIR}
  WORKING_DIRECTORY ${TELA_CPP_LIB_TEST_DIR}
  COMMENT "Generating ctags"
)
add_dependencies(tests GenerateTestTags)

include(CTest)
add_test(NAME "small words" COMMAND tests)
