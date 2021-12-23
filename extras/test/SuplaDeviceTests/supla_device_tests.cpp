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
#include <element_mock.h>

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Assign;
using ::testing::ReturnPointee;

class SuplaDeviceTests : public ::testing::Test {
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

TEST_F(SuplaDeviceTests, DefaultValuesTest) {
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

TEST_F(SuplaDeviceTests, ClockMethods) {
  SuplaDeviceClass sd;
  ClockMock clock;

  ASSERT_EQ(sd.getClock(), nullptr);
  sd.onGetUserLocaltimeResult(nullptr);

  sd.addClock(&clock);
  ASSERT_EQ(sd.getClock(), &clock);

  EXPECT_CALL(clock, parseLocaltimeFromServer(nullptr)).Times(1);

  sd.onGetUserLocaltimeResult(nullptr);
}

TEST_F(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElements) {
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

TEST_F(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElementsWithStorage) {
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

TEST_F(SuplaDeviceTests, StartWithoutNetworkInterfaceNoElementsWithStorageAndDataLoadAttempt) {
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

TEST_F(SuplaDeviceTests, StartWithoutNetworkInterfaceWithElements) {
  ::testing::InSequence seq;
  SuplaDeviceClass sd;
  TimerMock timer;
  ElementMock el1;
  ElementMock el2;

  ASSERT_NE(Supla::Element::begin(), nullptr);

  EXPECT_CALL(el1, onInit());
  EXPECT_CALL(el2, onInit());
  
  EXPECT_CALL(timer, initTimers());
  EXPECT_CALL(el1, onTimer());
  EXPECT_CALL(el2, onTimer());
  EXPECT_CALL(el1, onFastTimer());
  EXPECT_CALL(el2, onFastTimer());



  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_MISSING_NETWORK_INTERFACE);
  sd.onTimer();
  sd.onFastTimer();
}

TEST_F(SuplaDeviceTests, StartWithoutNetworkInterfaceWithElementsWithStorage) {
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

TEST_F(SuplaDeviceTests, BeginStopsAtEmptyGUID) {
  ::testing::InSequence seq;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;

  EXPECT_CALL(timer, initTimers());

  EXPECT_FALSE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_GUID);
}

TEST_F(SuplaDeviceTests, BeginStopsAtEmptyServer) {
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

TEST_F(SuplaDeviceTests, BeginStopsAtEmptyEmail) {
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


TEST_F(SuplaDeviceTests, BeginStopsAtEmptyAuthkey) {
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
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_AUTHKEY);
}

TEST_F(SuplaDeviceTests, SuccessfulBegin) {
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
  EXPECT_CALL(srpc, srpc_set_proto_version(&dummy, 16));

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {2};
  sd.setGUID(GUID);
  sd.setServer("supla.rulez");
  sd.setEmail("john@supla");
  sd.setAuthKey(AUTHKEY);
  EXPECT_TRUE(sd.begin());
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INITIALIZED);
}



TEST_F(SuplaDeviceTests, SuccessfulBeginAlternative) {
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
  EXPECT_CALL(srpc, srpc_set_proto_version(&dummy, 16));

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {2};
  EXPECT_TRUE(sd.begin(GUID, "supla.rulez", "superman@supla.org", AUTHKEY));
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INITIALIZED);
}


TEST_F(SuplaDeviceTests, FailedBeginAlternativeOnEmptyAUTHKEY) {
  ::testing::InSequence seq;
  SrpcMock srpc;
  NetworkMock net;
  TimerMock timer;

  SuplaDeviceClass sd;
  int dummy;

  EXPECT_CALL(timer, initTimers());

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0};
  EXPECT_FALSE(sd.begin(GUID, "supla.rulez", "superman@supla.org", AUTHKEY));
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_AUTHKEY);
}

TEST_F(SuplaDeviceTests, TwoChannelElementsNoNetworkWithStorage) {
  SrpcMock srpc;
  NetworkMock net;
  StorageMock2 storage;
  TimerMock timer;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;
  ElementMock el1;
  ElementMock el2;
  int dummy;
  EXPECT_CALL(storage, prepareState(true)).WillOnce(Return(true));
  EXPECT_CALL(storage, init());
  EXPECT_CALL(el1, onSaveState());
  EXPECT_CALL(el2, onSaveState());

  EXPECT_CALL(storage, finalizeSaveState()).WillOnce(Return(true));
  EXPECT_CALL(storage, prepareState(false));
  EXPECT_CALL(el1, onLoadState());
  EXPECT_CALL(el2, onLoadState());

  EXPECT_CALL(el1, onInit());
  EXPECT_CALL(el2, onInit());

  EXPECT_CALL(timer, initTimers());
  EXPECT_CALL(net, setup());
  EXPECT_CALL(srpc, srpc_params_init(_));
  EXPECT_CALL(srpc, srpc_init(_)).WillOnce(Return(&dummy));
  EXPECT_CALL(srpc, srpc_set_proto_version(&dummy, 16));

  char GUID[SUPLA_GUID_SIZE] = {1};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {2};
  EXPECT_TRUE(sd.begin(GUID, "supla.rulez", "superman@supla.org", AUTHKEY));
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INITIALIZED);
  EXPECT_CALL(el1, iterateAlways()).Times(2);
  EXPECT_CALL(el2, iterateAlways()).Times(2);
  EXPECT_CALL(net, isReady()).WillRepeatedly(Return(false));

  EXPECT_CALL(storage, prepareState(false));
  EXPECT_CALL(el1, onSaveState());
  EXPECT_CALL(el2, onSaveState());
  EXPECT_CALL(storage, finalizeSaveState());

  for (int i = 0; i < 2; i++) sd.iterate();
}

TEST_F(SuplaDeviceTests, OnVersionErrorShouldCallDisconnect) {
  NetworkMock net;
  TimeInterfaceStub time;

  EXPECT_CALL(net, disconnect()).Times(1);

  SuplaDeviceClass sd;
  TSDC_SuplaVersionError versionError{};

  sd.onVersionError(&versionError);
  EXPECT_EQ(sd.getCurrentStatus(), STATUS_PROTOCOL_VERSION_ERROR);
}

TEST_F(SuplaDeviceTests, OnRegisterResultOK) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_TRUE;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_REGISTERED_AND_READY);
}

TEST_F(SuplaDeviceTests, OnRegisterResultBadCredentials) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_BAD_CREDENTIALS;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_BAD_CREDENTIALS);
}

TEST_F(SuplaDeviceTests, OnRegisterResultTemporairlyUnavailable) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_TEMPORARILY_UNAVAILABLE;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_TEMPORARILY_UNAVAILABLE);
}

TEST_F(SuplaDeviceTests, OnRegisterResultLocationConflict) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_LOCATION_CONFLICT;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_LOCATION_CONFLICT);
}

TEST_F(SuplaDeviceTests, OnRegisterResultChannelConflict) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_CHANNEL_CONFLICT;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_CHANNEL_CONFLICT);
}

TEST_F(SuplaDeviceTests, OnRegisterResultDeviceDisabled) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_DEVICE_DISABLED;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_DEVICE_IS_DISABLED);
}

TEST_F(SuplaDeviceTests, OnRegisterResultLocationDisabled) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_LOCATION_DISABLED;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_LOCATION_IS_DISABLED);
}

TEST_F(SuplaDeviceTests, OnRegisterResultDeviceLimitExceeded) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_DEVICE_LIMITEXCEEDED;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_DEVICE_LIMIT_EXCEEDED);
}

TEST_F(SuplaDeviceTests, OnRegisterResultGuidError) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_GUID_ERROR;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_GUID);
}

TEST_F(SuplaDeviceTests, OnRegisterResultAuthKeyError) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_AUTHKEY_ERROR;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_INVALID_AUTHKEY);
}

TEST_F(SuplaDeviceTests, OnRegisterResultRegistrationDisabled) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_REGISTRATION_DISABLED;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_REGISTRATION_DISABLED);
}

TEST_F(SuplaDeviceTests, OnRegisterResultNoLocationAvailable) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = SUPLA_RESULTCODE_NO_LOCATION_AVAILABLE;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_NO_LOCATION_AVAILABLE);
}

TEST_F(SuplaDeviceTests, OnRegisterResultUnknownError) {
  NetworkMock net;
  SrpcMock srpc;
  TimeInterfaceStub time;
  SuplaDeviceClass sd;

  EXPECT_CALL(srpc, srpc_dcs_async_set_activity_timeout(_, _)).Times(0);
  EXPECT_CALL(net, disconnect()).Times(1);

  TSD_SuplaRegisterDeviceResult register_device_result{};
  register_device_result.result_code = 666;
  register_device_result.activity_timeout = 45;
  register_device_result.version = 16;
  register_device_result.version_min = 1;

  sd.onRegisterResult(&register_device_result);

  EXPECT_EQ(sd.getCurrentStatus(), STATUS_UNKNOWN_ERROR);
}
