set(SUPLA_DEVICE_LINUX_SRCS
  linux_network.cpp
  linux_timers.cpp
  network_client.cpp

  supla/source/cmd.cpp

  supla/parser/simple.cpp

  supla/sensor/thermometer_parsed.cpp

  ../../../src/supla/pv/fronius.cpp
  ../../../src/supla/pv/afore.cpp
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

