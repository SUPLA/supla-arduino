FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_Declare(cxxopts URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.0.0.tar.gz)
FetchContent_MakeAvailable(json cxxopts)

set(SUPLA_DEVICE_LINUX_SRCS
  linux_network.cpp
  linux_platform.cpp

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
  supla/sensor/binary_parsed.cpp

  ../../../src/supla/pv/fronius.cpp
  ../../../src/supla/pv/afore.cpp

  ../../../src/supla-common/tools.c
  ../../../src/supla-common/eh.c

  linux_log.c
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
  cxxopts::cxxopts
  )
