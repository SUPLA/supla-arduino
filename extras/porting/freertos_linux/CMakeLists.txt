set(SUPLA_DEVICE_FREERTOS_SRCS
  freertos_platform.cpp
)

target_sources(supladevice
  PUBLIC
  ${SUPLA_DEVICE_FREERTOS_SRCS}
  )

target_include_directories (supladevice
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_compile_definitions(supladevice PUBLIC SUPLA_FREERTOS)

