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

#include <Arduino.h>
#include <string.h>

#include "SuplaDevice.h"
#include "supla-common/IEEE754tools.h"
#include "supla-common/log.h"
#include "supla-common/srpc.h"
#include "supla/channel.h"
#include "supla/element.h"
#include "supla/io.h"
#include "supla/storage/storage.h"
#include "supla/timer.h"

void SuplaDeviceClass::status(int newStatus, const char *msg, bool alwaysLog) {
  bool showLog = false;
  if (currentStatus != newStatus && !(newStatus == STATUS_REGISTER_IN_PROGRESS && currentStatus > STATUS_REGISTER_IN_PROGRESS)) {
    if (impl_arduino_status != NULL) {
      impl_arduino_status(newStatus, msg);
    } 
    currentStatus = newStatus;
    showLog = true;
  }
  if (alwaysLog || showLog) supla_log(LOG_DEBUG, "Current status: [%d] %s", newStatus, msg);
}

SuplaDeviceClass::SuplaDeviceClass()
    : port(-1),
      connectionFailCounter(0),
      networkIsNotReadyCounter(0),
      currentStatus(STATUS_UNKNOWN),
      impl_arduino_status(nullptr),
      clock(nullptr) {
  srpc = NULL;
  registered = 0;
  lastIterateTime = 0;
  waitForIterate = 0;
}

SuplaDeviceClass::~SuplaDeviceClass() {
}

void SuplaDeviceClass::setStatusFuncImpl(
    _impl_arduino_status impl_arduino_status) {
  this->impl_arduino_status = impl_arduino_status;
}

bool SuplaDeviceClass::isInitialized(bool msg) {
  if (srpc != NULL) {
    if (msg)
      status(STATUS_ALREADY_INITIALIZED, "SuplaDevice is already initialized");

    return true;
  }

  return false;
}

bool SuplaDeviceClass::begin(char GUID[SUPLA_GUID_SIZE],
                             const char *Server,
                             const char *email,
                             char authkey[SUPLA_AUTHKEY_SIZE],
                             unsigned char version) {
  setGUID(GUID);
  setServer(Server);
  setEmail(email);
  setAuthKey(authkey);

  return begin(version);
}

bool SuplaDeviceClass::begin(unsigned char version) {
  if (isInitialized(true)) return false;
  supla_log(LOG_DEBUG, "Supla - starting initialization");

  Supla::Storage::Init();

  // Supla::Storage::LoadDeviceConfig();
  // Supla::Storage::LoadElementConfig();

  // Pefrorm dry run of write state to validate stored state section with
  // current device configuration
  if (Supla::Storage::PrepareState(true)) {
    Serial.println(F(
        "Validating storage state section with current device configuration"));
    for (auto element = Supla::Element::begin(); element != nullptr;
         element = element->next()) {
      element->onSaveState();
      delay(0);
    }
    // If state storage validation was successful, perform read state
    if (Supla::Storage::FinalizeSaveState()) {
      Serial.println(
          F("Storage state section validation completed. Loading elements "
            "state..."));
      // Iterate all elements and load state
      Supla::Storage::PrepareState();
      for (auto element = Supla::Element::begin(); element != nullptr;
           element = element->next()) {
        element->onLoadState();
        delay(0);
      }
    }
  } else {
    Serial.println(F("Storage not found. Running without state memory"));
  }

  // Initialize elements
  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    element->onInit();
    delay(0);
  }

  // Enable timers
  Supla::initTimers();

  if (Supla::Network::Instance() == NULL) {
    status(STATUS_MISSING_NETWORK_INTERFACE, "Network Interface not defined!");
    return false;
  }

  bool emptyGuidDetected = true;
  for (int i = 0; i < SUPLA_GUID_SIZE; i++) {
    if (Supla::Channel::reg_dev.GUID[i] != 0) {
      emptyGuidDetected = false;
    }
  }
  if (emptyGuidDetected) {
    status(STATUS_INVALID_GUID, "Missing GUID");
    return false;
  }

  if (Supla::Channel::reg_dev.ServerName[0] == '\0') {
    status(STATUS_UNKNOWN_SERVER_ADDRESS, "Missing server address");
    return false;
  }

  if (Supla::Channel::reg_dev.Email[0] == '\0') {
    status(STATUS_MISSING_CREDENTIALS, "Missing email address");
    return false;
  }

  bool emptyAuthKeyDetected = true;
  for (int i = 0; i < SUPLA_AUTHKEY_SIZE; i++) {
    if (Supla::Channel::reg_dev.AuthKey[i] != 0) {
      emptyAuthKeyDetected = false;
      break;
    }
  }
  if (emptyAuthKeyDetected) {
    status(STATUS_INVALID_AUTHKEY, "Missing AuthKey");
    return false;
  }

  if (strnlen(Supla::Channel::reg_dev.Name, SUPLA_DEVICE_NAME_MAXSIZE) == 0) {
#if defined(ARDUINO_ARCH_ESP8266)
    setString(
        Supla::Channel::reg_dev.Name, "ESP8266", SUPLA_DEVICE_NAME_MAXSIZE);
#elif defined(ARDUINO_ARCH_ESP32)
    setString(Supla::Channel::reg_dev.Name, "ESP32", SUPLA_DEVICE_NAME_MAXSIZE);
#else
    setString(
        Supla::Channel::reg_dev.Name, "ARDUINO", SUPLA_DEVICE_NAME_MAXSIZE);
#endif
  }

  if (strnlen(Supla::Channel::reg_dev.SoftVer, SUPLA_SOFTVER_MAXSIZE) == 0) {
    setString(Supla::Channel::reg_dev.SoftVer,
              "User SW, lib 2.3.5",
              SUPLA_SOFTVER_MAXSIZE);
  }

  Serial.println(F("Initializing network layer"));
  Supla::Network::Setup();

  TsrpcParams srpc_params;
  srpc_params_init(&srpc_params);
  srpc_params.data_read = &Supla::data_read;
  srpc_params.data_write = &Supla::data_write;
  srpc_params.on_remote_call_received = &Supla::message_received;
  srpc_params.user_params = this;

  srpc = srpc_init(&srpc_params);

  // Set Supla protocol interface version
  srpc_set_proto_version(srpc, version);

  supla_log(LOG_DEBUG, "Using Supla protocol version %d", version);

  status(STATUS_INITIALIZED, "SuplaDevice initialized");
  return true;
}

void SuplaDeviceClass::setName(const char *Name) {
  if (isInitialized(true)) return;
  setString(Supla::Channel::reg_dev.Name, Name, SUPLA_DEVICE_NAME_MAXSIZE);
}

void SuplaDeviceClass::setString(char *dst, const char *src, int max_size) {
  if (src == NULL) {
    dst[0] = 0;
    return;
  }

  int size = strlen(src);

  if (size + 1 > max_size) size = max_size - 1;

  memcpy(dst, src, size);
}

void SuplaDeviceClass::onTimer(void) {
  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    element->onTimer();
  }
}

void SuplaDeviceClass::onFastTimer(void) {
  // Iteration over all impulse counters will count incomming impulses. It is
  // after SuplaDevice initialization (because we have to read stored counter
  // values) and before any other operation like connection to Supla cloud
  // (because we want to count impulses even when we have connection issues.
  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    element->onFastTimer();
  }
}

void SuplaDeviceClass::iterate(void) {
  if (!isInitialized(false)) return;

  unsigned long _millis = millis();
  unsigned long timeDiff = _millis - lastIterateTime;

  uptime.iterate(_millis);

  // Iterate all elements
  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    element->iterateAlways();
    delay(0);
  }

  // Iterate all elements and saves state
  if (Supla::Storage::SaveStateAllowed(_millis)) {
    Supla::Storage::PrepareState();
    for (auto element = Supla::Element::begin(); element != nullptr;
         element = element->next()) {
      element->onSaveState();
      delay(0);
    }
    Supla::Storage::FinalizeSaveState();
  }

  if (waitForIterate != 0 && _millis < waitForIterate) {
    return;
  } else {
    waitForIterate = 0;
  }

  // Restart network after >1 min of failed connection attempts
  if (connectionFailCounter > 30) {
    connectionFailCounter = 0;
    supla_log(LOG_DEBUG,
              "Connection fail counter overflow. Trying to setup network "
              "interface again");
    Supla::Network::Setup();
    return;
  }

  if (!Supla::Network::IsReady()) {
    uptime.setConnectionLostCause(
        SUPLA_LASTCONNECTIONRESETCAUSE_WIFI_CONNECTION_LOST);
    waitForIterate = _millis + 100;
    status(STATUS_NETWORK_DISCONNECTED, "No connection to network");
    networkIsNotReadyCounter++;
    if (networkIsNotReadyCounter > 20) {
      networkIsNotReadyCounter = 0;
      connectionFailCounter++;
    }
    return;
  }
  networkIsNotReadyCounter = 0;

  if (!Supla::Network::Connected()) {

    uptime.setConnectionLostCause(
        SUPLA_LASTCONNECTIONRESETCAUSE_SERVER_CONNECTION_LOST);

    registered = 0;

    int result =
        Supla::Network::Connect(Supla::Channel::reg_dev.ServerName, port);
    if (1 == result) {
      uptime.resetConnectionUptime();
      connectionFailCounter = 0;
      supla_log(LOG_DEBUG, "Connected to Supla Server");
    } else {
      status(STATUS_SERVER_DISCONNECTED, "Not connected to Supla server");
      supla_log(LOG_DEBUG,
                "Connection fail (%d). Server: %s",
                result,
                Supla::Channel::reg_dev.ServerName);

      Supla::Network::Disconnect();
      waitForIterate = _millis + 2000;
      connectionFailCounter++;
      return;
    }
  }

  Supla::Network::Iterate();

  if (srpc_iterate(srpc) == SUPLA_RESULT_FALSE) {
    status(STATUS_ITERATE_FAIL, "Iterate fail");
    Supla::Network::Disconnect();

    waitForIterate = _millis + 5000;
    return;
  }

  if (registered == 0) {
    // Perform registration if we are not yet registered
    registered = -1;
    lastIterateTime = _millis;
    status(STATUS_REGISTER_IN_PROGRESS, "Register in progress");
    if (!srpc_ds_async_registerdevice_e(srpc, &Supla::Channel::reg_dev)) {
      supla_log(LOG_DEBUG, "Fatal SRPC failure!");
    }
  } else if (registered == -1) {
    // Handle registration timeout (in case of no reply received)
    if (timeDiff > 10*1000) {
      supla_log(LOG_DEBUG, "No reply to registration message. Resetting connection.");
      status(STATUS_SERVER_DISCONNECTED, "Not connected to Supla server");
      Supla::Network::Disconnect();

      waitForIterate = _millis + 2000;
      connectionFailCounter++;
    }

  } else if (registered == 1) {
    // Device is registered and everything is correct

    if (Supla::Network::Ping(srpc) == false) {
      uptime.setConnectionLostCause(
          SUPLA_LASTCONNECTIONRESETCAUSE_ACTIVITY_TIMEOUT);
      supla_log(LOG_DEBUG, "TIMEOUT - lost connection with server");
      status(STATUS_SERVER_DISCONNECTED, "Not connected to Supla server");
      Supla::Network::Disconnect();
    }

    if (timeDiff > 0) {
      // Iterate all elements
      for (auto element = Supla::Element::begin(); element != nullptr;
           element = element->next()) {
        if (!element->iterateConnected(srpc)) {
          break;
        }
        delay(0);
      }

      lastIterateTime = _millis;
    }
  }
}

void SuplaDeviceClass::onVersionError(TSDC_SuplaVersionError *version_error) {
  status(STATUS_PROTOCOL_VERSION_ERROR, "Protocol version error");
  Serial.print(F("Protocol version error. Server min: "));
  Serial.print(version_error->server_version_min);
  Serial.print(F("; Server version: "));
  Serial.println(version_error->server_version);

  Supla::Network::Disconnect();

  waitForIterate = millis() + 5000;
}

void SuplaDeviceClass::onRegisterResult(
    TSD_SuplaRegisterDeviceResult *register_device_result) {
  _supla_int_t activity_timeout = 0;

  switch (register_device_result->result_code) {
    // OK scenario
    case SUPLA_RESULTCODE_TRUE:
      activity_timeout = register_device_result->activity_timeout;
      Supla::Network::Instance()->setActivityTimeout(activity_timeout);
      registered = 1;
      supla_log(LOG_DEBUG,
                "Device registered (activity timeout %d s, server version: %d, "
                "server min version: %d)",
                register_device_result->activity_timeout,
                register_device_result->version,
                register_device_result->version_min);
      lastIterateTime = millis();
      status(STATUS_REGISTERED_AND_READY, "Registered and ready");

      if (activity_timeout != ACTIVITY_TIMEOUT) {
        supla_log(
            LOG_DEBUG, "Changing activity timeout to %d", ACTIVITY_TIMEOUT);
        TDCS_SuplaSetActivityTimeout at;
        at.activity_timeout = ACTIVITY_TIMEOUT;
        srpc_dcs_async_set_activity_timeout(srpc, &at);
      }

      for (auto element = Supla::Element::begin(); element != nullptr;
           element = element->next()) {
        element->onRegistered();
        delay(0);
      }

      return;

      // NOK scenarios
    case SUPLA_RESULTCODE_TEMPORARILY_UNAVAILABLE:
      status(STATUS_TEMPORARILY_UNAVAILABLE, "Temporarily unavailable!", true);
      break;

    case SUPLA_RESULTCODE_GUID_ERROR:
      status(STATUS_INVALID_GUID, "Incorrect device GUID!", true);
      break;

    case SUPLA_RESULTCODE_AUTHKEY_ERROR:
      status(STATUS_INVALID_AUTHKEY, "Incorrect AuthKey!", true);
      break;

    case SUPLA_RESULTCODE_BAD_CREDENTIALS:
      status(STATUS_BAD_CREDENTIALS, "Bad credentials - incorrect AuthKey or email", true);
      break;

    case SUPLA_RESULTCODE_REGISTRATION_DISABLED:
      status(STATUS_REGISTRATION_DISABLED, "Registration disabled!", true);
      break;

    case SUPLA_RESULTCODE_DEVICE_LIMITEXCEEDED:
      status(STATUS_DEVICE_LIMIT_EXCEEDED, "Device limit exceeded!", true);
      break;

    case SUPLA_RESULTCODE_NO_LOCATION_AVAILABLE:
      status(STATUS_NO_LOCATION_AVAILABLE, "No location available!", true);
      break;

    case SUPLA_RESULTCODE_DEVICE_DISABLED:
      status(STATUS_DEVICE_IS_DISABLED, "Device is disabled!", true);
      break;

    case SUPLA_RESULTCODE_LOCATION_DISABLED:
      status(STATUS_LOCATION_IS_DISABLED, "Location is disabled!", true);
      break;

    case SUPLA_RESULTCODE_LOCATION_CONFLICT:
      status(STATUS_LOCATION_CONFLICT, "Location conflict!", true);
      break;

    case SUPLA_RESULTCODE_CHANNEL_CONFLICT:
      status(STATUS_CHANNEL_CONFLICT, "Channel conflict!", true);
      break;

    default:
      status(STATUS_UNKNOWN_ERROR, "Unknown registration error", true);
      supla_log(LOG_ERR,
                "Register result code %i",
                register_device_result->result_code);
      break;
  }

  Supla::Network::Disconnect();
  waitForIterate = millis() + 5000;
}

void SuplaDeviceClass::channelSetActivityTimeoutResult(
    TSDC_SuplaSetActivityTimeoutResult *result) {
  Supla::Network::Instance()->setActivityTimeout(result->activity_timeout);
  supla_log(
      LOG_DEBUG, "Activity timeout set to %d s", result->activity_timeout);
}

void SuplaDeviceClass::setServerPort(int value) {
  port = value;
}

void SuplaDeviceClass::setSwVersion(const char *swVersion) {
  setString(Supla::Channel::reg_dev.SoftVer, swVersion, SUPLA_SOFTVER_MAXSIZE);
}

int SuplaDeviceClass::getCurrentStatus() {
  return currentStatus;
}

void SuplaDeviceClass::fillStateData(TDSC_ChannelState &channelState) {
  channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_UPTIME |
                         SUPLA_CHANNELSTATE_FIELD_CONNECTIONUPTIME;

  channelState.Uptime = uptime.getUptime();
  channelState.ConnectionUptime = uptime.getConnectionUptime();
  if (uptime.getLastResetCause() > 0) {
    channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_LASTCONNECTIONRESETCAUSE;
    channelState.LastConnectionResetCause = uptime.getLastResetCause();
  }
}

void SuplaDeviceClass::setGUID(char GUID[SUPLA_GUID_SIZE]) {
  memcpy(Supla::Channel::reg_dev.GUID, GUID, SUPLA_GUID_SIZE);
}

void SuplaDeviceClass::setAuthKey(char authkey[SUPLA_AUTHKEY_SIZE]) {
  memcpy(Supla::Channel::reg_dev.AuthKey, authkey, SUPLA_AUTHKEY_SIZE);
}

void SuplaDeviceClass::setEmail(const char *email) {
  setString(Supla::Channel::reg_dev.Email, email, SUPLA_EMAIL_MAXSIZE);
}

void SuplaDeviceClass::setServer(const char *server) {
  setString(
      Supla::Channel::reg_dev.ServerName, server, SUPLA_SERVER_NAME_MAXSIZE);
}

void SuplaDeviceClass::onGetUserLocaltimeResult(
    TSDC_UserLocalTimeResult *result) {
  if (clock) {
    clock->parseLocaltimeFromServer(result);
  }
}

void SuplaDeviceClass::addClock(Supla::Clock *_clock) {
  Serial.println(F("Clock class added"));
  clock = _clock;
}

Supla::Clock *SuplaDeviceClass::getClock() {
  return clock;
}

SuplaDeviceClass SuplaDevice;
