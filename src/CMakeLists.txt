set(SRCS 
  supla/uptime.cpp
  supla/channel.cpp
  supla/channel_extended.cpp
)

add_library(supladevicelib SHARED ${SRCS})
