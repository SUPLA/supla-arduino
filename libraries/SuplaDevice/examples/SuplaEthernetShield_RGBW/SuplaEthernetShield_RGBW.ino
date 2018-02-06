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
#include <Ethernet.h>
#include <SuplaDevice.h>

/*
* Youtube: https://youtu.be/FE9tqzTjmA4
*/


 #define RED_PIN    44
 #define GREEN_PIN  45
 #define BLUE_PIN   46
 #define BRIGHTNESS_PIN        7
 #define COLOR_BRIGHTNESS_PIN  8

 unsigned char _red = 0;
 unsigned char _green = 255;
 unsigned char _blue = 0;
 unsigned char _color_brightness = 0;
 unsigned char _brightness = 0;

void get_rgbw_value(int channelNumber, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *color_brightness, unsigned char *brightness) {

  *brightness = _brightness;
  *color_brightness= _color_brightness;

  *red = _red;
  *green = _green;
  *blue = _blue;

}

void set_rgbw() {
    
    analogWrite(BRIGHTNESS_PIN, (_brightness * 255) / 100);
    analogWrite(COLOR_BRIGHTNESS_PIN, (_color_brightness * 255) / 100);
    analogWrite(RED_PIN, _red);
    analogWrite(GREEN_PIN, _green);
    analogWrite(BLUE_PIN, _blue);
}

void set_rgbw_value(int channelNumber, unsigned char red, unsigned char green, unsigned char blue, unsigned char color_brightness, unsigned char brightness) {

    _brightness = brightness;
    _color_brightness= color_brightness;
  
    _red = red;
    _green = green;
    _blue = blue;  
    
    set_rgbw();
  
}


void setup() {

  Serial.begin(9600);

  set_rgbw();
  
  // Set RGBW callbacks
  SuplaDevice.setRGBWCallbacks(&get_rgbw_value, &set_rgbw_value);

  // ﻿Replace the falowing GUID
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  // ﻿with GUID that you can retrieve from https://www.supla.org/arduino/get-guid


  // Ethernet MAC address
  uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */
    
  // CHANNEL0 - RGB controller and dimmer (RGBW)
  SuplaDevice.addRgbControllerAndDimmer();    

  // SuplaDevice.addRgbController(); 
  // SuplaDevice.addDimmer(); 

  /*
   * SuplaDevice Initialization.
   * Server address, LocationID and LocationPassword are available at https://cloud.supla.org 
   * If you do not have an account, you can create it at https://cloud.supla.org/account/create
   * SUPLA and SUPLA CLOUD are free of charge
   * 
   */

  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    mac,               // Ethernet MAC address
                    "svr1.supla.org",  // SUPLA server address
                    0,                 // Location ID 
                    "");               // Location Password
    
}

void loop() {
  SuplaDevice.iterate();
}
