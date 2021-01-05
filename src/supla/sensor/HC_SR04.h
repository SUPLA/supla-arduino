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

#ifndef _hc_sr04_h
#define _hc_sr04_h

#include "supla/channel.h"
#include "supla/sensor/distance.h"

#define DURATION_COUNT 2

namespace Supla {
namespace Sensor {
class HC_SR04 : public Distance {
 public:
  HC_SR04(int8_t trigPin, int8_t echoPin) : failCount(0), lastDuration(0) {
    _trigPin = trigPin;
    _echoPin = echoPin;
  }
  void onInit() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);

    channel.setNewValue(getValue());
    channel.setNewValue(getValue());
  }

  virtual double getValue() {
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);
    unsigned long duration = pulseIn(_echoPin, HIGH, 60000);
    if (duration > 50) {
      lastDuration = duration;
      failCount = 0;
    } else {
      duration = lastDuration;
      failCount++;
    }

    return failCount <= 3 ? duration * 0.034 / 2 / 100 : DISTANCE_NOT_AVAILABLE;
  }

 protected:
  int8_t _trigPin;
  int8_t _echoPin;
  char failCount;
  bool ready;
  unsigned long lastDuration;
};

};  // namespace Sensor
};  // namespace Supla

#endif
