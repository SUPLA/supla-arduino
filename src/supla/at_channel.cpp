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

#include "at_channel.h"
#include "supla-common/srpc.h"

namespace Supla {

  void AtChannel::sendUpdate(void *srpc) {
    TDS_ActionTrigger at = {};
    at.ChannelNumber = getChannelNumber();
    at.ActionTrigger = popAction();
    srpc_ds_async_action_trigger(srpc, &at);
  }

  int AtChannel::popAction() {
    for (int i = 0; i < 32; i++) {
      if (actionToSend & (1 << i)) {
        actionToSend ^= (1 << i);
        if (actionToSend == 0) {
          clearUpdateReady();
        }
        return (1 << i);
      }
    }
    return 0;
  }

  void AtChannel::pushAction(int action) {
    actionToSend |= action;
    setUpdateReady();
  }

  void AtChannel::activateAction(int action) {
    setActionTriggerCaps(getActionTriggerCaps() | action);
  }

};
