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


#include "Arduino.h"
#include <gmock/gmock.h>

SerialStub Serial;

class DigitalInterface {
  public:
    DigitalInterface() {
      instance = this;
    }
    virtual ~DigitalInterface() {
      instance = nullptr;
    }

    virtual void digitalWrite(uint8_t, uint8_t) = 0;
    virtual int digitalRead(uint8_t) = 0;
    virtual void pinMode(uint8_t, uint8_t) = 0;
    
    static DigitalInterface *instance;
};

DigitalInterface *DigitalInterface::instance = nullptr;

class DigitalInterfaceMock : public DigitalInterface {
  MOCK_METHOD(void, digitalWrite, (uint8_t, uint8_t), (override));
//  MOCK_METHOD(int, digitalRead, (uint8_t), (override));
 // MOCK_METHOD(void, pinMode, (uint8_t, uint8_t), (override));

};

void digitalWrite(uint8_t pin, uint8_t val) {
  DigitalInterface::instance->digitalWrite(pin, val);
}

int digitalRead(uint8_t pin) {
  return DigitalInterface::instance->digitalRead(pin);
}

void pinMode(uint8_t pin, uint8_t mode) {
  DigitalInterface::instance->pinMode(pin, mode);
}

