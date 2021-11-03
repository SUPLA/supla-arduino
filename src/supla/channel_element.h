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

#ifndef _channel_element_h
#define _channel_element_h

#include "element.h"
#include "channel.h"
#include "local_action.h"
#include "action_handler.h"
#include "condition.h"

namespace Supla {

class Condition;

class ChannelElement : public Element, public LocalAction {
  public:
  
  Channel *getChannel();

  // Override local action methods in order to delegate execution to Channel
  void addAction(int action, ActionHandler &client, int event) override;
  void addAction(int action, ActionHandler *client, int event) override;
  bool isEventAlreadyUsed(int event) override;

  virtual void addAction(int action, ActionHandler &client, Supla::Condition *condition);
  virtual void addAction(int action, ActionHandler *client, Supla::Condition *condition);

  protected:
    Channel channel;
};

};

#endif

