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

#include "../channel.h"
#include "../element.h"
#include "../triggerable.h"
#include "../actions.h"

namespace Supla {
namespace Control {
class RGBWBase : public Element, public Triggerable {
 public:
  RGBWBase();

  virtual void setRGBWValueOnDevice(uint8_t red,
                                    uint8_t green,
                                    uint8_t blue,
                                    uint8_t colorBrightness,
                                    uint8_t brightness) = 0;

  virtual void setRGBW(
      int red, int green, int blue, int colorBrightness, int brightness);

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);
  virtual void turnOn();
  virtual void turnOff();
  virtual void toggle();
  void trigger(int trigger, int action);
  void setStep(int step);
  void setDefaultDimmedBrightness(int dimmedBrightness);
  void setFadeEffectTime(int timeMs);
  void onTimer();

void onInit() {

  // Send to Supla server new values
  channel.setNewValue(
      curRed, curGreen, curBlue, curColorBrightness, curBrightness);
}
 protected:
  uint8_t addWithLimit(int value, int addition, int limit = 255);
  Channel *getChannel();
  void iterateDimmerRGBW(int rgbStep, int wStep);

  Channel channel;
  uint8_t buttonStep;               // 10
  uint8_t curRed;                   // 0 - 255
  uint8_t curGreen;                 // 0 - 255
  uint8_t curBlue;                  // 0 - 255
  uint8_t curColorBrightness;       // 0 - 100
  uint8_t curBrightness;            // 0 - 100
  uint8_t lastColorBrightness;      // 0 - 100
  uint8_t lastBrightness;           // 0 - 100
  uint8_t defaultDimmedBrightness;  // 20
  bool dimIterationDirection;
  int iterationDelayCounter;
  int fadeEffect;
  int hwRed;                   // 0 - 255
  int hwGreen;                 // 0 - 255
  int hwBlue;                  // 0 - 255
  int hwColorBrightness;       // 0 - 100
  int hwBrightness;            // 0 - 100
  unsigned long lastTick;

};

};  // namespace Control
};  // namespace Supla

#endif
