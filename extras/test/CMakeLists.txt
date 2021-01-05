cmake_minimum_required(VERSION 3.11)

project(supladevice)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(../../src)

add_subdirectory(../../src/ build)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
  )

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

file(GLOB SRCS UptimeTests/*.cpp)

add_executable(supladevicetests ${SRCS})

target_link_libraries(supladevicetests
  gtest
  gtest_main
  supladevicelib
  )

add_test(NAME supladevicetests
  COMMAND supladevicetests)
