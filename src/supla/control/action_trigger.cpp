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

#include "action_trigger.h"

Supla::Control::ActionTrigger::ActionTrigger() {
  channel.setType(SUPLA_CHANNELTYPE_ACTIONTRIGGER);
  channel.setDefault(SUPLA_CHANNELFNC_ACTIONTRIGGER);
}

Supla::Control::ActionTrigger::~ActionTrigger() {}

void Supla::Control::ActionTrigger::attach(Supla::Control::Button *button) {
  attachedButton = button;
}

void Supla::Control::ActionTrigger::attach(Supla::Control::Button &button) {
  attach(&button);
}

void Supla::Control::ActionTrigger::handleAction(int event, int action) {
  (void)(event);
  uint32_t actionCap = getActionTriggerCap(action);

  if (actionCap & activeActionsFromServer) {
    channel.pushAction(actionCap);
  }
}

Supla::Channel *Supla::Control::ActionTrigger::getChannel() {
  return &channel;
}

void Supla::Control::ActionTrigger::activateAction(int action) {
  channel.activateAction(getActionTriggerCap(action));
}

int Supla::Control::ActionTrigger::getActionTriggerCap(int action) {
  switch (action) {
    case SEND_AT_TURN_ON: {
      return SUPLA_ACTION_CAP_TURN_ON;
    }
    case SEND_AT_TURN_OFF: {
      return SUPLA_ACTION_CAP_TURN_OFF;
    }
    case SEND_AT_TOGGLE_x1: {
      return SUPLA_ACTION_CAP_TOGGLE_x1;
    }
    case SEND_AT_TOGGLE_x2: {
      return SUPLA_ACTION_CAP_TOGGLE_x2;
    }
    case SEND_AT_TOGGLE_x3: {
      return SUPLA_ACTION_CAP_TOGGLE_x3;
    }
    case SEND_AT_TOGGLE_x4: {
      return SUPLA_ACTION_CAP_TOGGLE_x4;
    }
    case SEND_AT_TOGGLE_x5: {
      return SUPLA_ACTION_CAP_TOGGLE_x5;
    }
    case SEND_AT_HOLD: {
      return SUPLA_ACTION_CAP_HOLD;
    }
    case SEND_AT_SHORT_PRESS_x1: {
      return SUPLA_ACTION_CAP_SHORT_PRESS_x1;
    }
    case SEND_AT_SHORT_PRESS_x2: {
      return SUPLA_ACTION_CAP_SHORT_PRESS_x2;
    }
    case SEND_AT_SHORT_PRESS_x3: {
      return SUPLA_ACTION_CAP_SHORT_PRESS_x3;
    }
    case SEND_AT_SHORT_PRESS_x4: {
      return SUPLA_ACTION_CAP_SHORT_PRESS_x4;
    }
    case SEND_AT_SHORT_PRESS_x5: {
      return SUPLA_ACTION_CAP_SHORT_PRESS_x5;
    }
  }
  return 0;
}
  
void Supla::Control::ActionTrigger::onRegistered() {
  // cleanup actions to be send
  while (channel.popAction());

  channel.requestChannelConfig();

}

void Supla::Control::ActionTrigger::handleChannelConfig(
    TSD_ChannelConfig *result) {
  if (result->ConfigType == 0 &&
      result->ConfigSize == sizeof(TSD_ChannelConfig_ActionTrigger)) {
    TSD_ChannelConfig_ActionTrigger *config =
      reinterpret_cast<TSD_ChannelConfig_ActionTrigger *>(result->Config);
    activeActionsFromServer = config->ActiveActions;
    Serial.print(F("AT["));
    Serial.print(channel.getChannelNumber());
    Serial.print(F("] received config with active actions: "));
    Serial.println(activeActionsFromServer);
  } else {
    Serial.println(F("Invalid format of channel config received for AT"));
  }
}

void Supla::Control::ActionTrigger::setRelatedChannel(Element *element) {
  if (element && element->getChannel()) {
    setRelatedChannel(element->getChannel());
  }
}

void Supla::Control::ActionTrigger::setRelatedChannel(Element &element) {
  setRelatedChannel(&element);
}

void Supla::Control::ActionTrigger::setRelatedChannel(Channel *relatedChannel) {
  if (relatedChannel) {
    channel.setRelatedChannel(relatedChannel->getChannelNumber());
  }
}

void Supla::Control::ActionTrigger::setRelatedChannel(Channel &relatedChannel) {
  setRelatedChannel(&relatedChannel);
}

void Supla::Control::ActionTrigger::onInit() {
  if (attachedButton && attachedButton->isBistable()) {
    attachedButton->addAction(Supla::SEND_AT_TURN_ON, this, Supla::ON_PRESS);
    attachedButton->addAction(Supla::SEND_AT_TURN_OFF, this, Supla::ON_RELEASE);
    attachedButton->addAction(
        Supla::SEND_AT_TOGGLE_x1, this, Supla::ON_CLICK_1);
    attachedButton->addAction(
        Supla::SEND_AT_TOGGLE_x2, this, Supla::ON_CLICK_2);
    attachedButton->addAction(
        Supla::SEND_AT_TOGGLE_x3, this, Supla::ON_CLICK_3);
    attachedButton->addAction(
        Supla::SEND_AT_TOGGLE_x4, this, Supla::ON_CLICK_4);
    attachedButton->addAction(
        Supla::SEND_AT_TOGGLE_x5, this, Supla::ON_CLICK_5);
  }
  if (attachedButton && !attachedButton->isBistable()) {
    attachedButton->addAction(Supla::SEND_AT_HOLD, this, Supla::ON_HOLD);
    attachedButton->addAction(
        Supla::SEND_AT_SHORT_PRESS_x1, this, Supla::ON_CLICK_1);
    attachedButton->addAction(
        Supla::SEND_AT_SHORT_PRESS_x2, this, Supla::ON_CLICK_2);
    attachedButton->addAction(
        Supla::SEND_AT_SHORT_PRESS_x3, this, Supla::ON_CLICK_3);
    attachedButton->addAction(
        Supla::SEND_AT_SHORT_PRESS_x4, this, Supla::ON_CLICK_4);
    attachedButton->addAction(
        Supla::SEND_AT_SHORT_PRESS_x5, this, Supla::ON_CLICK_5);
  }
}
