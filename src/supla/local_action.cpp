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

#include "supla/local_action.h"

namespace Supla {

LocalAction::LocalAction() : registeredClientsCount(0) {
}

void LocalAction::addAction(int action, Triggerable &client, int event) {
  if (registeredClientsCount < MAX_TRIGGERABLE_CLIENTS) {
    clients[registeredClientsCount].client = &client;
    clients[registeredClientsCount].onEvent = event;
    clients[registeredClientsCount].action = action;
    registeredClientsCount++;
  }
}

void LocalAction::addAction(int action, Triggerable *client, int event) {
  addAction(action, *client, event);
}

void LocalAction::runAction(int event) {
  for (int i = 0; i < registeredClientsCount; i++) {
    if (clients[i].onEvent == event) {
      clients[i].client->runAction(event, clients[i].action);
    }
  }
}

};  // namespace Supla

