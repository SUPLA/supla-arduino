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

#include "rgb_leds.h"

#ifdef ARDUINO_ARCH_ESP32
extern int esp32PwmChannelCouner;
#endif

Supla::Control::RGBLeds::RGBLeds(int redPin,
                                   int greenPin,
                                   int bluePin,
                                   int colorBrightnessPin)
    : redPin(redPin),
      greenPin(greenPin),
      bluePin(bluePin),
      colorBrightnessPin(colorBrightnessPin)
{}

void Supla::Control::RGBLeds::setRGBWValueOnDevice(uint8_t red,
                          uint8_t green,
                          uint8_t blue,
                          uint8_t colorBrightness,
                          uint8_t brightness) {
#ifdef ARDUINO_ARCH_ESP32
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
  ledcWrite(colorBrightnessPin, (colorBrightness * 255) / 100);
#else
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
  analogWrite(colorBrightnessPin, (colorBrightness * 255) / 100);
#endif
}

void Supla::Control::RGBLeds::onInit() {
#ifdef ARDUINO_ARCH_ESP32
  Serial.print(F("RGB: attaching pin "));
  Serial.print(redPin);
  Serial.print(F(" to PWM channel: "));
  Serial.println(esp32PwmChannelCouner);

  ledcSetup(esp32PwmChannelCouner, 12000, 8);
  ledcAttachPin(redPin, esp32PwmChannelCouner);
  // on ESP32 we write to PWM channels instead of pins, so we copy channel
  // number as pin in order to reuse variable
  redPin = esp32PwmChannelCouner;
  esp32PwmChannelCouner++;

  ledcSetup(esp32PwmChannelCouner, 12000, 8);
  ledcAttachPin(greenPin, esp32PwmChannelCouner);
  greenPin = esp32PwmChannelCouner;
  esp32PwmChannelCouner++;

  ledcSetup(esp32PwmChannelCouner, 12000, 8);
  ledcAttachPin(bluePin, esp32PwmChannelCouner);
  bluePin = esp32PwmChannelCouner;
  esp32PwmChannelCouner++;

  ledcSetup(esp32PwmChannelCouner, 12000, 8);
  ledcAttachPin(colorBrightnessPin, esp32PwmChannelCouner);
  colorBrightnessPin = esp32PwmChannelCouner;
  esp32PwmChannelCouner++;
#else
  pinMode(redPin, OUTPUT); 
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT); 
  pinMode(colorBrightnessPin, OUTPUT); 

 #ifdef ARDUINO_ARCH_ESP8266
  analogWriteRange(255);
 #endif 
#endif

  Supla::Control::RGBBase::onInit();
}

