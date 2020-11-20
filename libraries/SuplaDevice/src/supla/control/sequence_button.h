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

#ifndef _sequence_button_h
#define _sequence_button_h

#include "button.h"

namespace Supla {
namespace Control {

#define SEQUENCE_MAX_SIZE 30

struct ClickSequence {
  uint16_t data[SEQUENCE_MAX_SIZE];
};
 
class SequenceButton : public SimpleButton {
 public:
  SequenceButton(int pin, bool pullUp = false, bool invertLogic = false);

  void onTimer();

  void setSequence(uint16_t *sequence);
  void setMargin(float);
  void getLastRecordedSequence(uint16_t *sequence);

 protected:
  unsigned long lastStateChangeMs;
  uint16_t longestSequenceTimeDeltaWithMargin;
  uint8_t clickCounter;
  bool sequenceDetectecion;

  ClickSequence currentSequence;
  ClickSequence matchSequence;

  float margin;
  unsigned int calculateMargin(unsigned int);

};

};  // namespace Control
};  // namespace Supla

#endif
