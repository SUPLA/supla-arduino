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

#ifndef _button_h
#define _button_h

#include <Arduino.h>

#include "../element.h"
#include "../events.h"
#include "../will_trigger.h"

namespace Supla {
namespace Control {
class Button : public Element, public WillTrigger {
 public:
  Button(int pin, bool pullUp = false, bool invertLogic = false)
      : pin(pin),
        pullUp(pullUp),
        prevStatus(LOW),
        newStatusCandidate(LOW),
        debounceTimeMs(0),
        filterTimeMs(0),
        debounceDelayMs(50),
        swNoiseFilterDelayMs(20),
        invertLogic(invertLogic) {
  }

  void iterateAlways() {
    // Ignore anything that happen within debounceDelayMs ms since last state
    // change
    if (millis() - debounceTimeMs > debounceDelayMs) {
      int currentStatus = digitalRead(pin);
      if (currentStatus != prevStatus) {
        // If status is changed, then make sure that it will be kept at
        // least swNoiseFilterDelayMs ms to avoid noise
        if (currentStatus != newStatusCandidate) {
          newStatusCandidate = currentStatus;
          filterTimeMs = millis();
          return;
        }
        // If new status is kept at least swNoiseFilterDelayMs ms, then apply
        // change of status
        if (millis() - filterTimeMs > swNoiseFilterDelayMs) {
          debounceTimeMs = millis();
          prevStatus = currentStatus;
          if (currentStatus == valueOnPress()) {
            runTrigger(ON_PRESS);
            runTrigger(ON_CHANGE);
          } else {
            runTrigger(ON_RELEASE);
            runTrigger(ON_CHANGE);
          }
        }
      } else {
        // If current status is the same as prevStatus, then reset
        // new status candidate
        newStatusCandidate = prevStatus;
      }
    }
  }

  void onInit() {
    pinMode(pin, pullUp ? INPUT_PULLUP : INPUT);
    prevStatus = digitalRead(pin);
    newStatusCandidate = prevStatus;
  }

  int valueOnPress() {
    return invertLogic ? LOW : HIGH;
  }

  void setSwNoiseFilterDelay(int newDelayMs) {
    swNoiseFilterDelayMs = newDelayMs;
  }

  void setDebounceDelay(int newDelayMs) {
    debounceDelayMs = newDelayMs;
  }

 protected:
  int pin;
  bool pullUp;
  unsigned long debounceTimeMs;
  unsigned long filterTimeMs;
  int debounceDelayMs;
  int swNoiseFilterDelayMs;
  int prevStatus;
  int newStatusCandidate;
  bool invertLogic;
};

};  // namespace Control
};  // namespace Supla

#endif
