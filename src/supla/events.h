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
  ON_CLICK_1,  // ON_MULTI_x is triggered when multiclick is detected
  ON_CLICK_2,
  ON_CLICK_3,
  ON_CLICK_4,
  ON_CLICK_5,
  ON_CLICK_6,
  ON_CLICK_7,
  ON_CLICK_8,
  ON_CLICK_9,
  ON_CLICK_10,
  ON_CRAZY_CLICKER,  // triggered on >= 10 clicks 
  ON_SEQUENCE_MATCH,        // triggered by SequenceButton 
  ON_SEQUENCE_DOESNT_MATCH,  // triggered by SequenceButton
  ON_TURN_ON,
  ON_TURN_OFF,
  ON_SECONDARY_CHANNEL_CHANGE
};

};

#endif
