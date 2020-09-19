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

#include <Arduino.h>

#include "../io.h"
#include "../actions.h"
#include "../storage/storage.h"
#include "relay.h"

using namespace Supla;
using namespace Control;

Relay::Relay(int pin, bool highIsOn, _supla_int_t functions)
      : pin(pin), durationMs(0), highIsOn(highIsOn) {
    channel.setType(SUPLA_CHANNELTYPE_RELAY);
    channel.setFuncList(functions);
  }

uint8_t Relay::pinOnValue() {
    return highIsOn ? HIGH : LOW;
  }

uint8_t Relay::pinOffValue() {
    return highIsOn ? LOW : HIGH;
  }

void Relay::onInit() {
    pinMode(pin, OUTPUT);
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());
  }

void Relay::iterateAlways() {
    if (durationMs && durationMs < millis()) {
      durationMs = 0;
      toggle();
    }
  }

  int Relay::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
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

void Relay::turnOn(_supla_int_t duration) {
    if (duration > 0) {
      durationMs = duration + millis();
    }
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOnValue());

    channel.setNewValue(true);
  }

void Relay::turnOff(_supla_int_t duration) {
    durationMs = 0;
    Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());

    channel.setNewValue(false);
  }

bool Relay::isOn() {
    return Supla::Io::digitalRead(channel.getChannelNumber(), pin) ==
           pinOnValue();
  }

void Relay::toggle() {
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

void Relay::runAction(int trigger, int action) {
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

Channel *Relay::getChannel() {
    return &channel;
  }

