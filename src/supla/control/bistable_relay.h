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

/* BistableRelay
 * This class can be used to controll bistable relay.
 * Supla device will send short impulses (<0.5 s) on GPIO to toggle bistable
 * relay state.
 * Device does not have knowledge about the status of bistable relay, so it
 * has to be read on a different GPIO (statusPin)
 * This class can work without statusPin information, but Supla will lose
 * information about status of bistable relay.
 */

#ifndef _bistable_relay_h
#define _bistable_relay_h

#include "relay.h"

namespace Supla {
namespace Control {
class BistableRelay : public Relay {
 public:
  BistableRelay(int pin,
                int statusPin = -1,
                bool statusPullUp = true,
                bool statusHighIsOn = true,
                bool highIsOn = true,
                _supla_int_t functions =
                    (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));
  void onInit();
  void iterateAlways();
  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);
  void turnOn(_supla_int_t duration = 0);
  void turnOff(_supla_int_t duration = 0);
  void toggle(_supla_int_t duration = 0);

  virtual bool isOn();
  bool isStatusUnknown();

 protected:
  void internalToggle();

  int statusPin;
  bool statusPullUp;
  bool statusHighIsOn;
  unsigned long disarmTimeMs;
  unsigned long lastReadTime;
  bool busy;
};

};  // namespace Control
};  // namespace Supla

#endif
