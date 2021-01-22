set(SRCS 
  supla/uptime.cpp
  supla/channel.cpp
  supla/channel_extended.cpp
  supla/io.cpp
  supla/tools.cpp
  supla/element.cpp
  supla/local_action.cpp
  supla/channel_element.cpp
  supla/control/internal_pin_output.cpp
  supla/control/pin_status_led.cpp
  supla/condition.cpp
  supla/conditions/on_less.cpp
  supla/conditions/while_less.cpp
)

add_library(supladevicelib SHARED ${SRCS})
