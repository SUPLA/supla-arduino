set(SRCS 
  supla/uptime.cpp
  supla/channel.cpp
  supla/channel_extended.cpp
  supla/io.cpp
  supla/tools.cpp
  supla/element.cpp
  supla/local_action.cpp
)

add_library(supladevicelib SHARED ${SRCS})
