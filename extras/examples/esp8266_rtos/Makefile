#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := supla-esp8266-mk

ifdef SUPLA_DEVICE_PATH
EXTRA_COMPONENT_DIRS := $(abspath ${SUPLA_DEVICE_PATH}/extras/esp-idf)
$(info Using supla-device from SUPLA_DEVICE_PATH: ${SUPLA_DEVICE_PATH})
else
EXTRA_COMPONENT_DIRS := $(abspath ../../supla-device/extras/esp-idf)
$(info Using supla-device from relative path ../../supla-device/)
endif

CFLAGS += -DSUPLA_DEVICE
CPPFLAGS += -DSUPLA_DEVICE

include $(IDF_PATH)/make/project.mk

