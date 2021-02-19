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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <arduino_mock.h>
#include <srpc_mock.h>
#include <timer_mock.h>
#include <SuplaDevice.h>
#include <supla/clock/clock.h>
#include <supla/storage/storage.h>

using ::testing::Return;
using ::testing::_;

class TimeInterfaceStub : public TimeInterface {
  public:
    virtual unsigned long millis() override {
      static unsigned long value = 0;
      value += 1000;
      return value;
    }
};

TEST(SuplaDeviceTests, DefaultValuesTest) {
  SuplaDeviceClass sd;
  SrpcMock srpc;
  TimerMock timer;

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_UNKNOWN);
  EXPECT_EQ(sd.getClock(), nullptr);
  

}

class ClockMock : public Supla::Clock {
  public:
    MOCK_METHOD(void,  parseLocaltimeFromServer, (TSDC_UserLocalTimeResult *result), (override));
};

TEST(SuplaDeviceTests, ClockMethods) {
  SuplaDeviceClass sd;
  ClockMock clock;

  ASSERT_EQ(sd.getClock(), nullptr);
  sd.onGetUserLocaltimeResult(nullptr);

  sd.addClock(&clock);
  ASSERT_EQ(sd.getClock(), &clock);

  EXPECT_CALL(clock, parseLocaltimeFromServer(nullptr)).Times(1);

  sd.onGetUserLocaltimeResult(nullptr);
}

TEST(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElements) {
  SuplaDeviceClass sd;
  TimerMock timer;

  ASSERT_EQ(Supla::Element::begin(), nullptr);
  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
}

class StorageMock2: public Supla::Storage {
 public:
  MOCK_METHOD(bool, init, (), (override));
  MOCK_METHOD(bool, prepareState, (bool), (override));
  MOCK_METHOD(bool, finalizeSaveState, (), (override));
  MOCK_METHOD(void, commit, (), (override));
  MOCK_METHOD(int, readStorage, (unsigned int, unsigned char *, int, bool), (override));
  MOCK_METHOD(int, writeStorage, (unsigned int, const unsigned char *, int), (override));

};

TEST(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElementsWithStorage) {
  ::testing::InSequence seq;
  SuplaDeviceClass sd;
  TimerMock timer;
  StorageMock2 storage;

  ASSERT_EQ(Supla::Element::begin(), nullptr);
  EXPECT_CALL(storage, init());
  EXPECT_CALL(storage, prepareState(true)).WillOnce(Return(true));;
  EXPECT_CALL(storage, finalizeSaveState()).WillOnce(Return(false));
  
  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
}

TEST(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElementsWithStorageAndDataLoadAttempt) {
  ::testing::InSequence seq;
  SuplaDeviceClass sd;
  TimerMock timer;
  StorageMock2 storage;

  ASSERT_EQ(Supla::Element::begin(), nullptr);
  EXPECT_CALL(storage, init());
  EXPECT_CALL(storage, prepareState(true)).WillOnce(Return(true));
  EXPECT_CALL(storage, finalizeSaveState()).WillOnce(Return(true));
  EXPECT_CALL(storage, prepareState(false));
  
  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
}

class ElementMock : public Supla::Element {
  public:
  MOCK_METHOD(void, onInit, (), (override));
  MOCK_METHOD(void, onLoadState, (), (override));
  MOCK_METHOD(void, onSaveState, (), (override));
  MOCK_METHOD(void, iterateAlways, (), (override));
  MOCK_METHOD(bool, iterateConnected, (void *), (override));
  MOCK_METHOD(void, onTimer, (), (override));
  MOCK_METHOD(void, onFastTimer, (), (override));
  MOCK_METHOD(int, handleNewValueFromServer, (TSD_SuplaChannelNewValue *), (override));
  MOCK_METHOD(void, handleGetChannelState, (TDSC_ChannelState &), (override));
  MOCK_METHOD(int, handleCalcfgFromServer, (TSD_DeviceCalCfgRequest *), (override));
    
};

TEST(SuplaDeviceTests, StartWithoutNetworkInterfaceWithElements) {
  ::testing::InSequence seq;
  SuplaDeviceClass sd;
  TimerMock timer;
  ElementMock el1;
  ElementMock el2;

  ASSERT_NE(Supla::Element::begin(), nullptr);

  EXPECT_CALL(el1, onInit());
  EXPECT_CALL(el2, onInit());
  
  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
}

TEST(SuplaDeviceTests, StartWithoutNetworkInterfaceWithElementsWithStorage) {
  ::testing::InSequence seq;
  StorageMock2 storage;
  SuplaDeviceClass sd;
  TimerMock timer;
  ElementMock el1;
  ElementMock el2;

  ASSERT_NE(Supla::Element::begin(), nullptr);

  EXPECT_CALL(storage, init());
  EXPECT_CALL(storage, prepareState(true)).WillOnce(Return(true));
  EXPECT_CALL(el1, onSaveState());
  EXPECT_CALL(el2, onSaveState());
  
  EXPECT_CALL(storage, finalizeSaveState()).WillOnce(Return(true));
  EXPECT_CALL(storage, prepareState(false));
  EXPECT_CALL(el1, onLoadState());
  EXPECT_CALL(el2, onLoadState());

  EXPECT_CALL(el1, onInit());
  EXPECT_CALL(el2, onInit());
  
  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
}

class NetworkMock : public Supla::Network {
  public:
    NetworkMock() : Supla::Network(nullptr) {};
  MOCK_METHOD(int, read, (void *, int ), (override));
  MOCK_METHOD(int, write, (void *, int ), (override));
  MOCK_METHOD(int, connect, (const char *, int), (override));
  MOCK_METHOD(bool, connected, (), (override));
  MOCK_METHOD(void, disconnect, (), (override));
  MOCK_METHOD(void, setup, (), (override));
  MOCK_METHOD(void, setTimeout, (int), (override));

  MOCK_METHOD(bool, isReady, (), (override));
  MOCK_METHOD(bool, iterate, (), (override));
  MOCK_METHOD(bool, ping, (void *), (override));

};

TEST(SuplaDeviceTEsts, BeginStopsAtEmptyGUID) {
  ::testing::InSequence seq;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;

  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_GUID);
}

TEST(SuplaDeviceTEsts, BeginStopsAtEmptyServer) {
  ::testing::InSequence seq;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;

  EXPECT_CALL(timer, initTimers());

  char GUID[SUPLA_GUID_SIZE] = {1};
  sd.setGUID(GUID);
  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_UNKNOWN_SERVER_ADDRESS);
}

TEST(SuplaDeviceTEsts, BeginStopsAtEmptyEmail) {
  ::testing::InSequence seq;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;

  EXPECT_CALL(timer, initTimers());

  char GUID[SUPLA_GUID_SIZE] = {1};
  sd.setGUID(GUID);
  sd.setServer("supla.rulez");
  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_CREDENTIALS);
}


TEST(SuplaDeviceTEsts, BeginStopsAtEmptyAuthkey) {
  ::testing::InSequence seq;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;

  EXPECT_CALL(timer, initTimers());

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {2};
  sd.setGUID(GUID);
  sd.setServer("supla.rulez");
  sd.setEmail("john@supla");
  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_CREDENTIALS);
}

TEST(SuplaDeviceTEsts, SuccessfulBegin) {
  ::testing::InSequence seq;
  SrpcMock srpc;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;
  int dummy;

  EXPECT_CALL(timer, initTimers());
  EXPECT_CALL(net, setup());
  EXPECT_CALL(srpc, srpc_params_init(_));
  EXPECT_CALL(srpc, srpc_init(_)).WillOnce(Return(&dummy));
  EXPECT_CALL(srpc, srpc_set_proto_version(&dummy, 12));

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {2};
  sd.setGUID(GUID);
  sd.setServer("supla.rulez");
  sd.setEmail("john@supla");
  sd.setAuthKey(AUTHKEY);
  EXPECT_TRUE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INITIALIZED);
}

