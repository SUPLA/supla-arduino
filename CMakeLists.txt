set(SUPLA_DEVICE_SRCS
  src/SuplaDevice.cpp

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
  src/supla/mutex.cpp
  src/supla/auto_lock.cpp
  src/supla/sha256.cpp
  src/supla/rsa_verificator.cpp

  src/supla/condition.cpp
  src/supla/condition_getter.cpp
  src/supla/conditions/on_less.cpp
  src/supla/conditions/on_less_eq.cpp
  src/supla/conditions/on_greater.cpp
  src/supla/conditions/on_greater_eq.cpp
  src/supla/conditions/on_between.cpp
  src/supla/conditions/on_between_eq.cpp
  src/supla/conditions/on_equal.cpp
  src/supla/conditions/on_invalid.cpp

  src/supla/clock/clock.cpp

  src/supla/device/last_state_logger.cpp
  src/supla/device/status_led.cpp
  src/supla/device/sw_update.cpp

  src/supla/storage/storage.cpp
  src/supla/storage/config.cpp
  src/supla/storage/key_value.cpp

  src/supla/network/network.cpp
  src/supla/network/html_element.cpp
  src/supla/network/html_generator.cpp
  src/supla/network/web_server.cpp
  src/supla/network/web_sender.cpp
  src/supla/network/netif_wifi.cpp
  src/supla/network/html/device_info.cpp
  src/supla/network/html/protocol_parameters.cpp
  src/supla/network/html/status_led_parameters.cpp
  src/supla/network/html/wifi_parameters.cpp
  src/supla/network/html/sw_update_beta.cpp
  src/supla/network/html/sw_update.cpp
  src/supla/network/html/custom_sw_update.cpp

  src/supla/control/action_trigger.cpp
  src/supla/control/bistable_relay.cpp
  src/supla/control/bistable_roller_shutter.cpp
  src/supla/control/button.cpp
  src/supla/control/dimmer_base.cpp
  src/supla/control/dimmer_leds.cpp
  src/supla/control/internal_pin_output.cpp
  src/supla/control/light_relay.cpp
  src/supla/control/pin_status_led.cpp
  src/supla/control/relay.cpp
  src/supla/control/rgb_base.cpp
  src/supla/control/rgb_leds.cpp
  src/supla/control/rgbw_base.cpp
  src/supla/control/rgbw_leds.cpp
  src/supla/control/roller_shutter.cpp
  src/supla/control/sequence_button.cpp
  src/supla/control/simple_button.cpp
  src/supla/control/virtual_relay.cpp

  # FreeRTOS target can't compile those files currently. So they are removed
  # here and added in linux porting cmake only;
  # TODO add freertos implementation
  #  src/supla/pv/fronius.cpp
  #  src/supla/pv/afore.cpp

# not all files from sensor folder are compiled here. Some still require
# porting from ARDUINO
  src/supla/sensor/binary.cpp
  src/supla/sensor/electricity_meter.cpp
  src/supla/sensor/hygro_meter.cpp
  src/supla/sensor/impulse_counter.cpp
  src/supla/sensor/therm_hygro_meter.cpp
  src/supla/sensor/therm_hygro_press_meter.cpp
  src/supla/sensor/thermometer.cpp
  src/supla/sensor/virtual_binary.cpp
)

set(NETTLE_SRCS
  src/nettle/bignum.c
  src/nettle/gmp-glue.c
  src/nettle/mini-gmp.c
  src/nettle/pkcs1-rsa-sha256.c
  src/nettle/pkcs1.c
  src/nettle/rsa-sha256-verify.c
  src/nettle/rsa-verify.c
  src/nettle/rsa.c
  src/nettle/sha256-compress.c
  src/nettle/sha256.c
  src/nettle/write-be32.c
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
  ${NETTLE_SRCS}
  )

target_include_directories (supladevice
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/supla-common
  )

target_compile_definitions(supladevice PUBLIC SUPLA_DEVICE)
