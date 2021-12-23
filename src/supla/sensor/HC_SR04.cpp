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

#include "HC_SR04.h"

namespace Supla {
namespace Sensor {
HC_SR04::HC_SR04(int8_t trigPin,
                 int8_t echoPin,
                 int16_t minIn,
                 int16_t maxIn,
                 int16_t minOut,
                 int16_t maxOut)
    : failCount(0), readouts{}, index(0) {
  _trigPin = trigPin;
  _echoPin = echoPin;
  _minIn = minIn;
  _maxIn = maxIn;
  _minOut = minOut;
  _maxOut = maxOut;
}

void HC_SR04::onInit() {
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);

  channel.setNewValue(getValue());
  channel.setNewValue(getValue());
}

double HC_SR04::getValue() {
  noInterrupts();
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  unsigned long duration = pulseIn(_echoPin, HIGH, 60000);
  interrupts();
  if (duration > 50) {
    index++;
    if (index > 4) index = 0;
    readouts[index] = duration;
    failCount = 0;
  } else {
    failCount++;
  }

  unsigned long min = 0, max = 0, sum = 0;
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (readouts[i] > 0) {
      count++;
      if (min > readouts[i] || min == 0) min = readouts[i];
      if (max < readouts[i]) max = readouts[i];
      sum += readouts[i];
    }
  }

  if (count == 5) {
    if (min > 0) {
      sum -= min;
      count--;
    }
    if (max > 0) {
      sum -= max;
      count--;
    }
  }
  if (count > 0) {
    duration = sum / count;
  } 

  long distance = (duration / 2.0) / 29.1;
  long value = map(distance, _minIn, _maxIn, _minOut, _maxOut);
  if (_minOut < _maxOut) {
    value = constrain(value, _minOut, _maxOut);
  } else {
    value = constrain(value, _maxOut, _minOut);
  }
  return failCount <= 3 ? static_cast<double>(value) / 100.0
                        : DISTANCE_NOT_AVAILABLE;
}

void HC_SR04::setMinMaxIn(int16_t minIn, int16_t maxIn) {
  _minIn = minIn;
  _maxIn = maxIn;
}

void HC_SR04::setMinMaxOut(int16_t minOut, int16_t maxOut) {
  _minOut = minOut;
  _maxOut = maxOut;
}

};  // namespace Sensor
};  // namespace Supla
