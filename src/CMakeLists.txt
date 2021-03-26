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

  supla/condition.cpp
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

)

add_library(supladevicelib SHARED ${SRCS})
