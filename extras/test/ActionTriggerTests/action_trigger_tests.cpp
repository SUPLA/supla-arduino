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
#include <supla/control/action_trigger.h>
#include <supla/control/button.h>
#include <srpc_mock.h>
#include <arduino_mock.h>

class ActionTriggerTests : public ::testing::Test {
  protected:
    virtual void SetUp() {
      Supla::Channel::lastCommunicationTimeMs = 0;
      memset(&(Supla::Channel::reg_dev), 0, sizeof(Supla::Channel::reg_dev));
    }
    virtual void TearDown() {
      Supla::Channel::lastCommunicationTimeMs = 0;
      memset(&(Supla::Channel::reg_dev), 0, sizeof(Supla::Channel::reg_dev));
    }

};

class TimeInterfaceStub : public TimeInterface {
  public:
    virtual unsigned long millis() override {
      static unsigned long value = 0;
      value += 1000;
      return value;
    }
};

TEST_F(ActionTriggerTests, AttachToButton) {
  Supla::Control::Button b1(10);
  Supla::Control::ActionTrigger at;

  at.attach(b1);
}


TEST_F(ActionTriggerTests, SendActionOnce) {
  SrpcMock srpc;
  TimeInterfaceStub time;
  Supla::Control::ActionTrigger at;
  Supla::Control::ActionTrigger at2;

  at.iterateConnected(0);

  EXPECT_CALL(srpc, actionTrigger(0, SUPLA_ACTION_CAP_TURN_ON));
  EXPECT_CALL(srpc, actionTrigger(1, SUPLA_ACTION_CAP_SHORT_PRESS_x1));

  at.handleAction(0, Supla::SEND_AT_TURN_ON);

  at.iterateConnected(0);
  at.iterateConnected(0);
  at.iterateConnected(0);

  at2.iterateConnected(0);
  at2.handleAction(0, Supla::SEND_AT_SHORT_PRESS_x1);

  at2.iterateConnected(0);
  at2.iterateConnected(0);
}

TEST_F(ActionTriggerTests, SendFewActions) {
  SrpcMock srpc;
  TimeInterfaceStub time;
  Supla::Control::ActionTrigger at;

  at.iterateConnected(0);

  EXPECT_CALL(srpc, actionTrigger(0, SUPLA_ACTION_CAP_TURN_ON));

  at.handleAction(0, Supla::SEND_AT_TURN_ON);

  at.iterateConnected(0);
  at.iterateConnected(0);
  at.iterateConnected(0);
}

TEST_F(ActionTriggerTests, ActionsShouldAddCaps) {
  SrpcMock srpc;
  TimeInterfaceStub time;
  Supla::Control::ActionTrigger at;
  Supla::Control::Button button(10, false, false);

  EXPECT_EQ(at.getChannel()->getActionTriggerCaps(), 0);

  button.addAction(Supla::SEND_AT_HOLD, at, Supla::ON_PRESS);
  EXPECT_EQ(at.getChannel()->getActionTriggerCaps(), SUPLA_ACTION_CAP_HOLD);

  button.addAction(Supla::SEND_AT_TOGGLE_x2, at, Supla::ON_PRESS);
  EXPECT_EQ(at.getChannel()->getActionTriggerCaps(),
      SUPLA_ACTION_CAP_HOLD | SUPLA_ACTION_CAP_TOGGLE_x2);

  button.addAction(Supla::SEND_AT_SHORT_PRESS_x5, at, Supla::ON_PRESS);
  EXPECT_EQ(at.getChannel()->getActionTriggerCaps(),
      SUPLA_ACTION_CAP_HOLD | SUPLA_ACTION_CAP_TOGGLE_x2 |
      SUPLA_ACTION_CAP_SHORT_PRESS_x5);
}



