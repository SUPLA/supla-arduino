#include <SPI.h>
#include <SuplaDevice.h>
#include <SuplaSomfy.h>
#include <EEPROM.h>

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
//
// ESP8266 based board:
// #include <supla/network/esp_wifi.h>
// Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
//
// ESP32 based board:
// #include <supla/network/esp32_wifi.h>
// Supla::ESP32Wifi wifi("your_wifi_ssid", "your_wifi_password");


#define RC_COUNT SUPLA_CHANNELMAXCOUNT
#define BEGIN_PIN 100
#define BEGIN_RC_ADDRESS 5

volatile uint8_t pin100;  //down
volatile uint8_t pin101;  //up
volatile uint8_t pinState[RC_COUNT][2];

SuplaSomfy myRemote(10);

uint8_t pinChanel(uint8_t chanel) {
  return BEGIN_PIN + (2 * chanel);
}

uint8_t addressChanel(uint8_t chanel) {
  uint8_t address = BEGIN_RC_ADDRESS + (chanel * 5);
}

void saveRollingCode(somfy_remote_t rc, uint8_t chanel) {
  uint8_t address = addressChanel(chanel);
  EEPROM.write(address + 3, rc.rollingCode.svalue.byte1);
  EEPROM.write(address + 4, rc.rollingCode.svalue.byte2);
}

void saveRemote(somfy_remote_t rc, uint8_t chanel) {
  uint8_t address = addressChanel(chanel);
  EEPROM.write(address, rc.remoteControl.svalue.byte1);
  EEPROM.write(address + 1, rc.remoteControl.svalue.byte2);
  EEPROM.write(address + 2, rc.remoteControl.svalue.byte3);
  //EEPROM.write(address + 3, rc.rollingCode.svalue.byte1);
  //EEPROM.write(address + 4, rc.rollingCode.svalue.byte2);
  saveRollingCode(rc, chanel);
}

somfy_remote_t loadRemote(uint8_t chanel) {
  somfy_remote_t rc;
  uint8_t address = addressChanel(chanel);
  rc.remoteControl.svalue.byte1 = EEPROM.read(address);
  rc.remoteControl.svalue.byte1 = EEPROM.read(address + 1);
  rc.remoteControl.svalue.byte1 = EEPROM.read(address + 2);
  rc.rollingCode.svalue.byte1 = EEPROM.read(address + 3);
  rc.rollingCode.svalue.byte1 = EEPROM.read(address + 4);

  return rc;
}

void createRemote(uint8_t count) {
  String cfg;
  for (uint8_t i = 0; i < 3; i++) {
    cfg += char(EEPROM.read(i));
  }
  if (cfg != "CFG") {
    somfy_remote_t remote;
    remote.rollingCode.svalue = { 0x00, 0x01 };
    remote.remoteControl.svalue = { random(0xFF), random(0xFF), random(0xFF) };

    for (uint8_t i = 0; i < count; i++) {
      saveRemote(remote, i);
      remote.remoteControl.ivalue++;
    }
    EEPROM.write(0, "C");
    EEPROM.write(1, "F");
    EEPROM.write(2, "G");
  }
}

void showRSCommand() {
  Serial.println("====== SOMFY REMOTE ======");
  Serial.println("U [chanel] => UP");
  Serial.println("S [chanel] => STOP");
  Serial.println("D [chanel] => DOWN");
  Serial.println("P [chanel] => PROGRAM");
}

void supla_Timer() {

    if (Serial.available()) {
    char cmd = (char)Serial.read();
    if (isSpace((char)Serial.read())) {
      int chanel = Serial.parseInt();
      if (chanel >= 0 && chanel < RC_COUNT) {
        switch (toupper(cmd)) {
          case 'U': {
              pushButton(chanel, UP);
              break;
            }
          case 'S': {
              pushButton(chanel, STOP);
              break;
            }
          case 'D': {
              pushButton(chanel, DOWN);
              break;
            }
          case 'P': {
              pushButton(chanel, PROG);
              break;
            }
        }
      }
    }
  }
  
}


void setup() {
  Serial.begin(115200);

  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Generate remote controls
  createRemote(RC_COUNT);

  // Show RS command
  showRSCommand();

  // CHANNEL3 - TWO RELAYS (Roller shutter operation)
  for (uint8_t i = 0; i < RC_COUNT; i++)
  {
    uint8_t pin = pinChanel(i);
    SuplaDevice.addRollerShutterRelays(pin,         // 100 - ﻿﻿Pin number where the 1st relay is connected
                                       pin + 1);    // 101 - ﻿Pin number where the 2nd relay is connected
  }

  SuplaDevice.setTimerFuncImpl(&supla_Timer);
  SuplaDevice.setDigitalReadFuncImpl(&supla_DigitalRead);
  SuplaDevice.setDigitalWriteFuncImpl(&supla_DigitalWrite);
  SuplaDevice.setName("Somfy Remote");

  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "email@address",   // Email address used to login to Supla Cloud
                    AUTHKEY);          // Authorization key

  //testRemote();
}

void loop() {
  SuplaDevice.iterate();
}

void testRemote() {
  somfy_remote_t remote;
  remote.rollingCode.svalue = {0x00, 0x01};
  remote.remoteControl.svalue = {0xAB, 0xCD, 0xEF};
  Serial.print("Rolling Code:"); myRemote.PrintHex8(remote.rollingCode.ivalue, 2);
  Serial.print("Remote SN:"); myRemote.PrintHex8(remote.remoteControl.ivalue, 3);
  myRemote.SetRemote(remote);
  myRemote.PushButton(UP);
  Serial.print("Rolling Code:"); myRemote.PrintHex8(remote.rollingCode.ivalue, 2);
  Serial.print("Remote SN:"); myRemote.PrintHex8(remote.remoteControl.ivalue, 3);
  remote = myRemote.GetRemote();
  Serial.print("Rolling Code:"); myRemote.PrintHex8(remote.rollingCode.ivalue, 2);
  Serial.print("Remote SN:"); myRemote.PrintHex8(remote.remoteControl.ivalue, 3);

  Serial.println("");
  Serial.print("Size remote:"); Serial.println(sizeof(remote));
  //Serial.print("Remote:"); myRemote.PrintHex8(&remote, sizeof(remote));
}

int supla_DigitalRead(int channelNumber, uint8_t pin) {
  Serial.println("");
  Serial.println(">DigitalRead<");
  Serial.print("Chanel:"); Serial.println(channelNumber);
  Serial.print("Pin:"); Serial.println(pin);

  uint8_t vpin = pinChanel(channelNumber);

  if (pin == vpin) {
    Serial.print("Value:"); Serial.println(pinState[channelNumber][0]);
    return pinState[channelNumber][0];
  }

  if (pin == vpin + 1) {
    Serial.print("Value:"); Serial.println(pinState[channelNumber][1]);
    return pinState[channelNumber][1];
  }
}

void supla_DigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  Serial.println("");
  Serial.println(">DigitalWrite<");
  Serial.print("Chanel:"); Serial.println(channelNumber);
  Serial.print("Pin:"); Serial.println(pin);
  Serial.print("Value:"); Serial.println(val);

  uint8_t vpin = pinChanel(channelNumber);

  if (pin == vpin) {
    // Sterowanie Somfy po 433
    pinState[channelNumber][0] = val;
  }

  if (pin == vpin + 1) {
    // Sterowanie Somfy po 433
    pinState[channelNumber][1] = val;
  }

  somfy(channelNumber);
}

void somfy(uint8_t channelNumber) {
  if (pinState[channelNumber][0] == 0 && pinState[channelNumber][1] == 1) {
    Serial.println("SOMFY UP");
    pushButton(channelNumber, UP);
  }

  else if (pinState[channelNumber][0] == 1 && pinState[channelNumber][1] == 0) {
    Serial.println("SOMFY DOWN");
    pushButton(channelNumber, DOWN);
  }

  else if (pinState[channelNumber][0] == 0 && pinState[channelNumber][1] == 0) {
    Serial.println("SOMFY STOP");
    pushButton(channelNumber, STOP);
  }
}

void pushButton(int channelNumber, ControlButtons button) {
  myRemote.SetRemote(loadRemote(channelNumber));
  myRemote.PushButton(button);
  saveRollingCode(myRemote.GetRemote(), channelNumber);
}
