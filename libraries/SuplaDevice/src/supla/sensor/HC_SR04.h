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
  HC_SR04(int8_t trigPin, int8_t echoPin, int16_t min_in = 0, int16_t max_in = 500, int16_t min_out = 0, int16_t max_out = 500) : failCount(0), lastDuration(0) {
    _trigPin = trigPin;
    _echoPin = echoPin;
    _min_in = min_in;
    _max_in = max_in;
    _min_out = min_out;
    _max_out = max_out;
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

    long distance = (duration/2) / 29.1;
    long value = map(distance, _min_in, _max_in, _min_out, _max_out);
    if (_min_out < _max_out){      
      value = constrain(value, _min_out, _max_out);      
    } else{
      value = constrain(value, _max_out, _min_out);
    }
    return failCount <= 3 ? (float)value / 100 : DISTANCE_NOT_AVAILABLE;
  }
 
  void setMinMaxIn(int16_t min_in, int16_t max_in) {
    _min_in = min_in;
    _max_in = max_in;
  }
  
  void setMinMaxOut(int16_t min_out, int16_t max_out) {
    _min_out = min_out;
    _max_out = max_out;
  }
 
 protected:
  int8_t _trigPin;
  int8_t _echoPin;
  int16_t _min_in;
  int16_t _max_in;
  int16_t _min_out;
  int16_t _max_out; 
  char failCount;
  bool ready;
  unsigned long lastDuration;
};

};  // namespace Sensor
};  // namespace Supla

#endif
