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
#include "channel_element.h"


namespace Supla {

class ChannelElement;
 
class Condition : public ActionHandler {
 public:
  Condition(double threshold, bool useAlternativeMeasurement);
  virtual ~Condition();
  void setSource(ChannelElement *src);
  void setClient(ActionHandler *clientPtr);
  void setSource(ChannelElement &src);
  void setClient(ActionHandler &clientPtr);

  void handleAction(int event, int action);
  bool deleteClient();
  virtual bool checkConditionFor(double val);

 protected:
  virtual bool condition(double val) = 0;

  double threshold;
  bool alreadyFired;
  bool useAlternativeMeasurement;
  Supla::ChannelElement *source;
  Supla::ActionHandler *client;

};

};

Supla::Condition *OnLess(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnLessEq(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnGreater(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnGreaterEq(double threshold, bool useAlternativeMeasurement = false);
Supla::Condition *OnBetween(double threshold1, double threshold2, bool useAlternativeMeasurement = false);
Supla::Condition *OnBetweenEq(double threshold1, double threshold2, bool useAlternativeMeasurement = false);
Supla::Condition *OnEqual(double threshold, bool useAlternativeMeasurement = false);

#endif
