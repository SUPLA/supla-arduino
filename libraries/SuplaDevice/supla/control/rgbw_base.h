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

#ifndef _rgbw_base_h
#define _rgbw_base_h

#include <Arduino.h>
#include <stdint.h>

#include "../../io.h"
#include "../channel.h"
#include "../element.h"
#include "../triggerable.h"

namespace Supla {
namespace Control {
class RGBWBase : public Element, public Triggerable {
 public:
  enum Action {
    TURN_ON,
    TURN_OFF,
    TOGGLE,
    BRIGHTEN_ALL,
    DIM_ALL,
    BRIGHTEN_R,
    BRIGHTEN_G,
    BRIGHTEN_B,
    BRIGHTEN_W,
    BRIGHTEN_RGB,
    DIM_R,
    DIM_G,
    DIM_B,
    DIM_W,
    DIM_RGB,
    TURN_ON_RGB,
    TURN_OFF_RGB,
    TOGGLE_RGB,
    TURN_ON_W,
    TURN_OFF_W,
    TOGGLE_W,
    TURN_ON_RGB_DIMMED,
    TURN_ON_W_DIMMED,
    TURN_ON_ALL_DIMMED
  };

  RGBWBase()
      : buttonStep(10),
        lastColorBrightness(255),
        lastBrightness(255),
        curRed(0),
        curGreen(255),
        curBlue(0),
        curColorBrightness(0),
        curBrightness(0),
        defaultDimmedBrightness(20) {
    channel.setType(SUPLA_CHANNELTYPE_DIMMERANDRGBLED);
    channel.setDefault(SUPLA_CHANNELFNC_DIMMERANDRGBLIGHTING);
  }

  virtual void setRGBWValueOnDevice(uint8_t red,
                                    uint8_t green,
                                    uint8_t blue,
                                    uint8_t colorBrightness,
                                    uint8_t brightness) = 0;

  virtual void setRGBW(uint8_t red,
                       uint8_t green,
                       uint8_t blue,
                       uint8_t colorBrightness,
                       uint8_t brightness) {
    // Store last non 0 brightness for turn on/toggle operations
    if (colorBrightness > 0) {
      lastColorBrightness = colorBrightness;
    }
    if (brightness > 0) {
      lastBrightness = brightness;
    }

    // Store current values
    curRed = red;
    curGreen = green;
    curBlue = blue;
    curColorBrightness = colorBrightness;
    curBrightness = brightness;

    // Set new values to device
    setRGBWValueOnDevice(red, green, blue, colorBrightness, brightness);

    // Send to Supla server new values
    channel.setNewValue(red, green, blue, colorBrightness, brightness);
  }

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
    uint8_t red = static_cast<uint8_t>(newValue->value[4]);
    uint8_t green = static_cast<uint8_t>(newValue->value[3]);
    uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
    uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
    uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

    setRGBW(red, green, blue, colorBrightness, brightness);

    return -1;
  }

  virtual void turnOn() {
    setRGBW(curRed, curGreen, curBlue, lastColorBrightness, lastBrightness);
  }
  virtual void turnOff() {
    setRGBW(curRed, curGreen, curBlue, 0, 0);
  }

  virtual void toggle() {
    setRGBW(curRed,
            curGreen,
            curBlue,
            curColorBrightness > 0 ? 0 : lastColorBrightness,
            curBrightness > 0 ? 0 : lastBrightness);
  }

  uint8_t addWithLimit(int value, int addition, int limit = 255) {
    if (addition > 0 && value + addition > limit) {
      return limit;
    }
    if (addition < 0 && value + addition < 0) {
      return 0;
    }
    return value + addition;
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
      case BRIGHTEN_ALL: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                addWithLimit(curColorBrightness, buttonStep, 100),
                addWithLimit(curBrightness, buttonStep, 100));
        break;
      }
      case DIM_ALL: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                addWithLimit(curColorBrightness, -buttonStep, 100),
                addWithLimit(curBrightness, -buttonStep, 100));
        break;
      }
      case BRIGHTEN_R: {
        setRGBW(addWithLimit(curRed, buttonStep),
                curGreen,
                curBlue,
                curColorBrightness,
                curBrightness);
        break;
      }
      case DIM_R: {
        setRGBW(addWithLimit(curRed, -buttonStep),
                curGreen,
                curBlue,
                curColorBrightness,
                curBrightness);
        break;
      }
      case BRIGHTEN_G: {
        setRGBW(curRed,
                addWithLimit(curGreen, buttonStep),
                curBlue,
                curColorBrightness,
                curBrightness);
        break;
      }
      case DIM_G: {
        setRGBW(curRed,
                addWithLimit(curGreen, -buttonStep),
                curBlue,
                curColorBrightness,
                curBrightness);
        break;
      }
      case BRIGHTEN_B: {
        setRGBW(curRed,
                curGreen,
                addWithLimit(curBlue, buttonStep),
                curColorBrightness,
                curBrightness);
        break;
      }
      case DIM_B: {
        setRGBW(curRed,
                curGreen,
                addWithLimit(curBlue, -buttonStep),
                curColorBrightness,
                curBrightness);
        break;
      }
      case BRIGHTEN_W: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                curColorBrightness,
                addWithLimit(curBrightness, buttonStep, 100));
        break;
      }
      case DIM_W: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                curColorBrightness,
                addWithLimit(curBrightness, -buttonStep, 100));
        break;
      }
      case BRIGHTEN_RGB: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                addWithLimit(curColorBrightness, buttonStep, 100),
                curBrightness);
        break;
      }
      case DIM_RGB: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                addWithLimit(curColorBrightness, -buttonStep, 100),
                curBrightness);
        break;
      }
      case TURN_ON_RGB: {
        setRGBW(curRed, curGreen, curBlue, lastColorBrightness, curBrightness);
        break;
      }
      case TURN_OFF_RGB: {
        setRGBW(curRed, curGreen, curBlue, 0, curBrightness);
        break;
      }
      case TOGGLE_RGB: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                curColorBrightness > 0 ? 0 : lastColorBrightness,
                curBrightness);
        break;
      }
      case TURN_ON_W: {
        setRGBW(curRed, curGreen, curBlue, curColorBrightness, lastBrightness);
        break;
      }
      case TURN_OFF_W: {
        setRGBW(curRed, curGreen, curBlue, curColorBrightness, 0);
        break;
      }
      case TOGGLE_W: {
        setRGBW(curRed,
                curGreen,
                curBlue,
                curColorBrightness,
                curBrightness > 0 ? 0 : lastColorBrightness);
        break;
      }
      case TURN_ON_RGB_DIMMED: {
        if (curColorBrightness == 0) {
          setRGBW(curRed,
                  curGreen,
                  curBlue,
                  defaultDimmedBrightness,
                  curBrightness);
        }
        break;                         
      }
      case TURN_ON_W_DIMMED: {
        if (curBrightness == 0) {
          setRGBW(curRed,
                  curGreen,
                  curBlue,
                  curColorBrightness,
                  defaultDimmedBrightness);
        }
        break;                         
      }
      case TURN_ON_ALL_DIMMED: {
        if (curBrightness == 0 && curColorBrightness == 0) {
          setRGBW(curRed,
                  curGreen,
                  curBlue,
                  defaultDimmedBrightness,
                  defaultDimmedBrightness);
        }
        break;                         
      }
    }
  }

 protected:
  Channel *getChannel() {
    return &channel;
  }
  Channel channel;
  uint8_t buttonStep; // 10
  uint8_t curRed;    // 0 - 255
  uint8_t curGreen;  // 0 - 255
  uint8_t curBlue;   // 0 - 255
  uint8_t curColorBrightness;  // 0 - 100
  uint8_t curBrightness;       // 0 - 100
  uint8_t lastColorBrightness; // 0 - 100
  uint8_t lastBrightness;      // 0 - 100
  uint8_t defaultDimmedBrightness; // 20
};

};  // namespace Control
};  // namespace Supla

#endif
