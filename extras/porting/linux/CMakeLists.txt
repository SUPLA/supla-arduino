FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

set(SUPLA_DEVICE_LINUX_SRCS
  linux_network.cpp
  linux_timers.cpp
  network_client.cpp

  supla/source/cmd.cpp
  supla/source/file.cpp

  supla/parser/parser.cpp
  supla/parser/simple.cpp
  supla/parser/json.cpp

  supla/sensor/sensor_parsed.cpp
  supla/sensor/thermometer_parsed.cpp
  supla/sensor/impulse_counter_parsed.cpp
  supla/sensor/electricity_meter_parsed.cpp

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

target_link_libraries (supladevice PUBLIC
  nlohmann_json::nlohmann_json
  )
