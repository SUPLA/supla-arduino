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

#ifndef _sht3x_h
#define _sht3x_h

// Dependency: ClosedCube SHT3x library - use library manager to install it
// https://github.com/closedcube/ClosedCube_SHT31D_Arduino

#include <ClosedCube_SHT31D.h>

#include "therm_hygro_meter.h"


namespace Supla {
namespace Sensor {
class SHT3x : public ThermHygroMeter {
 public:
  SHT3x(int8_t address = 0x44)
      : temperature(TEMPERATURE_NOT_AVAILABLE),
        humidity(HUMIDITY_NOT_AVAILABLE),
        address(address),
        retryCount(0) {
  }

  double getTemp() {
    return temperature;
  }

  double getHumi() {
    return humidity;
  }

 private:
  void iterateAlways() {
    if (millis() - lastReadTime > 10000) {
      lastReadTime = millis();
      readValuesFromDevice();
      channel.setNewValue(getTemp(), getHumi());
    }
  }

  void onInit() {
    sht.begin(address);
    readValuesFromDevice();
    channel.setNewValue(getTemp(), getHumi());
  }

  void readValuesFromDevice() {
    SHT31D result = sht.readTempAndHumidity(
        SHT3XD_REPEATABILITY_LOW, SHT3XD_MODE_CLOCK_STRETCH, 50);

    if (result.error != SHT3XD_NO_ERROR) {
      Serial.print(F("SHT [ERROR] Code #"));
      Serial.println(result.error);
      retryCount++;
      if (retryCount > 3) {
        retryCount = 0;
        temperature = TEMPERATURE_NOT_AVAILABLE;
        humidity = HUMIDITY_NOT_AVAILABLE;
      }
    } else {
      retryCount = 0;
      temperature = result.t;
      humidity = result.rh;
    }
  }

 protected:
  int8_t address;
  double temperature;
  double humidity;
  int8_t retryCount;
  ::ClosedCube_SHT31D sht;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif
