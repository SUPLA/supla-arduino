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

#ifndef _events_h
#define _events_h

namespace Supla {

enum Event {
// Supla::Control::Button events
  ON_PRESS,    // Triggered on transition to valueOnPress()
  ON_RELEASE,  // Triggered on transition from valueOnPress()
  ON_CHANGE,   // Triggered on all transitions
  ON_HOLD,     // Triggered when button is hold
  ON_MULTI_1,  // ON_MULTI_x is triggered when multiclick is detected
  ON_MULTI_2,
  ON_MULTI_3,
  ON_MULTI_4,
  ON_MULTI_5,
  ON_MULTI_6,
  ON_MULTI_7,
  ON_MULTI_8,
  ON_MULTI_9
};

};

#endif
