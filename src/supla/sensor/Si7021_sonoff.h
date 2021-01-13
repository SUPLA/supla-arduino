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

#include <Arduino.h>
#include <supla/sensor/therm_hygro_meter.h>


namespace Supla {
namespace Sensor {
class Si7021Sonoff : public ThermHygroMeter {
 public:
  Si7021Sonoff(int pin);
  double getTemp();
  double getHumi();

 private:
  void iterateAlways();
  void onInit();
  double readTemp(uint8_t* data);
  double readHumi(uint8_t* data);
  bool read();
  bool waitState(bool state);

 protected:
  int8_t pin;
  double temperature;
  double humidity;
  int8_t retryCount;
};

};  // namespace Sensor
};  // namespace Supla

#endif