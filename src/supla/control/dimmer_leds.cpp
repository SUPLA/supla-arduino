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

#include "dimmer_leds.h"

#ifdef ARDUINO_ARCH_ESP32
extern int esp32PwmChannelCouner;
#endif

Supla::Control::DimmerLeds::DimmerLeds(int brightnessPin)
    : brightnessPin(brightnessPin) {
}

void Supla::Control::DimmerLeds::setRGBWValueOnDevice(uint8_t red,
                                                      uint8_t green,
                                                      uint8_t blue,
                                                      uint8_t colorBrightness,
                                                      uint8_t brightness) {
  int multiplier = 1;
#ifdef ARDUINO_ARCH_ESP8266
  multiplier = 4;
#endif
#ifdef ARDUINO_ARCH_ESP32
  multiplier = 4;
#endif

  int brightnessAdj = brightness * multiplier * 255 / 100;
#ifdef ARDUINO_ARCH_ESP32
  ledcWrite(brightnessPin, brightnessAdj);
#else
  analogWrite(brightnessPin, brightnessAdj);
#endif
}

void Supla::Control::DimmerLeds::onInit() {
#ifdef ARDUINO_ARCH_ESP32
  Serial.print(F("Dimmer: attaching pin "));
  Serial.print(brightnessPin);
  Serial.print(F(" to PWM channel: "));
  Serial.println(esp32PwmChannelCouner);

  ledcSetup(esp32PwmChannelCouner, 12000, 10);
  ledcAttachPin(brightnessPin, esp32PwmChannelCouner);
  // on ESP32 we write to PWM channels instead of pins, so we copy channel
  // number as pin in order to reuse variable
  brightnessPin = esp32PwmChannelCouner;
  esp32PwmChannelCouner++;
#else
  pinMode(brightnessPin, OUTPUT);

#ifdef ARDUINO_ARCH_ESP8266
  analogWriteRange(1024);
#endif
#endif

  Supla::Control::DimmerBase::onInit();
}
