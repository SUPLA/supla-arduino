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

#include "button.h"

enum StateResults {PRESSED, RELEASED, TO_PRESSED, TO_RELEASED};

Supla::Control::ButtonState::ButtonState(int pin, bool pullUp, bool invertLogic)
    : pin(pin),
      pullUp(pullUp),
      invertLogic(invertLogic),
      newStatusCandidate(LOW),
      debounceTimeMs(0),
      filterTimeMs(0),
      debounceDelayMs(50),
      swNoiseFilterDelayMs(20) {
}

int Supla::Control::ButtonState::update() {
  if (millis() - debounceTimeMs > debounceDelayMs) {
    int currentState = digitalRead(pin);
    if (currentState != prevState) {
      // If status is changed, then make sure that it will be kept at
      // least swNoiseFilterDelayMs ms to avoid noise
      if (currentState != newStatusCandidate) {
        newStatusCandidate = currentState;
        filterTimeMs = millis();
      } else if (millis() - filterTimeMs > swNoiseFilterDelayMs) {
      // If new status is kept at least swNoiseFilterDelayMs ms, then apply
      // change of status
        debounceTimeMs = millis();
        prevState = currentState;
        if (currentState == valueOnPress()) {
          return TO_PRESSED;
        } else {
          return TO_RELEASED;
        }
      }
    } else {
      // If current status is the same as prevState, then reset
      // new status candidate
      newStatusCandidate = prevState;
    }
  }
  if (prevState == valueOnPress()) {
    return PRESSED;
  } else {
    return RELEASED;
  }
}

Supla::Control::Button::Button(int pin, bool pullUp, bool invertLogic)
    : state(pin, pullUp, invertLogic),
      holdTimeMs(0),
      multiclickTimeMs(0),
      enableExtDetection(false),
      lastStateChangeMs(0),
      holdSend(false),
      clickCounter(0),
      bistable(false) {
}

void Supla::Control::Button::onTimer() {
  unsigned int timeDelta = millis() - lastStateChangeMs;
  bool stateChanged = false;
  int stateResult = state.update();
  if (stateResult == TO_PRESSED) {
    stateChanged = true;
    runAction(ON_PRESS);
    runAction(ON_CHANGE);
  } else if (stateResult == TO_RELEASED) {
    stateChanged = true;
    runAction(ON_RELEASE);
    runAction(ON_CHANGE);
  }

  if (!stateChanged) {
    if (!bistable && stateResult == PRESSED) {
      if (clickCounter <= 1 && holdTimeMs > 0 && timeDelta > holdTimeMs && !holdSend) {
        runAction(ON_HOLD);
        holdSend = true;
      }
    } else if (bistable || stateResult == RELEASED) {
      if (multiclickTimeMs > 0 && timeDelta > multiclickTimeMs) {
        if (!holdSend) {
          switch (clickCounter) {
            case 1:
              runAction(ON_CLICK_1);
              break;
            case 2:
              runAction(ON_CLICK_2);
              break;
            case 3:
              runAction(ON_CLICK_3);
              break;
            case 4:
              runAction(ON_CLICK_4);
              break;
            case 5:
              runAction(ON_CLICK_5);
              break;
            case 6:
              runAction(ON_CLICK_6);
              break;
            case 7:
              runAction(ON_CLICK_7);
              break;
            case 8:
              runAction(ON_CLICK_8);
              break;
            case 9:
              runAction(ON_CLICK_9);
              break;
          }
        }
        holdSend = false;
        clickCounter = 0;
      }
    }
  }

  if (stateChanged) {
    lastStateChangeMs = millis();
    if (stateResult == TO_PRESSED || bistable) {
      clickCounter++;
    }

  }
}

void Supla::Control::Button::onInit() {
  state.init();
}

void Supla::Control::ButtonState::init() {
  pinMode(pin, pullUp ? INPUT_PULLUP : INPUT);
  prevState = digitalRead(pin);
  newStatusCandidate = prevState;
}

int Supla::Control::ButtonState::valueOnPress() {
  return invertLogic ? LOW : HIGH;
}

void Supla::Control::Button::setSwNoiseFilterDelay(unsigned int newDelayMs) {
  state.setSwNoiseFilterDelay(newDelayMs);
}
void Supla::Control::ButtonState::setSwNoiseFilterDelay(unsigned int newDelayMs) {
  swNoiseFilterDelayMs = newDelayMs;
}

void Supla::Control::Button::setDebounceDelay(unsigned int newDelayMs) {
  state.setDebounceDelay(newDelayMs);
}

void Supla::Control::ButtonState::setDebounceDelay(unsigned int newDelayMs) {
  debounceDelayMs = newDelayMs;
}

void Supla::Control::Button::setHoldTime(unsigned int timeMs) {
  holdTimeMs = timeMs;
  if (bistable) {
    holdTimeMs = 0;
  }
}

void Supla::Control::Button::setMulticlickTime(unsigned int timeMs, bool bistableButton) {
  multiclickTimeMs = timeMs;
  bistable = bistableButton;
  if (bistable) {
    holdTimeMs = 0;
  }
}
