set(SUPLA_COMMONM_SRCS
  ../../../src/supla-common/lck.c
  ../../../src/supla-common/log.c
  ../../../src/supla-common/proto.c
  ../../../src/supla-common/srpc.c
)

set(SUPLA_DEVICE_SRCS
  ../../../src/supla/action_handler.cpp
  ../../../src/supla/at_channel.cpp
  ../../../src/supla/channel.cpp
  ../../../src/supla/channel_element.cpp
  ../../../src/supla/channel_extended.cpp
  ../../../src/supla/condition.cpp
  ../../../src/supla/condition_getter.cpp
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

  # supla/storage/storage.cpp
  #
  # supla/control/internal_pin_output.cpp
  # supla/control/pin_status_led.cpp
  # supla/control/rgbw_base.cpp
  # supla/control/rgb_base.cpp
  # supla/control/dimmer_base.cpp
  # supla/control/rgbw_leds.cpp
  # supla/control/rgb_leds.cpp
  # supla/control/dimmer_leds.cpp
  # supla/control/simple_button.cpp
  # supla/control/button.cpp
  # supla/control/action_trigger.cpp
  # supla/control/relay.cpp
  # supla/control/virtual_relay.cpp
  #
  # supla/condition.cpp
  # supla/condition_getter.cpp
  # supla/conditions/on_less.cpp
  # supla/conditions/on_less_eq.cpp
  # supla/conditions/on_greater.cpp
  # supla/conditions/on_greater_eq.cpp
  # supla/conditions/on_between.cpp
  # supla/conditions/on_between_eq.cpp
  # supla/conditions/on_equal.cpp
  # supla/conditions/on_invalid.cpp
  #
  # SuplaDevice.cpp
  # supla/network/network.cpp
  # supla/clock/clock.cpp
  #
  # supla/sensor/therm_hygro_press_meter.cpp
  # supla/sensor/therm_hygro_meter.cpp
  # supla/sensor/thermometer.cpp
  # supla/sensor/electricity_meter.cpp
)

idf_component_register(SRCS ${SUPLA_COMMONM_SRCS} ${SUPLA_DEVICE_SRCS}
                       INCLUDE_DIRS "../../../src/"
                      )

add_definitions(-DSUPLA_DEVICE)
