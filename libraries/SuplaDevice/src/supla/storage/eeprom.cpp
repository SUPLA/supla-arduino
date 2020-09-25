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

#include <Arduino.h>
#include <EEPROM.h>

#include "eeprom.h"

using namespace Supla;

#define SUPLA_EEPROM_WRITING_PERIOD 10000

Eeprom::Eeprom(unsigned int storageStartingOffset)
    : Storage(storageStartingOffset),
      lastWriteTimestamp(0),
      dataChanged(false) {
}

bool Eeprom::init() {
#if defined(ARDUINO_ARCH_ESP8266) 
  EEPROM.begin(1024);
#elif defined(ARDUINO_ARCH_ESP32)
  EEPROM.begin(512);
#endif
  delay(15);

  return Storage::init();
}

int Eeprom::readStorage(int offset, unsigned char *buf, int size, bool logs) {
  if (logs) {
    Serial.print(F("readStorage: "));
    Serial.print(size);
    Serial.print(F("; Read: ["));
  }
  for (int i = 0; i < size; i++) {
    buf[i] = EEPROM.read(offset + i);
    if (logs) {
      Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
      Serial.print(F(" "));
    }
  }
  if (logs) {
    Serial.println(F("]"));
  }
  return size;
}

int Eeprom::writeStorage(int offset, const unsigned char *buf, int size) {
  dataChanged = true;
  for (int i = 0; i < size; i++) {
    EEPROM.write(offset + i, buf[i]);
  }
  Serial.print(F("Wrote "));
  Serial.print(size);
  Serial.println(F(" bytes to storage"));
  return size;
}

bool Eeprom::saveStateAllowed(unsigned long ms) {
  if (ms - lastWriteTimestamp > SUPLA_EEPROM_WRITING_PERIOD) {
    lastWriteTimestamp = ms;
    return true;
  }
  return false;
}

void Eeprom::commit() {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  if (dataChanged) {
    EEPROM.commit();
    Serial.println(F("Commit"));
  }
#endif
  dataChanged = false;
}
