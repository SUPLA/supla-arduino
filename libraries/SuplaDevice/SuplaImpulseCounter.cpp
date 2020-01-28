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
#include <SuplaImpulseCounter.h>

#include "crc16.h"
#include "supla-common/log.h"

const int eepromOffset =
    512;  // Starting byte where impulse counter data is stored in EEPROM

SuplaImpulseCounter *SuplaImpulseCounter::firstCounter = NULL;

SuplaImpulseCounter *SuplaImpulseCounter::getLastCounter() {
  SuplaImpulseCounter *ptr = firstCounter;
  if (ptr != NULL) {
    for (; ptr->nextCounter; ptr = ptr->nextCounter)
      ;
  }
  return ptr;
}

int SuplaImpulseCounter::count() {
  int counter = 0;
  SuplaImpulseCounter *ptr = firstCounter;
  if (firstCounter != NULL) {
    counter++;
    while (ptr->nextCounter != NULL) {
      counter++;
      ptr = ptr->nextCounter;
    }
  }
  return counter;
}

void SuplaImpulseCounter::create(int _channelNumber,
                                 int _impulsePin,
                                 int _statusLedPin,
                                 bool _detectLowToHigh,
                                 bool _inputPullup,
                                 unsigned long _debounceDelay) {
  new SuplaImpulseCounter(_channelNumber,
                          _impulsePin,
                          _statusLedPin,
                          _detectLowToHigh,
                          _inputPullup,
                          _debounceDelay);
}

void SuplaImpulseCounter::iterateAll() {
  SuplaImpulseCounter *ptr = firstCounter;
  while (ptr != NULL) {
    ptr->iterate();
    ptr = ptr->nextCounter;
  }
}

_supla_int64_t SuplaImpulseCounter::getCounter() {
  return counter;
}

void SuplaImpulseCounter::clearStorage() {
  supla_log(LOG_DEBUG, "Clearing impulse counter data stored in EEPROM... ");
  int amount =
      3 * count() * 8 +
      3 * 2;  // Single counter is stored on 8 bytes. Each counter is stored 3
              // times in a memory + 3 times 2 bytes used for CRC
  for (int i = eepromOffset; i < eepromOffset + amount; i++) {
    EEPROM.write(i, 0);
  }
  #if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.commit();
  #endif
}

void SuplaImpulseCounter::writeToStorage() {
  int address = eepromOffset;
  int countersCount = count();
  // Store 3 copies of counters data in EEPROM
  for (int copy = 0; copy < 3; copy++) {
    SuplaImpulseCounter *ptr = firstCounter;
    uint16_t crc = 0;
    for (int i = 0; i < countersCount; i++) {
      //            Serial.print("Counter id ");
      //            Serial.print(i);
      //            Serial.print("; value: ");
      _supla_int64_t value = ptr->counter;
      //            Serial.print(static_cast<int>(value));
      //            Serial.print("; address ");
      //            Serial.println(address);
      EEPROM.put(address, value);
      address +=
          sizeof(value);  // Increment address by 8 bytes (size of counter)
      ptr = ptr->nextCounter;
      unsigned char *vptr = (unsigned char *)&value;
      for (int i = 0; i < 8; i++) {
        crc = crc16_update(crc, vptr[i]);
      }
    }
    EEPROM.put(address, crc);  // Store CRC at the end of counters block
    address += sizeof(crc);
  }
  #if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.commit();
  #endif
}

void SuplaImpulseCounter::loadStorage() {
  if (firstCounter == NULL) return;

  #if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.begin(1024);  // ------------------- start eeprom before the first reading ------------------
  #endif

  supla_log(LOG_DEBUG, "Loading counters data from EEPROM");
  int address = eepromOffset;
  int countersCount = count();
  // Try 3 copies of counters data in EEPROM
  for (int copy = 0; copy < 3; copy++) {
    uint16_t crc = 0;
    SuplaImpulseCounter *ptr = firstCounter;
    for (int i = 0; i < countersCount; i++) {
      _supla_int64_t value;
      EEPROM.get(address, value);
      ptr->setCounter(value);
      address +=
          sizeof(value);  // Increment address by 8 bytes (size of counter)
      ptr = ptr->nextCounter;
      unsigned char *vptr = (unsigned char *)&value;
      for (int i = 0; i < 8; i++) {
        crc = crc16_update(crc, vptr[i]);
      }
    }
    uint16_t eepromcrc;
    EEPROM.get(address, eepromcrc);  // Store CRC at the end of counters block
    address += sizeof(crc);
    if (crc != eepromcrc) {  // implement check if CRC is OK
      supla_log(LOG_DEBUG,
                "Incorrect CRC: calaculated %d; read from EEPROM %d",
                crc,
                eepromcrc);
      ptr = firstCounter;
      while (ptr != NULL) {
        ptr->setCounter(0);
        ptr = ptr->nextCounter;
      }

      supla_log(LOG_DEBUG, "Trying another storage set");
    } else {
      supla_log(LOG_DEBUG, "CRC is OK");
      break;
    }
  }
}

void SuplaImpulseCounter::updateStorageOccasionally() {
  if (firstCounter == NULL) return;

  // 1000 ms * 60 seconds * 5 min = write every 5 minutes to EEPROM
  const unsigned long UPDATE_DELAY = static_cast<unsigned long>(1000) * 60 * 5;
  static unsigned long timeToUpdate = UPDATE_DELAY;
  static unsigned long lastUpdateTime = millis();

  unsigned long timeDiff = millis() - lastUpdateTime;

  if (timeToUpdate < timeDiff) {
    supla_log(LOG_DEBUG, "Impulse counter: EEPROM update");
    writeToStorage();
    timeToUpdate = UPDATE_DELAY;
    lastUpdateTime = millis();
  } else {
  }
}

SuplaImpulseCounter::SuplaImpulseCounter(int _channelNumber,
                                         int _impulsePin,
                                         int _statusLedPin,
                                         bool _detectLowToHigh,
                                         bool _inputPullup,
                                         unsigned long _debounceDelay) {
  // Init list of pointers to the first element
  if (firstCounter == NULL) {
    firstCounter = this;
  }
  // Add current instance to the last position on list
  nextCounter = NULL;
  SuplaImpulseCounter *ptr = getLastCounter();
  if (ptr != this) {
    ptr->nextCounter = this;
  }

  channelNumber = _channelNumber;
  impulsePin = _impulsePin;
  statusLedPin = _statusLedPin;
  debounceDelay = _debounceDelay;
  detectLowToHigh = _detectLowToHigh;
  lastImpulseMillis = 0;
  prevState = (detectLowToHigh == true ? LOW : HIGH);
  counter = 0;
  isValueChanged = true;

  supla_log(LOG_DEBUG,
            "Creating Impulse Counter: impulsePin(%d), statusLedPin(%d), "
            "delay(%d ms)",
            impulsePin,
            statusLedPin,
            debounceDelay);
  if (impulsePin <= 0) {
    supla_log(LOG_DEBUG,
              "SuplaImpulseCounter ERROR - incorrect impulse pin number");
    return;
  }

  if (_inputPullup) {
    pinMode(impulsePin, INPUT_PULLUP);
  } else {
    pinMode(impulsePin, INPUT);
  }

  if (statusLedPin <= 0) {
    //        Serial.println("SuplaImpulseCounter - status LED disabled");
    statusLedPin = 0;
  } else {
    pinMode(statusLedPin, OUTPUT);
  }
}

void SuplaImpulseCounter::debug() {
  supla_log(LOG_DEBUG,
            "SuplaImpulseCounter - channel(%d), impulsePin(%d), counter(%d)",
            channelNumber,
            impulsePin,
            static_cast<int>(counter));
}

void SuplaImpulseCounter::setCounter(_supla_int64_t value) {
  counter = value;
  isValueChanged = true;
  supla_log(LOG_DEBUG,
            "SuplaImpulseCounter - set counter to %d",
            static_cast<int>(counter));
}

void SuplaImpulseCounter::iterate() {
  int currentState = digitalRead(impulsePin);
  if (prevState == (detectLowToHigh == true ? LOW : HIGH)) {
    if (millis() - lastImpulseMillis > debounceDelay) {
      if (currentState == (detectLowToHigh == true ? HIGH : LOW)) {
        incCounter();
        lastImpulseMillis = millis();
      }
    }
  }
  prevState = currentState;
}

void SuplaImpulseCounter::incCounter() {
  counter++;
  isValueChanged = true;
  debug();
}

int SuplaImpulseCounter::getChannelNumber() {
  return channelNumber;
}

SuplaImpulseCounter *SuplaImpulseCounter::getCounterByChannel(int channel) {
  SuplaImpulseCounter *ptr = firstCounter;
  while (ptr != NULL) {
    if (ptr->getChannelNumber() == channel) {
      return ptr;
    }
    ptr = ptr->nextCounter;
  }

  return NULL;
}

bool SuplaImpulseCounter::isChanged() {
  return isValueChanged;
}

void SuplaImpulseCounter::clearIsChanged() {
  isValueChanged = false;
}
