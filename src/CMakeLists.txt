set(SRCS 
  supla/uptime.cpp
  supla/channel.cpp
  supla/channel_extended.cpp
  supla/io.cpp
  supla/tools.cpp
  supla/element.cpp
  supla/local_action.cpp
  supla/channel_element.cpp
  supla/correction.cpp
  supla/at_channel.cpp
  supla/action_handler.cpp
  
  supla/storage/storage.cpp

  supla/control/internal_pin_output.cpp
  supla/control/pin_status_led.cpp
  supla/control/rgbw_base.cpp
  supla/control/rgb_base.cpp
  supla/control/dimmer_base.cpp
  supla/control/rgbw_leds.cpp
  supla/control/rgb_leds.cpp
  supla/control/dimmer_leds.cpp
  supla/control/simple_button.cpp
  supla/control/button.cpp
  supla/control/action_trigger.cpp
  supla/control/relay.cpp
  supla/control/virtual_relay.cpp

  supla/condition.cpp
  supla/condition_getter.cpp
  supla/conditions/on_less.cpp
  supla/conditions/on_less_eq.cpp
  supla/conditions/on_greater.cpp
  supla/conditions/on_greater_eq.cpp
  supla/conditions/on_between.cpp
  supla/conditions/on_between_eq.cpp
  supla/conditions/on_equal.cpp
  supla/conditions/on_invalid.cpp

  SuplaDevice.cpp
  supla/network/network.cpp
  supla/clock/clock.cpp

  supla/sensor/therm_hygro_press_meter.cpp
  supla/sensor/therm_hygro_meter.cpp
  supla/sensor/thermometer.cpp
  supla/sensor/electricity_meter.cpp
)

add_library(supladevicelib SHARED ${SRCS})
