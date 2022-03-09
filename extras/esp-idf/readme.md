This folder contain esp-idf component CMakefile.txt.
It can be used for cmake based (idf.py) build on esp-idf for ESP32 targets and
for cmake and make based builds for ESP8266_RTOS_SDK targets.
Please add below line to your esp-idf project CMakefile.txt:

`set(EXTRA_COMPONENT_DIRS "/home/user/path_to_supla_device/supla-device/extras/esp-idf/")`

Or to Makefile:
EXTRA_COMPONENT_DIRS := $(abspath ../../supla-device/extras/esp-idf)

