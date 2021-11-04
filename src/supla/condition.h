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

#ifndef _condition_h
#define _condition_h

#include "action_handler.h"
#include "condition_getter.h"

namespace Supla {

class Element;
 
class Condition : public ActionHandler {
 public:
  Condition(double threshold, bool useAlternativeMeasurement);
  Condition(double threshold, ConditionGetter *getter);

  virtual ~Condition();
  void setSource(Element *src);
  void setClient(ActionHandler *clientPtr);
  void setSource(Element &src);
  void setClient(ActionHandler &clientPtr);

  void handleAction(int event, int action);
  bool deleteClient();
  virtual bool checkConditionFor(double val, bool isValid = true);

 protected:
  virtual bool condition(double val, bool isValid = true) = 0;

  double threshold = 0;
  bool useAlternativeMeasurement = false;
  bool alreadyFired = false;
  Supla::Element *source = nullptr;
  Supla::ActionHandler *client = nullptr;
  Supla::ConditionGetter *getter = nullptr;

};

};

Supla::Condition *OnLess(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnLessEq(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnGreater(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnGreaterEq(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnBetween(double threshold1, double threshold2, bool useAlternativeMeasurement = false);
Supla::Condition *OnBetweenEq(double threshold1, double threshold2, bool useAlternativeMeasurement = false);
Supla::Condition *OnEqual(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnInvalid(bool useAlternativeMeasurement = false);

Supla::Condition *OnLess(double threshold, Supla::ConditionGetter *);
Supla::Condition *OnLessEq(double threshold, Supla::ConditionGetter *);
Supla::Condition *OnGreater(double threshold, Supla::ConditionGetter *);
Supla::Condition *OnGreaterEq(double threshold, Supla::ConditionGetter *);
Supla::Condition *OnBetween(double threshold1, double threshold2, Supla::ConditionGetter *);
Supla::Condition *OnBetweenEq(double threshold1, double threshold2, Supla::ConditionGetter *);
Supla::Condition *OnEqual(double threshold, Supla::ConditionGetter *);
Supla::Condition *OnInvalid(Supla::ConditionGetter *);

#endif
