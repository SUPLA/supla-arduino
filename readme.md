ESP32:
Espressif ESP-IDF SDK installation steps.
Source https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-setup.html :
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
mkdir ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git


ESP8266:
Espressif RTOS-SDK ESP-IDF style.
Source: https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html
Assuming that ESP-IDF installation was already done:
sudo apt install gcc git wget make libncurses-dev flex bison gperf python
cd ~/esp
wget https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
tar -xzf xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git

Add to ~/.profile:
# ESP tools setup options
export ESP_TOOLSET="none"
alias unload_esp='export ESP_TOOLSET="none" && export PATH=$(echo $PATH | sed -e "s#${HOME}/esp/[^:]*:##g" | sed -e "s#:${HOME}/esp/xtensa-lx106-elf/bin##g") && unset IDF_PATH && unset IDF_PYTHON_ENV_PATH && unset IDF_TOOLS_EXPORT_CMD && unset IDF_TOOLS_INSTALL_CMD && unset IDF_TOOLS_PATH'
alias use_esp8266='unload_esp && export PATH="$PATH:$HOME/esp/xtensa-lx106-elf/bin" && export ESP_TOOLSET="esp8266" && source ~/esp/ESP8266_RTOS_SDK/export.sh'
alias use_esp32='unload_esp && export IDF_TOOLS_PATH=~/esp/esp_idf_tools && export ESP_TOOLSET="esp32" && source ~/esp/esp-idf/export.sh'

/usr/bin/python -m pip install --user -r ~/esp/ESP8266_RTOS_SDK/requirements.txt



FreeRTOS installation:
git clone https://github.com/FreeRTOS/FreeRTOS.git --recurse-submodules
sudo apt install libpcap-dev
