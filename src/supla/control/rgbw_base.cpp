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
#include <stdint.h>

#include "rgbw_base.h"

namespace Supla {
namespace Control {

RGBWBase::RGBWBase()
    : buttonStep(10),
      curRed(0),
      curGreen(255),
      curBlue(0),
      curColorBrightness(0),
      curBrightness(0),
      lastColorBrightness(100),
      lastBrightness(100),
      defaultDimmedBrightness(20),
      dimIterationDirection(false),
      iterationDelayCounter(0),
      fadeEffect(1000),
      hwRed(0),
      hwGreen(255),
      hwBlue(0),
      hwColorBrightness(0),
      hwBrightness(0),
      lastTick(0) {
  channel.setType(SUPLA_CHANNELTYPE_DIMMERANDRGBLED);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMERANDRGBLIGHTING);
}


void RGBWBase::setRGBW(
    int red, int green, int blue, int colorBrightness, int brightness) {
  // Store last non 0 brightness for turn on/toggle operations
  if (colorBrightness > 0) {
    lastColorBrightness = colorBrightness;
  }
  if (brightness > 0) {
    lastBrightness = brightness;
  }

  // Store current values
  if (red >= 0) {
    curRed = red;
  }
  if (green >= 0) {
    curGreen = green;
  }
  if (blue >= 0) {
    curBlue = blue;
  }
  if (colorBrightness >= 0) {
    curColorBrightness = colorBrightness;
  }
  if (brightness >= 0) {
    curBrightness = brightness;
  }

  // If fade effect is disabled, then set new values to device directly
  if (fadeEffect <= 0) {
    setRGBWValueOnDevice(
        curRed, curGreen, curBlue, curColorBrightness, curBrightness);
  }

  // Send to Supla server new values
  channel.setNewValue(
      curRed, curGreen, curBlue, curColorBrightness, curBrightness);
}

int RGBWBase::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  setRGBW(red, green, blue, colorBrightness, brightness);

  return -1;
}

void RGBWBase::turnOn() {
  setRGBW(-1, -1, -1, lastColorBrightness, lastBrightness);
}
void RGBWBase::turnOff() {
  setRGBW(-1, -1, -1, 0, 0);
}

void RGBWBase::toggle() {
  setRGBW(-1,
          -1,
          -1,
          curColorBrightness > 0 ? 0 : lastColorBrightness,
          curBrightness > 0 ? 0 : lastBrightness);
}

uint8_t RGBWBase::addWithLimit(int value, int addition, int limit) {
  if (addition > 0 && value + addition > limit) {
    return limit;
  }
  if (addition < 0 && value + addition < 0) {
    return 0;
  }
  return value + addition;
}

void RGBWBase::runAction(int event, int action) {
  (void)(event);
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
    case BRIGHTEN_ALL: {
      setRGBW(-1,
              -1,
              -1,
              addWithLimit(curColorBrightness, buttonStep, 100),
              addWithLimit(curBrightness, buttonStep, 100));
      break;
    }
    case DIM_ALL: {
      setRGBW(-1,
              -1,
              -1,
              addWithLimit(curColorBrightness, -buttonStep, 100),
              addWithLimit(curBrightness, -buttonStep, 100));
      break;
    }
    case BRIGHTEN_R: {
      setRGBW(addWithLimit(curRed, buttonStep), -1, -1, -1, -1);
      break;
    }
    case DIM_R: {
      setRGBW(addWithLimit(curRed, -buttonStep), -1, -1, -1, -1);
      break;
    }
    case BRIGHTEN_G: {
      setRGBW(-1, addWithLimit(curGreen, buttonStep), -1, -1, -1);
      break;
    }
    case DIM_G: {
      setRGBW(-1, addWithLimit(curGreen, -buttonStep), -1, -1, -1);
      break;
    }
    case BRIGHTEN_B: {
      setRGBW(-1, -1, addWithLimit(curBlue, buttonStep), -1, -1);
      break;
    }
    case DIM_B: {
      setRGBW(-1, -1, addWithLimit(curBlue, -buttonStep), -1, -1);
      break;
    }
    case BRIGHTEN_W: {
      setRGBW(-1, -1, -1, -1, addWithLimit(curBrightness, buttonStep, 100));
      break;
    }
    case DIM_W: {
      setRGBW(-1, -1, -1, -1, addWithLimit(curBrightness, -buttonStep, 100));
      break;
    }
    case BRIGHTEN_RGB: {
      setRGBW(
          -1, -1, -1, addWithLimit(curColorBrightness, buttonStep, 100), -1);
      break;
    }
    case DIM_RGB: {
      setRGBW(
          -1, -1, -1, addWithLimit(curColorBrightness, -buttonStep, 100), -1);
      break;
    }
    case TURN_ON_RGB: {
      setRGBW(-1, -1, -1, lastColorBrightness, -1);
      break;
    }
    case TURN_OFF_RGB: {
      setRGBW(-1, -1, -1, 0, -1);
      break;
    }
    case TOGGLE_RGB: {
      setRGBW(-1, -1, -1, curColorBrightness > 0 ? 0 : lastColorBrightness, -1);
      break;
    }
    case TURN_ON_W: {
      setRGBW(-1, -1, -1, -1, lastBrightness);
      break;
    }
    case TURN_OFF_W: {
      setRGBW(-1, -1, -1, -1, 0);
      break;
    }
    case TOGGLE_W: {
      setRGBW(-1, -1, -1, -1, curBrightness > 0 ? 0 : lastBrightness);
      break;
    }
    case TURN_ON_RGB_DIMMED: {
      if (curColorBrightness == 0) {
        setRGBW(-1, -1, -1, defaultDimmedBrightness, -1);
      }
      break;
    }
    case TURN_ON_W_DIMMED: {
      if (curBrightness == 0) {
        setRGBW(-1, -1, -1, -1, defaultDimmedBrightness);
      }
      break;
    }
    case TURN_ON_ALL_DIMMED: {
      if (curBrightness == 0 && curColorBrightness == 0) {
        setRGBW(-1, -1, -1, defaultDimmedBrightness, defaultDimmedBrightness);
      }
      break;
    }
    case ITERATE_DIM_RGB: {
      iterateDimmerRGBW(buttonStep, 0);
      break;
    }
    case ITERATE_DIM_W: {
      iterateDimmerRGBW(0, buttonStep);
      break;
    }
    case ITERATE_DIM_ALL: {
      iterateDimmerRGBW(buttonStep, buttonStep);
      break;
    }
  }
}

void RGBWBase::iterateDimmerRGBW(int rgbStep, int wStep) {
  // if we iterate both RGB and W, then we should sync brightness
  if (rgbStep > 0 && wStep > 0) {
    curBrightness = curColorBrightness;
  }
  if (rgbStep > 0) {
    if (curColorBrightness <= 10 && dimIterationDirection == true) {
      iterationDelayCounter++;
      if (iterationDelayCounter == 5) {
        dimIterationDirection = false;
        iterationDelayCounter = 0;
      } else {
        return;
      }
    } else if (curColorBrightness == 100 && dimIterationDirection == false) {
      iterationDelayCounter++;
      if (iterationDelayCounter == 5) {
        dimIterationDirection = true;
        iterationDelayCounter = 0;
      } else {
        return;
      }
    }
  } else if (wStep > 0) {
    if (curBrightness <= 10 && dimIterationDirection == true) {
      iterationDelayCounter++;
      if (iterationDelayCounter == 5) {
        dimIterationDirection = false;
        iterationDelayCounter = 0;
      } else {
        return;
      }
    } else if (curBrightness == 100 && dimIterationDirection == false) {
      iterationDelayCounter++;
      if (iterationDelayCounter == 5) {
        dimIterationDirection = true;
        iterationDelayCounter = 0;
      } else {
        return;
      }
    }
  }
  iterationDelayCounter = 0;

  // If direction is dim, then brightness step is set to negative
  if (dimIterationDirection) {
    rgbStep = -rgbStep;
    wStep = -wStep;
  }

  setRGBW(-1,
      -1,
      -1,
      addWithLimit(curColorBrightness, rgbStep, 100),
      addWithLimit(curBrightness, wStep, 100));
}

Channel *RGBWBase::getChannel() {
  return &channel;
}

void RGBWBase::setStep(int step) {
  buttonStep = step;
}

void RGBWBase::setDefaultDimmedBrightness(int dimmedBrightness) {
  defaultDimmedBrightness = dimmedBrightness;
}

void RGBWBase::setFadeEffectTime(int timeMs) {
  fadeEffect = timeMs;
}

void RGBWBase::onTimer() {
  // exit it fade effect is disabled
  if (fadeEffect <= 0) {
    return;
  }
  unsigned long timeDiff = millis() - lastTick;
  lastTick = millis();

  if (timeDiff > 0) {
    int divider = fadeEffect / timeDiff;
    if (divider <= 0) {
      divider = 1;
    }

    uint8_t rgbStep = 255 / divider;
    uint8_t brightnessStep = 100 / divider;
    bool valueChanged = false;
    if (rgbStep < 1) {
      rgbStep = 1;
    }
    if (brightnessStep < 1) {
      brightnessStep = 1;
    }

    if (curRed > hwRed) {
      valueChanged = true;
      hwRed += rgbStep;
      if (hwRed > curRed) {
        hwRed = curRed;
      }
    } else if (curRed < hwRed) {
      valueChanged = true;
      hwRed -= rgbStep;
      if (hwRed < curRed) {
        hwRed = curRed;
      }
    }

    if (curGreen > hwGreen) {
      valueChanged = true;
      hwGreen += rgbStep;
      if (hwGreen > curGreen) {
        hwGreen = curGreen;
      }
    } else if (curGreen < hwGreen) {
      valueChanged = true;
      hwGreen -= rgbStep;
      if (hwGreen < curGreen) {
        hwGreen = curGreen;
      }
    }

    if (curBlue > hwBlue) {
      valueChanged = true;
      hwBlue += rgbStep;
      if (hwBlue > curBlue) {
        hwBlue = curBlue;
      }
    } else if (curBlue < hwBlue) {
      valueChanged = true;
      hwBlue -= rgbStep;
      if (hwBlue < curBlue) {
        hwBlue = curBlue;
      }
    }

    if (curColorBrightness > hwColorBrightness) {
      valueChanged = true;
      hwColorBrightness += brightnessStep;
      if (hwColorBrightness > curColorBrightness) {
        hwColorBrightness = curColorBrightness;
      }
    } else if (curColorBrightness < hwColorBrightness) {
      valueChanged = true;
      hwColorBrightness -= brightnessStep;
      if (hwColorBrightness < curColorBrightness) {
        hwColorBrightness = curColorBrightness;
      }
    }

    if (curBrightness > hwBrightness) {
      valueChanged = true;
      hwBrightness += brightnessStep;
      if (hwBrightness > curBrightness) {
        hwBrightness = curBrightness;
      }
    } else if (curBrightness < hwBrightness) {
      valueChanged = true;
      hwBrightness -= brightnessStep;
      if (hwBrightness < curBrightness) {
        hwBrightness = curBrightness;
      }
    }

    if (valueChanged) {
      setRGBWValueOnDevice(hwRed, hwGreen, hwBlue, hwColorBrightness, hwBrightness);
    }
  }
}

};  // namespace Control
};  // namespace Supla
