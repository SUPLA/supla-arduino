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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <supla/channel_element.h>
#include <supla/events.h>
#include <supla/actions.h>
#include <supla/action_handler.h>


class ActionHandlerMock : public Supla::ActionHandler {
 public:
  MOCK_METHOD(void, handleAction, (int, int), (override));
};


TEST(ChannelElementTests, ActionDelegationToChannel) {
  Supla::ChannelElement element;

  ActionHandlerMock mock1;
  ActionHandlerMock mock2;

  int action1 = 11;

  EXPECT_CALL(mock1, handleAction(Supla::ON_TURN_ON, action1)).Times(2);
  EXPECT_CALL(mock2, handleAction(Supla::ON_TURN_OFF, action1)).Times(2);

  element.addAction(action1, mock1, Supla::ON_TURN_ON);
  element.addAction(action1, &mock2, Supla::ON_TURN_OFF);

  element.getChannel()->setNewValue(false);
  element.getChannel()->setNewValue(true);
  element.getChannel()->setNewValue(true);

  element.getChannel()->setNewValue(false);
  element.getChannel()->setNewValue(true);
  element.getChannel()->setNewValue(false);
  element.getChannel()->setNewValue(false);
}

