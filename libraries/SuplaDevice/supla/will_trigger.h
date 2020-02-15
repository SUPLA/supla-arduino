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

#ifndef _will_trigger_h
#define _will_trigger_h

#include "triggerable.h"

#define MAX_TRIGGERABLE_CLIENTS 10

namespace Supla {

class TriggerableClient {
 public:
  Triggerable *client;
  int onEvent;
  int action;
};

class WillTrigger {
 public:
  WillTrigger() : registeredClientsCount(0) {
  }

  virtual void willTrigger(Triggerable &client, int event, int action) {
    if (registeredClientsCount < MAX_TRIGGERABLE_CLIENTS) {
      clients[registeredClientsCount].client = &client;
      clients[registeredClientsCount].onEvent = event;
      clients[registeredClientsCount].action = action;
      registeredClientsCount++;
    }
  }

  virtual void runTrigger(int event) {
    for (int i = 0; i < registeredClientsCount; i++) {
      if (clients[i].onEvent == event) {
        clients[i].client->trigger(event, clients[i].action);
      }
    }
  }

 protected:
  TriggerableClient clients[MAX_TRIGGERABLE_CLIENTS];
  int registeredClientsCount;
};

};

#endif
