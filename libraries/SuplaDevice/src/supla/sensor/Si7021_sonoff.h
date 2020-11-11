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

#ifndef _si7021_sonoff_h
#define _si7021_sonoff_h

#include <supla/sensor/therm_hygro_meter.h>

namespace Supla {
namespace Sensor {
class Si7021Sonoff : public ThermHygroMeter {
 public:
  Si7021Sonoff(int pin) {
    _pin = pin;
    pinMode(_pin, INPUT);
    delay(100);
    retryCountTemp = 0;
    retryCountHumi = 0;
    lastValidTemp = TEMPERATURE_NOT_AVAILABLE;
    lastValidHumi = HUMIDITY_NOT_AVAILABLE;
  }

  double getTemp() {
    double value = TEMPERATURE_NOT_AVAILABLE;
    ReadTemp();
    value = _temp;
    if (isnan(value)) {
      value = TEMPERATURE_NOT_AVAILABLE;
    }

    if (value == TEMPERATURE_NOT_AVAILABLE) {
      retryCountTemp++;
      if (retryCountTemp > 3) {
        retryCountTemp = 0;
      } else {
        value = lastValidTemp;
      }
    } else {
      retryCountTemp = 0;
    }
    lastValidTemp = value;

    return value;
  }

  double getHumi() {
    double value = HUMIDITY_NOT_AVAILABLE;
    ReadTemp();
    value = _humidity;
    if (isnan(value)) {
      value = HUMIDITY_NOT_AVAILABLE;
    }

    if (value == HUMIDITY_NOT_AVAILABLE) {
      retryCountHumi++;
      if (retryCountHumi > 3) {
        retryCountHumi = 0;
      } else {
        value = lastValidHumi;
      }
    } else {
      retryCountHumi = 0;
    }
    lastValidHumi = value;

    return value;
  }

  void iterateAlways() {
    if (lastReadTime + 10000 < millis()) {
      lastReadTime = millis();
      channel.setNewValue(getTemp(), getHumi());
    }
  }

  void onInit() {
    channel.setNewValue(getTemp(), getHumi());
  }

 private:
  bool ReadTemp() {
    _temp = NAN;
    _humidity = NAN;

    uint8_t d[5];
    d[0] = d[1] = d[2] = d[3] = d[4] = 0;

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(500);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(20);
    pinMode(_pin, INPUT);

    uint32_t i = 0;
    if (WaitState(0) and WaitState(1) and WaitState(0)) {
      for (i = 0; i < 40; i++) {
        if (!WaitState(1)) {
          break;
        }
        delayMicroseconds(35);
        if (digitalRead(_pin) == HIGH) {
          d[i / 8] |= (1 << (7 - i % 8));
        }
        if (!WaitState(0)) {
          break;
        }
      }
    }

    if (i < 40) {
      return false;
    }

    uint8_t checksum = (d[0] + d[1] + d[2] + d[3]) & 0xFF;
    if (d[4] == checksum) {
      _temp = (((d[2] & 0x7F) << 8) | d[3]) * 0.1;
      _humidity = ((d[0] << 8) | d[1]) * 0.1;
      if (d[2] & 0x80) {
        _temp *= -1;
      }
    }

    if (isnan(_temp) || isnan(_humidity)) {
      Serial.println(F("Invalid NAN reading"));
      return false;
    }
    return true;
  }

  bool WaitState(bool state) {
    unsigned long timeout = micros();
    while (micros() - timeout < 100) {
      if (digitalRead(_pin) == state) return true;
      delayMicroseconds(1);
    }
    return false;
  }

 protected:
  int8_t _pin;
  double _temp = NAN, _humidity = NAN;
  double lastValidTemp;
  double lastValidHumi;
  int8_t retryCountTemp;
  int8_t retryCountHumi;
};

};  // namespace Sensor
};  // namespace Supla

#endif
