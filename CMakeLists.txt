set(SUPLA_DEVICE_SRCS
  src/supla/uptime.cpp
  src/supla/channel.cpp
  src/supla/channel_extended.cpp
  src/supla/io.cpp
  src/supla/tools.cpp
  src/supla/element.cpp
  src/supla/local_action.cpp
  src/supla/channel_element.cpp
  src/supla/correction.cpp
  src/supla/at_channel.cpp
  src/supla/action_handler.cpp
  src/supla/time.cpp
  src/supla/timer.cpp

  src/supla/condition.cpp
  src/supla/condition_getter.cpp
)

set(SUPLA_COMMON_SRCS
  src/supla-common/lck.c
  src/supla-common/log.c
  src/supla-common/proto.c
  src/supla-common/srpc.c
  )


target_sources(supladevice
  PUBLIC
  ${SUPLA_DEVICE_SRCS}
  ${SUPLA_COMMON_SRCS}
  )

target_include_directories (supladevice
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/supla-common
  )

target_compile_definitions(supladevice PUBLIC SUPLA_FREERTOS SUPLA_DEVICE)
