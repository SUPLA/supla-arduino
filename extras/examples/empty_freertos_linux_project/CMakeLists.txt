cmake_minimum_required(VERSION 3.13)

project(empty-supla-freertos-example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)

# Current board sources
set(BOARD_SRC
  main.cpp
  )

# All dependencies (supla-device + freeRTOS) will be compiled together
# because freeRTOS build configuration depends on local board
# FreeRTOSConfig.h file. supla-device depends on FreeRTOS.h and beacuse of
# that everything depends on local board files...
add_library(supladevice "")
target_compile_definitions(supladevice PUBLIC SUPLA_FREERTOS)
add_subdirectory($ENV{SUPLA_DEVICE_PATH} supla-device)

# FreeRTOS sources required for Linux compilation (from portable folder)
set(FREERTOS_SRC
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/event_groups.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/list.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/queue.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/tasks.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/timers.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/croutine.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/port.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/portable/MemMang/heap_3.c
  )

target_sources(supladevice PUBLIC ${FREERTOS_SRC} )
target_include_directories (supladevice
  PUBLIC
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/include
  .
  )


include_directories(
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/include
  $ENV{SUPLA_FREERTOS_PATH}/FreeRTOS/Source/portable/ThirdParty/GCC/Posix
  $ENV{SUPLA_DEVICE_PATH}/src
  .
  )

add_executable(empty-supla-freertos-example
  ${BOARD_SRC}
  )

set_target_properties(empty-supla-freertos-example PROPERTIES LINK_LIBRARIES -pthread)
target_link_libraries (empty-supla-freertos-example LINK_PUBLIC
  supladevice
  )

