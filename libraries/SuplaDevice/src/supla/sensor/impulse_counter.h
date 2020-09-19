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

#ifndef _supla_impulse_counter_h_
#define _supla_impulse_counter_h_

#include <supla-common/proto.h>
#include <supla/channel.h>
#include <supla/element.h>
#include <supla/triggerable.h>

namespace Supla {
namespace Sensor {
class ImpulseCounter : public Element, public Triggerable {
 public:
  ImpulseCounter(int _impulsePin,
                 bool _detectLowToHigh = false,
                 bool inputPullup = true,
                 unsigned int _debounceDelay = 10);

  void onInit();
  void onLoadState();
  void onSaveState();
  void onFastTimer();
  void runAction(int trigger, int action);

  // Returns value of a counter at given Supla channel
  _supla_int64_t getCounter();

  // Set counter to a given value
  void setCounter(_supla_int64_t value);

  // Increment the counter by 1
  void incCounter();

 protected:
  int prevState;  // Store previous state of pin (LOW/HIGH). It is used to track
                  // changes on pin state.
  int impulsePin;  // Pin where impulses are counted

  unsigned long
      lastImpulseMillis;  // Stores timestamp of last impulse (used to ignore
                          // changes of state during 10 ms timeframe)
  unsigned int debounceDelay;
  bool detectLowToHigh;  // defines if we count raining (LOW to HIGH) or falling
                         // (HIGH to LOW) edge
  bool inputPullup;

  _supla_int64_t counter;  // Actual count of impulses

  Channel *getChannel();
  Channel channel;

};
};  // namespace Sensor
};  // namespace Supla

#endif
