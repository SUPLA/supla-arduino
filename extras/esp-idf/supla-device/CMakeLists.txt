set(SUPLA_COMMONM_SRCS
  ../../../src/supla-common/lck.c
  ../../../src/supla-common/log.c
  ../../../src/supla-common/proto.c
  ../../../src/supla-common/srpc.c
)

set(SUPLA_DEVICE_SRCS
  ../../../src/SuplaDevice.cpp

  ../../../src/supla/action_handler.cpp
  ../../../src/supla/at_channel.cpp
  ../../../src/supla/channel.cpp
  ../../../src/supla/channel_element.cpp
  ../../../src/supla/channel_extended.cpp
  ../../../src/supla/correction.cpp
  ../../../src/supla/element.cpp
  ../../../src/supla/io.cpp
  ../../../src/supla/local_action.cpp
  ../../../src/supla/log_wrapper.cpp
  ../../../src/supla/status.cpp
  ../../../src/supla/time.cpp
  ../../../src/supla/timer.cpp
  ../../../src/supla/tools.cpp
  ../../../src/supla/uptime.cpp

  ../../../src/supla/storage/storage.cpp

  ../../../src/supla/network/network.cpp

  ../../../src/supla/clock/clock.cpp

  ../../../src/supla/control/action_trigger.cpp
  ../../../src/supla/control/bistable_relay.cpp
  ../../../src/supla/control/bistable_roller_shutter.cpp
  ../../../src/supla/control/button.cpp
  ../../../src/supla/control/dimmer_base.cpp
  ../../../src/supla/control/dimmer_leds.cpp
  ../../../src/supla/control/internal_pin_output.cpp
  ../../../src/supla/control/light_relay.cpp
  ../../../src/supla/control/pin_status_led.cpp
  ../../../src/supla/control/relay.cpp
  ../../../src/supla/control/rgb_base.cpp
  ../../../src/supla/control/rgb_leds.cpp
  ../../../src/supla/control/rgbw_base.cpp
  ../../../src/supla/control/rgbw_leds.cpp
  ../../../src/supla/control/roller_shutter.cpp
  ../../../src/supla/control/sequence_button.cpp
  ../../../src/supla/control/simple_button.cpp
  ../../../src/supla/control/virtual_relay.cpp

  ../../../src/supla/condition.cpp
  ../../../src/supla/condition_getter.cpp
  ../../../src/supla/conditions/on_less.cpp
  ../../../src/supla/conditions/on_less_eq.cpp
  ../../../src/supla/conditions/on_greater.cpp
  ../../../src/supla/conditions/on_greater_eq.cpp
  ../../../src/supla/conditions/on_between.cpp
  ../../../src/supla/conditions/on_between_eq.cpp
  ../../../src/supla/conditions/on_equal.cpp
  ../../../src/supla/conditions/on_invalid.cpp

  ../../../src/supla/device/status_led.cpp
# not all files from sensor folder are compiled here. Some still require
# porting from ARDUINO
  ../../../src/supla/sensor/binary.cpp
  ../../../src/supla/sensor/electricity_meter.cpp
  ../../../src/supla/sensor/hygro_meter.cpp
  ../../../src/supla/sensor/impulse_counter.cpp
  ../../../src/supla/sensor/therm_hygro_meter.cpp
  ../../../src/supla/sensor/therm_hygro_press_meter.cpp
  ../../../src/supla/sensor/thermometer.cpp
  ../../../src/supla/sensor/virtual_binary.cpp
  ../../../src/supla/sensor/distance.cpp
  ../../../src/supla/sensor/HC_SR04.cpp
)

set(SUPLA_DEVICE_PORTING_SRCS
  ../../porting/esp-idf/esp_idf_wifi.cpp
  ../../porting/esp-idf/esp_idf_gpio.cpp
  ../../porting/esp-idf/spiffs_storage.cpp
)


idf_component_register(SRCS
  ${SUPLA_COMMONM_SRCS}
  ${SUPLA_DEVICE_SRCS}
  ${SUPLA_DEVICE_PORTING_SRCS}
  INCLUDE_DIRS "../../../src/" "../../porting/esp-idf"
  REQUIRES esp-tls nvs_flash spiffs
  EMBED_TXTFILES supla_org_cert.pem
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC SUPLA_DEVICE)
target_compile_definitions(${COMPONENT_LIB} PUBLIC SUPLA_DEVICE_ESP32)
