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

#ifndef SUPLADEVICE_H
#define SUPLADEVICE_H

#include "supla-common/proto.h"
#include "supla/network/network.h"
#include "supla/uptime.h"
#include "supla/clock/clock.h"

#define ACTIVITY_TIMEOUT 30

#define STATUS_UNKNOWN                   -1
#define STATUS_ALREADY_INITIALIZED       2
#define STATUS_MISSING_NETWORK_INTERFACE 3
#define STATUS_INVALID_GUID              4
#define STATUS_UNKNOWN_SERVER_ADDRESS    5
#define STATUS_UNKNOWN_LOCATION_ID       6
#define STATUS_INITIALIZED               7
#define STATUS_CHANNEL_LIMIT_EXCEEDED    8
#define STATUS_SERVER_DISCONNECTED       9
#define STATUS_REGISTER_IN_PROGRESS      10
#define STATUS_ITERATE_FAIL              11
#define STATUS_PROTOCOL_VERSION_ERROR    12
#define STATUS_BAD_CREDENTIALS           13
#define STATUS_TEMPORARILY_UNAVAILABLE   14
#define STATUS_LOCATION_CONFLICT         15
#define STATUS_CHANNEL_CONFLICT          16
#define STATUS_REGISTERED_AND_READY      17
#define STATUS_DEVICE_IS_DISABLED        18
#define STATUS_LOCATION_IS_DISABLED      19
#define STATUS_DEVICE_LIMIT_EXCEEDED     20
#define STATUS_NETWORK_DISCONNECTED      21
#define STATUS_REGISTRATION_DISABLED     22
#define STATUS_MISSING_CREDENTIALS       23

typedef void (*_impl_arduino_status)(int status, const char *msg);

class SuplaDeviceClass {
 protected:
  void *srpc;
  char registered;
  int port;
  int connectionFailCounter;
  int networkIsNotReadyCounter;

  unsigned long last_iterate_time;
  unsigned long wait_for_iterate;

  _impl_arduino_status impl_arduino_status;
  int currentStatus;

  Supla::Uptime uptime;
  Supla::Clock *clock;

  bool isInitialized(bool msg);
  void setString(char *dst, const char *src, int max_size);

 private:
  void status(int status, const char *msg);

 public:
  SuplaDeviceClass();
  ~SuplaDeviceClass();

  void fillStateData(TDSC_ChannelState &channelState);
  void addClock(Supla::Clock *clock);
  Supla::Clock *getClock();

  bool begin(char GUID[SUPLA_GUID_SIZE],
             const char *Server,
             const char *email,
             char authkey[SUPLA_AUTHKEY_SIZE],
             unsigned char version = 12);

  bool begin(unsigned char version = 12);

  void setName(const char *Name);
  void setGUID(char GUID[SUPLA_GUID_SIZE]);
  void setAuthKey(char authkey[SUPLA_AUTHKEY_SIZE]);
  void setEmail(const char *email);
  void setServer(const char *server);

  // Timer with 100 Hz frequency (10 ms)
  void onTimer(void);
  // TImer with 2000 Hz frequency (0.5 ms)
  void onFastTimer(void);
  void iterate(void);

  void setStatusFuncImpl(_impl_arduino_status impl_arduino_status);
  void setServerPort(int value);

  void onVersionError(TSDC_SuplaVersionError *version_error);
  void onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result);
  void channelSetActivityTimeoutResult(
      TSDC_SuplaSetActivityTimeoutResult *result);
  void onGetUserLocaltimeResult(TSDC_UserLocalTimeResult *result);

  void setSwVersion(const char *);
  int getCurrentStatus();
};

extern SuplaDeviceClass SuplaDevice;
#endif
