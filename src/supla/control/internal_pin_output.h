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

#ifndef _internal_pin_output_h
#define _internal_pin_output_h

#include "../action_handler.h"
#include "../actions.h"
#include "../element.h"
#include "../io.h"
#include "../local_action.h"

#define STATE_ON_INIT_OFF 0
#define STATE_ON_INIT_ON  1

namespace Supla {
namespace Control {
class InternalPinOutput : public Element,
                          public ActionHandler,
                          public LocalAction {
 public:
  InternalPinOutput(int pin, bool highIsOn = true);

  virtual InternalPinOutput &setDefaultStateOn();
  virtual InternalPinOutput &setDefaultStateOff();
  virtual InternalPinOutput &setDurationMs(_supla_int_t duration);

  virtual uint8_t pinOnValue();
  virtual uint8_t pinOffValue();
  virtual void turnOn(_supla_int_t duration = 0);
  virtual void turnOff(_supla_int_t duration = 0);
  virtual bool isOn();
  virtual void toggle(_supla_int_t duration = 0);

  void handleAction(int event, int action);

  void onInit();
  void iterateAlways();

 protected:
  int pin;
  bool highIsOn;

  int8_t stateOnInit;

  unsigned _supla_int_t durationMs;
  unsigned _supla_int_t storedTurnOnDurationMs;
  unsigned long durationTimestamp;
};

};  // namespace Control
};  // namespace Supla

#endif
