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

#ifndef _virtual_relay_h
#define _virtual_relay_h

#include "relay.h"

namespace Supla {
namespace Control {
class VirtualRelay : public Relay {
 public:
  VirtualRelay(_supla_int_t functions = (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER)) : Relay(-1, true, functions), state(false) {
  }

  void onInit() {
  }

  void turnOn(_supla_int_t duration) {
    if (duration > 0) {
      durationMs = duration + millis();
    }
    state = true;

    channel.setNewValue(state);
  }

  virtual void turnOff(_supla_int_t duration) {
    durationMs = 0;
    state = false;

    channel.setNewValue(state);
  }

  virtual bool isOn() {
    return state;
  }

  virtual void toggle() {
    durationMs = 0;
    state = !state;

    channel.setNewValue(isOn());
  }

 protected:
  bool state;
};

};  // namespace Control
};  // namespace Supla

#endif
