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
                    (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER))
      : Relay(pin, highIsOn, functions),
        statusPin(statusPin),
        statusPullUp(statusPullUp),
        statusHighIsOn(statusHighIsOn),
        disarmTimeMs(0),
        busy(false) {
  }

  void onInit() {
    Relay::onInit();
    if (statusPin >= 0) {
      pinMode(statusPin, statusPullUp ? INPUT_PULLUP : INPUT);
      channel.setNewValue(isOn());
    } else {
      channel.setNewValue(false);
    }
  }

  void iterateAlways() {
    Relay::iterateAlways();

    if (statusPin >= 0 && (lastReadTime + 100 < millis())) {
      lastReadTime = millis();
      channel.setNewValue(isOn());
    }

    if (disarmTimeMs < millis()) {
      busy = false;
      Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());
    }
  }

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
    // ignore new requests if we are in the middle of state change
    if (busy) {
      return 0;
    } else {
      return Relay::handleNewValueFromServer(newValue);
    }
  }

  virtual void turnOn(_supla_int_t duration) {
    if (busy) {
      return;
    }

    // Change turn on requests duration to be at least 1 s
    if (duration > 0 && duration < 1000) {
      duration = 1000;
    }
    if (duration > 0) {
      durationMs = duration + millis();
    }

    if (isStatusUnknown()) {
      internalToggle();
    } else if (!isOn()) {
      internalToggle();
    }
  }

  virtual void turnOff(_supla_int_t duration) {
    if (busy) {
      return;
    }

    durationMs = 0;

    if (isStatusUnknown()) {
      internalToggle();
    } else if (isOn()) {
      internalToggle();
    }
  }

  virtual bool isOn() {
    if (isStatusUnknown()) {
      return false;
    }
    return Supla::Io::digitalRead(channel.getChannelNumber(), statusPin) == (statusHighIsOn ? HIGH : LOW);
  }

  bool isStatusUnknown() {
    return (statusPin < 0);
  }

  virtual void toggle() {
    if (busy) {
      return;
    }
    durationMs = 0;

    internalToggle();
  }

 protected:
  void internalToggle() {
    busy = true;
    disarmTimeMs = millis() + 200;
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOnValue());
  }

  int statusPin;
  bool statusPullUp;
  bool statusHighIsOn;
  unsigned long disarmTimeMs;
  bool busy;
};

};  // namespace Control
};  // namespace Supla

#endif
