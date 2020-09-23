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
#include <crc16.h>
#include <supla-common/log.h>
#include <supla/storage/storage.h>
#include <supla/actions.h>

#include "impulse_counter.h"

using namespace Supla::Sensor;

ImpulseCounter::ImpulseCounter(int _impulsePin,
                               bool _detectLowToHigh,
                               bool _inputPullup,
                               unsigned int _debounceDelay)
    : impulsePin(_impulsePin),
      debounceDelay(_debounceDelay),
      detectLowToHigh(_detectLowToHigh),
      lastImpulseMillis(0),
      counter(0),
      inputPullup(_inputPullup) {
  channel.setType(SUPLA_CHANNELTYPE_IMPULSE_COUNTER);

  prevState = (detectLowToHigh == true ? LOW : HIGH);

  supla_log(LOG_DEBUG,
            "Creating Impulse Counter: impulsePin(%d), "
            "delay(%d ms)",
            impulsePin,
            debounceDelay);
  if (impulsePin <= 0) {
    supla_log(LOG_DEBUG,
              "SuplaImpulseCounter ERROR - incorrect impulse pin number");
    return;
  }
}

void ImpulseCounter::onInit() {
  if (inputPullup) {
    pinMode(impulsePin, INPUT_PULLUP);
  } else {
    pinMode(impulsePin, INPUT);
  }
}

_supla_int64_t ImpulseCounter::getCounter() {
  return counter;
}

void ImpulseCounter::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&counter, sizeof(counter));
}

void ImpulseCounter::onLoadState() {
  _supla_int64_t data;
  if (Supla::Storage::ReadState((unsigned char *)&data, sizeof(data))) {
    setCounter(data);
  }
}

void ImpulseCounter::setCounter(_supla_int64_t value) {
  counter = value;
  channel.setNewValue(value);
  supla_log(LOG_DEBUG,
            "ImpulseCounter[%d] - set counter to %d",
            channel.getChannelNumber(),
            static_cast<int>(counter));
}

void ImpulseCounter::incCounter() {
  counter++;
  channel.setNewValue(getCounter());
}

void ImpulseCounter::onFastTimer() {
  int currentState = digitalRead(impulsePin);
  if (prevState == (detectLowToHigh == true ? LOW : HIGH)) {
    if (millis() - lastImpulseMillis > debounceDelay) {
      if (currentState == (detectLowToHigh == true ? HIGH : LOW)) {
        incCounter();
        lastImpulseMillis = millis();
      }
    }
  }
  prevState = currentState;
}

Supla::Channel *ImpulseCounter::getChannel() {
  return &channel;
}

void ImpulseCounter::runAction(int trigger, int action) {
  switch (action) {
    case RESET: {
      setCounter(0);
      break;
    }
  }
}
