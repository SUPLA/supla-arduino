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

#ifndef _action_trigger_h
#define _action_trigger_h

#include <stdint.h>
#include "simple_button.h"
#include "../element.h"
#include "../action_handler.h"
#include "../at_channel.h"
#include "../actions.h"

namespace Supla {
namespace Control {

class ActionTrigger : public Element, public ActionHandler {
 public:
  ActionTrigger(); 
  virtual ~ActionTrigger();

  void attach(Supla::Control::SimpleButton *);
  void attach(Supla::Control::SimpleButton &);


  void handleAction(int event, int action) override;
  void activateAction(int action) override;
  Supla::Channel *getChannel() override;
  void onRegistered() override;

 protected:
  Supla::Control::SimpleButton *attachedButton;
  Supla::AtChannel channel;

  int getActionTriggerCap(int action);
};

}
}

#endif /*_action_trigger_h*/
