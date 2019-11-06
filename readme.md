# SUPLA project

[SUPLA](https://www.supla.org) is an open source project for home automation. 

# SuplaDevice library for Arduino IDE

SuplaDevice is a library for [Arduino IDE](https://www.arduino.cc/en/main/software) that allows to implement devices working with [Supla](https://www.supla.org).

## Hardware requirements

SuplaDevice works with Arduino Mega boards. Currently Arduino Uno is not supported.
It also works with ESP8266 based devices.

## Installation

1. Download Arduino IDE, run it and configure it. Pplease refer to other tutorials how to start with Arduino IDE.
2. Make sure that your board is working by uploading example program from Arduino IDE.
... 

## Usage

### Supported hardware
SuplaDevice is created for Arduino Mega boards. Arduino Uno is not working because of RAM limitations. However it should be possible to run on Uno board as well, 
            if memory usage of library will be reduced.
Library also works with ESP8266 based boards. It was tested on Wemos D1 R1 board.

### Network interfaces
Supported network interfaces for Arduino Mega:
* Ethernet Shield - with W5100 chipset. Include `<supla/network/ethernet_shield.h>` and add `Supla::EthernetShield ethernet;` as a global variable.
* ENC28J60 - it requires additional UIPEthenet library (https://github.com/ntruchsess/arduino_uip). Include `<supla/network/ENC28J60.h>` and 
add `Supla::ENC28J60 ethernet;` as a global variable. Warning: network initialization on this library is blocking. In case of missing ENC28J60 board
or some other problem with network, program will stuck on initialization and will not work until connection is properly esablished.
Second warning: UIPEthernet library is consuming few hundred of bytes of RAM memory more, compared to standard Ethernet library. 

Supported network interface for ESP8266:
* There is native WIFI controller. Include `<supla/network/esp_wifi.h>` and add `Supla::ESPWifi wifi(ssid, password);` as a global variable and provide SSID
and password in constructor.

### Exmaples

Each example can run on Arduino Mega or ESP8266 board. Please read comments in example files and uncomment proper library for your network interface.

SuplaSomfy, Supla_RollerShutter_FRAM - those examples are not updated yet.

## History

Version 1.7.0

## Credits


## License

Please check it [here](https://github.com/SUPLA/supla-cloud/blob/master/LICENSE).

