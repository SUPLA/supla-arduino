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

/* Relay class
 * This class is used to control any type of relay that can be controlled
 * by setting LOW or HIGH output on selected GPIO.
 */

#ifndef _relay_h
#define _relay_h

#include <Arduino.h>

#include "../io.h"
#include "../channel.h"
#include "../element.h"
#include "../triggerable.h"
#include "../actions.h"

namespace Supla {
namespace Control {
class Relay : public Element, public Triggerable {
 public:
  Relay(int pin,
        bool highIsOn = true,
        _supla_int_t functions = (0xFF ^
                                  SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER))
      : pin(pin), durationMs(0), highIsOn(highIsOn) {
    channel.setType(SUPLA_CHANNELTYPE_RELAY);
    channel.setFuncList(functions);
  }

  virtual uint8_t pinOnValue() {
    return highIsOn ? HIGH : LOW;
  }

  virtual uint8_t pinOffValue() {
    return highIsOn ? LOW : HIGH;
  }

  void onInit() {
    pinMode(pin, OUTPUT);
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());
  }

  void iterateAlways() {
    if (durationMs && durationMs < millis()) {
      durationMs = 0;
      toggle();
    }
  }

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
    int result = -1;
    if (newValue->value[0] == 1) {
      turnOn(newValue->DurationMS);
      result = 1;
    } else if (newValue->value[0] == 0) {
      turnOff(newValue->DurationMS);
      result = 1;
    }

    return result;
  }

  virtual void turnOn(_supla_int_t duration = 0) {
    if (duration > 0) {
      durationMs = duration + millis();
    }
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOnValue());

    channel.setNewValue(true);
  }

  virtual void turnOff(_supla_int_t duration = 0) {
    durationMs = 0;
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());

    channel.setNewValue(false);
  }

  virtual bool isOn() {
    return Supla::Io::digitalRead(channel.getChannelNumber(), pin) ==
           pinOnValue();
  }

  virtual void toggle() {
    durationMs = 0;
    Supla::Io::digitalWrite(
        channel.getChannelNumber(),
        pin,
        Supla::Io::digitalRead(channel.getChannelNumber(), pin) == LOW ? HIGH
                                                                       : LOW);

    if (isOn()) {
      channel.setNewValue(true);
    } else {
      channel.setNewValue(false);
    }
  }

  void trigger(int trigger, int action) {
    switch (action) {
      case TURN_ON: {
        turnOn();
        break;
      }
      case TURN_OFF: {
        turnOff();
        break;
      }
      case TOGGLE: {
        toggle();
        break;
      }
    }
  }

 protected:
  Channel *getChannel() {
    return &channel;
  }
  Channel channel;
  _supla_int_t durationMs;
  int pin;
  bool highIsOn;
};

};  // namespace Control
};  // namespace Supla

#endif
