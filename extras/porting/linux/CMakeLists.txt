set(SUPLA_DEVICE_LINUX_SRCS
  linux_network.cpp
)

target_sources(supladevice
  PUBLIC
  ${SUPLA_DEVICE_LINUX_SRCS}
  )

target_include_directories (supladevice
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_compile_definitions(supladevice PUBLIC SUPLA_LINUX)

