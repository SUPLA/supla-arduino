FROM ubuntu:latest
LABEL maintainer="Krzysztof Lewandowski <krzysiek@supla.org>"

# Install required packages
RUN apt update && \
    DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt install -y tzdata && \
    apt install -y git wget flex bison gperf python3 python3-pip \
        python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev \
        dfu-util libusb-1.0-0 python-is-python3 gcc make libncurses-dev

# Add supla user and setup workdir
RUN useradd -ms /bin/bash supla && usermod -a -G dialout supla
USER supla
WORKDIR /home/supla

# Download and install espressif toolset and ESP8266 RTOS SDK
RUN mkdir esp && \
    cd esp && \
    git clone --recursive https://github.com/espressif/esp-idf.git && \
    cd esp-idf && \
    ./install.sh all

# Download supla-device
RUN git clone https://github.com/SUPLA/supla-device.git && mkdir projects
WORKDIR /home/supla/projects

ENV SUPLA_DEVICE_PATH="/home/supla/supla-device"
ENV ESP_TOOLSET="esp32"
RUN echo "source /home/supla/esp/esp-idf/export.sh" >> ~/.bashrc

