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

#ifndef _pin_status_led_h
#define _pin_status_led_h

#include "../element.h"

namespace Supla {
namespace Control {
class PinStatusLed : public Element {
 public:
  PinStatusLed(uint8_t srcPin, uint8_t outPin, bool invert = false);

  void onInit();
  void iterateAlways();
  void setInvertedLogic(bool invertedLogic);

 protected:
  void updatePin();

  uint8_t srcPin;
  uint8_t outPin;
  bool invert;
};

};  // namespace Control
};  // namespace Supla

#endif
