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

#ifndef _bistable_roller_shutter_h
#define _bistable_roller_shutter_h

#include "roller_shutter.h"

namespace Supla {
namespace Control {
class BistableRollerShutter : public RollerShutter {
 public:
  BistableRollerShutter(int pinUp, int pinDown, bool highIsOn = true);

  void onTimer();

 protected:
  void stopMovement();
  void relayDownOn();
  void relayUpOn();
  void relayUpOff();
  void relayDownOff();

  bool activeBiRelay;
  unsigned long toggleTime;
};

};  // namespace Control
};  // namespace Supla

#endif


