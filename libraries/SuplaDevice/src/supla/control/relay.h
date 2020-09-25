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

#include "../actions.h"
#include "../channel.h"
#include "../element.h"
#include "../io.h"
#include "../storage/storage.h"
#include "../triggerable.h"

namespace Supla {
namespace Control {
class Relay : public Element, public Triggerable {
 public:
  Relay(int pin,
        bool highIsOn = true,
        _supla_int_t functions = (0xFF ^
                                  SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

  virtual uint8_t pinOnValue();
  virtual uint8_t pinOffValue();
  virtual void turnOn(_supla_int_t duration = 0);
  virtual void turnOff(_supla_int_t duration = 0);
  virtual bool isOn();
  virtual void toggle();

  void runAction(int trigger, int action);

  void onInit();
  void iterateAlways();
  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);

 protected:
  Channel *getChannel();
  Channel channel;
  _supla_int_t durationMs;
  int pin;
  bool highIsOn;
};

};  // namespace Control
};  // namespace Supla

#endif
