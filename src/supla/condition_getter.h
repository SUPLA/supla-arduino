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

#ifndef _condition_getter_h
#define _condition_getter_h

#include <stdint.h>
#include <supla-common/proto.h>

namespace Supla {

class Element;

class ConditionGetter {
  public:
    virtual double getValue(Supla::Element *element, bool &isValid) = 0;
  protected:
    TElectricityMeter_Measurement *getMeasurement(Supla::Element *element,
        _supla_int_t &measuredValues);
};


};  // namespace Supla

Supla::ConditionGetter *EmVoltage(int8_t phase = 0);
Supla::ConditionGetter *EmCurrent(int8_t phase = 0);
Supla::ConditionGetter *EmTotalCurrent();
Supla::ConditionGetter *EmPowerActiveW(int8_t phase = 0);
Supla::ConditionGetter *EmTotalPowerActiveW();
Supla::ConditionGetter *EmPowerApparentVA(int8_t phase = 0);
Supla::ConditionGetter *EmTotalPowerApparentVA();
Supla::ConditionGetter *EmPowerReactiveVar(int8_t phase = 0);
Supla::ConditionGetter *EmTotalPowerReactiveVar();

#endif /*_condition_getter_h*/
