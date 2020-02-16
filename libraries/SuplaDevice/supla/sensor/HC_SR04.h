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
#include "supla/element.h"

#define DISTANCE_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class HC_SR04: public Element {
 public:
  HC_SR04(int8_t trigPin,int8_t echoPin) {
    channel.setType(SUPLA_CHANNELTYPE_DISTANCESENSOR);
    channel.setDefault(SUPLA_CHANNELFNC_DISTANCESENSOR);
    channel.setNewValue(getValue());
	_trigPin = trigPin;
	_echoPin = echoPin;
  }
        void onInit() {
		pinMode(_trigPin, OUTPUT); 
		pinMode(_echoPin, INPUT);
        channel.setNewValue(getValue());
  }
		
  virtual double getValue() {
  		double duration;
		digitalWrite(_trigPin, LOW);
		delayMicroseconds(2);
		digitalWrite(_trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(_trigPin, LOW);
		duration = pulseIn(_echoPin, HIGH);
    return duration*0.034/2/100;
  }

  void iterateAlways() {
    if (lastReadTime + 10000 < millis()) {
      lastReadTime = millis();
      channel.setNewValue(getValue());
    }
  }


 protected:
 int8_t _trigPin;
 int8_t _echoPin;
  Channel *getChannel() {
    return &channel;
  }
  unsigned long lastReadTime;
  Channel channel;
};

};  // namespace Sensor
};  // namespace Supla

#endif
