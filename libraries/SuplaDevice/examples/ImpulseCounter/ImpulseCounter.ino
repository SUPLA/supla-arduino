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
#include <SuplaDevice.h>
#include <SuplaImpulseCounter.h>

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
//For ESP based boards (ESP8266) you can use secure connections via SSL. 
//To do this, uncomment the following line
//#define ESP_SSL
//
// ESP8266 based board:
// #include <supla/network/esp_wifi.h>
// Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
//
// ESP32 based board:
// #include <supla/network/esp32_wifi.h>
// Supla::ESP32Wifi wifi("your_wifi_ssid", "your_wifi_password");



void setup() {

  Serial.begin(9600);

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
    
  // CHANNEL0 - Impulse Counter on pin 34, without status LED (it is not implemented yet), counting raising edge (from LOW to HIGH), no pullup on pin, and 10 ms debounce timeout
  SuplaDevice.addImpulseCounter(34, 0, true, false, 10);
  
  // CHANNEL1 - Impulse Counter on pin 34, without status LED (it is not implemented yet), counting folling edge (from HIGH to LOW), with pullup on pin, and 50 ms debounce timeout
  SuplaDevice.addImpulseCounter(35, 0, false, true, 50);
  
  // Uncomment below line to cleanup the counter storage data
//  SuplaImpulseCounter::clearStorage();
   
  /* If you are using secure connections over SSL (ESP8266 boards only),and you know the supla-server certificate fingerprint
   * uncomment following lines and fill proper server's certificate thumbprint
   * Remember if certificate would change you will need to change that value if uncommented
   * If you leave it commented the certificate will not be validated but connection will be encrypted
   */ 
  
  //char fingerprint[MAX_FINGERPRINT_SIZE] = "9b:a8:18:b9:5e:d6:06:52:f8:22:15:00:e1:56:68:8d:7a:61:11:77";
  //wifi.setFingerprint(fingerprint);
  

  /*
   * SuplaDevice Initialization.
   * Server address is available at https://cloud.supla.org 
   * If you do not have an account, you can create it at https://cloud.supla.org/account/create
   * SUPLA and SUPLA CLOUD are free of charge
   * 
   */

  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "email@address",   // Email address used to login to Supla Cloud
                    AUTHKEY, 		   // Authorization key
					2015);             // SUPLA server port number (default 2015 and 2016 for SSL)
    
}

void loop() {
  SuplaDevice.iterate();
}

