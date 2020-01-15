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

### How to migrate programs written in SuplaDevice libraray versions 1.6 and older

For Arduino Mega applications include proper network interface header:
```
// Choose proper network interface for your card:
// Arduino Mega with EthernetShield W5100:
#include <supla/network/ethernet_shield.h>
// Ethernet MAC address
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
Supla::EthernetShield ethernet(mac);
//
// Arduino Mega with ENC28J60:
// #include <supla/network/ENC28J60.h>
// Supla::ENC28J60 ethernet(mac);

```

For ESP8266 based applications include wifi header and provide WIFI SSID and password:
```
// ESP8266 based board:
#include <supla/network/esp_wifi.h>
Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
```

In case of ESP8266 remove all methods that defined network interface. They were usually added on the bottom of ino file, after end of loop() method, i.e.:
```
// Supla.org ethernet layer
    int supla_arduino_tcp_read(void *buf, int count) {
...
SuplaDeviceCallbacks supla_arduino_get_callbacks(void) {
...
```

Remove also those lines:
```
#define SUPLADEVICE_CPP
WiFiClient client;
```
You may also remove all WIFI related includes from ino file.

Common instruction for all boards:

If you use local IP address, please provide it in constructor of your network inteface class, i.e.:
```
Supla::EthernetShield ethernet(mac, localIp);
```

After that go to SuplaDevice.begin() method. Old code looked like this
```
SuplaDevice.begin(GUID,              // Global Unique Identifier 
                  mac,               // Ethernet MAC address
                  "svr1.supla.org",  // SUPLA server address
                  locationId,                 // Location ID 
                  locationPassword);          // Location Password
```
This method requires now different set of parameters:
```
  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "mail@address.pl",     // Email address
                    AUTHKEY);          // Authentication key
```
What is different? Well, GUID and Supla server address it the same as previously. MAC address, location ID, location password are removed.
MAC address was moved to network interface class. Location ID and password were replaced with new authentication method - via email address
and authentication key. You can generate your authentication key in the same way as GUID (it is actually in exactly the same format):
```
// Generate AUTHKEY from https://www.supla.org/arduino/get-guid
char AUTHKEY[SUPLA_AUTHKEY_SIZE] =  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
```

Next change is for custom digitalWrite and digitalRead methods. Those can be used to create virtual digital pins. Instead of adding custom
callback method that overrides digitalWrite/Read method, you should create a new class which inhertis from Supla::Io base class and define
your own customDigitalRead/Write methods. Here is short example (you can put this code in ino file, before setup()):
```
#include <supla/io.h>

class MyDigitalRead : public Supla::Io {
  public:
    int customDigitalRead(int channelNumber, uint8_t pin) {
      if (channelNumber == MY_EXTRA_VIRTUAL_CHANNEL) {
        return someCustomSourceOfData.getValue();
      } else {
        return ::digitalRead(pin);
      }
    }
}

MyDigitalRead instanceMyDigitalRead;
```



## History

Version 2.3.0


## Credits


## License

Please check it [here](https://github.com/SUPLA/supla-cloud/blob/master/LICENSE).

