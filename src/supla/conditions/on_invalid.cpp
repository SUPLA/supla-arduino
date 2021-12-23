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

#include "../condition.h"

class OnInvalidCond : public Supla::Condition {
 public:
  OnInvalidCond(bool useAlternativeMeasurement)
      : Supla::Condition(0, useAlternativeMeasurement) {
  }

  OnInvalidCond(Supla::ConditionGetter *getter)
      : Supla::Condition(0, getter) {
  }

  bool condition(double val, bool isValid) {
    (void)(val);
    return !isValid;
  }

};


Supla::Condition *OnInvalid(bool useAlternativeMeasurement) {
  return new OnInvalidCond(useAlternativeMeasurement);
}

Supla::Condition *OnInvalid(Supla::ConditionGetter *getter) {
  return new OnInvalidCond(getter);
}
