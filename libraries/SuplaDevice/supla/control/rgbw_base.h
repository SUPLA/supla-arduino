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

#include "../../io.h"
#include "../channel.h"
#include "../element.h"
#include "../triggerable.h"

namespace Supla {
namespace Control {
class RGBWBase : public Element, public Triggerable {
 public:
  enum Action { TURN_ON, TURN_OFF, TOGGLE };

  virtual void setRGBWValueOnDevice(uint8_t red,
                               uint8_t green,
                               uint8_t blue,
                               int8_t colorBrightness,
                               int8_t brightness) = 0;

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
    uint8_t red = static_cast<uint8_t>(newValue->value[4]);
    uint8_t green = static_cast<uint8_t>(newValue->value[3]);
    uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
    int8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
    char brightness = static_cast<uint8_t>(newValue->value[0]);

    setRGBWValueOnDevice(red, green, blue, colorBrightness, brightness);
    
    channel.setNewValue(newValue->value);

    return -1;
  }

  virtual void turnOn(_supla_int_t duration = 0) = 0;

  virtual void turnOff(_supla_int_t duration = 0) = 0;

  virtual bool isOn() = 0;

  virtual void toggle() = 0; 

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
};

};  // namespace Control
};  // namespace Supla

#endif
