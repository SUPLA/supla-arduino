/*
  Copyright (C) AC SOFTWARE SP. Z O.O.
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <SPI.h>
#include <Adafruit_FRAM_SPI.h>
#include <SuplaDevice.h>

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


char tag[6] = {'S','U','P','L','A','1'};

typedef struct
{
  char tag[6];
  int position;
  unsigned int full_opening_time;
  unsigned int full_closing_time;
}RS_Settings;

RS_Settings settings;

bool FRAM_FOUND = false;

uint8_t FRAM_CS = 9;
uint8_t FRAM_SCK= 13;
uint8_t FRAM_MISO = 12;
uint8_t FRAM_MOSI = 11;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS);

void settings_init(RS_Settings *settings) {
  memset(settings, 0, sizeof(RS_Settings));
  memcpy(settings->tag, tag, 6);
}

bool fram_read(int channelNumber) {
    memset(&settings, 0, sizeof(RS_Settings));
      
    if ( FRAM_FOUND )
      {
         fram.read (sizeof(RS_Settings)*channelNumber, (uint8_t *)&settings, sizeof(RS_Settings));
         if ( memcmp(&settings.tag, tag, 6) == 0 ) 
           {
              return true;         
           }
      }

      settings_init(&settings);

return false;
}

void fram_write(int channelNumber) {
      if ( FRAM_FOUND )
      {
         fram.writeEnable(true);
         fram.write (sizeof(RS_Settings)*channelNumber, (uint8_t *)&settings, sizeof(RS_Settings));
         fram.writeEnable(false);
      }
}

void supla_rs_SavePosition(int channelNumber, int position) {
  fram_read(channelNumber);
  settings.position = position;
  fram_write(channelNumber);
}

void supla_rs_LoadPosition(int channelNumber, int *position) {
  if ( fram_read(channelNumber) )
    {
       *position = settings.position;
    }
}

void supla_rs_SaveSettings(int channelNumber, unsigned int full_opening_time, unsigned int full_closing_time) {
  fram_read(channelNumber);
  settings.full_opening_time = full_opening_time;
  settings.full_closing_time = full_closing_time;
  fram_write(channelNumber);
}

void supla_rs_LoadSettings(int channelNumber, unsigned int *full_opening_time, unsigned int *full_closing_time) {
    if ( fram_read(channelNumber) )
    {
       *full_opening_time = settings.full_opening_time;
       *full_closing_time = settings.full_closing_time;
    }
}

void setup() {

  Serial.begin(9600);

  if (fram.begin()) {
    Serial.println("Found SPI FRAM");
    FRAM_FOUND = true;
  } else {
    Serial.println("No SPI FRAM found ... check your connections\r\n");
  }
 
  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */
    

  // CHANNEL0 - TWO RELAYS (Roller shutter operation)
  SuplaDevice.addRollerShutterRelays(47,     // 46 - ﻿﻿Pin number where the 1st relay is connected   
                                     46);    // 47 - ﻿Pin number where the 2nd relay is connected  


  SuplaDevice.setRollerShutterButtons(0,    // 0 - Channel Number
                                      20,   // 20 - Pin where the 1st button is connected
                                      21);  // 21 - Pin where the 2nd button is connected


  
  SuplaDevice.setRollerShutterFuncImpl(&supla_rs_SavePosition, &supla_rs_LoadPosition, &supla_rs_SaveSettings, &supla_rs_LoadSettings);
  
  /*
   * SuplaDevice Initialization.
   * Server address, LocationID and LocationPassword are available at https://cloud.supla.org 
   * If you do not have an account, you can create it at https://cloud.supla.org/account/create
   * SUPLA and SUPLA CLOUD are free of charge
   * 
   */
 
  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "email@address",   // Email address used to login to Supla Cloud
                    AUTHKEY);          // Authorization key
    
}

void loop() {
  SuplaDevice.iterate();
}
