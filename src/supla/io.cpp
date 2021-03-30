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

#include "io.h"

#include <Arduino.h>

namespace Supla {
void Io::pinMode(uint8_t pin, uint8_t mode) {
  return pinMode(-1, pin, mode);
}

int Io::digitalRead(uint8_t pin) {
  return digitalRead(-1, pin);
}

void Io::digitalWrite(uint8_t pin, uint8_t val) {
  digitalWrite(-1, pin, val);
}

void Io::pinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  if (ioInstance) {
    ioInstance->customPinMode(channelNumber, pin, mode);
  } else { 
    ::pinMode(pin, mode);
  }
}

int Io::digitalRead(int channelNumber, uint8_t pin) {
  if (ioInstance) {
    return ioInstance->customDigitalRead(channelNumber, pin);
  }
  return ::digitalRead(pin);
}

void Io::digitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  Serial.print(" **** Digital write[");
  Serial.print(channelNumber);
  Serial.print("], pin: ");
  Serial.print(pin);
  Serial.print("; value: ");
  Serial.println(val);
  if (ioInstance) {
    ioInstance->customDigitalWrite(channelNumber, pin, val);
    return;
  }
  ::digitalWrite(pin, val);
}

Io *Io::ioInstance = 0;

Io::Io() {
  ioInstance = this;
}

Io::~Io() {
  ioInstance = nullptr;
}

int Io::customDigitalRead(int channelNumber, uint8_t pin) {
  (void)(channelNumber);
  return ::digitalRead(pin);
}

void Io::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  (void)(channelNumber);
  ::digitalWrite(pin, val);
}

void Io::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  (void)(channelNumber);
  ::pinMode(pin, mode);
}

};  // namespace Supla
