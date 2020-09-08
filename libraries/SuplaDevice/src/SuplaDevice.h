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

#include <IPAddress.h>

#include "supla-common/proto.h"
#include "supla/network/network.h"
#include "supla/uptime.h"

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

typedef void (*_impl_arduino_timer)(void);

typedef struct SuplaChannelPin {
  int pin1;
  int pin2;
  bool hiIsLo;
  bool bistable;

  unsigned long time_left;
  unsigned long bi_time_left;
  unsigned long vc_time;

  uint8_t last_val;
  double last_val_dbl1;
  double last_val_dbl2;
};

typedef struct SuplaDeviceRollerShutterTask {
  uint8_t percent;
  uint8_t direction;
  bool active;
};

typedef struct SuplaDeviceRollerShutterCVR {
  uint8_t active;
  uint8_t value;
  unsigned long time;
};

typedef struct {
  int pin;
  uint8_t value;
  unsigned long time;
} SuplaDeviceRollerShutterButton;

typedef struct SuplaDeviceRollerShutter {
  SuplaDeviceRollerShutterButton btnUp;
  SuplaDeviceRollerShutterButton btnDown;

  int position;
  int last_position;
  int channel_number;
  unsigned int full_opening_time;
  unsigned int full_closing_time;

  unsigned long last_iterate_time;
  unsigned long tick_1s;
  unsigned long up_time;
  unsigned long down_time;

  unsigned long start_time;
  unsigned long stop_time;

  SuplaDeviceRollerShutterCVR cvr1;  // Change Value Request 1
  SuplaDeviceRollerShutterCVR cvr2;

  SuplaDeviceRollerShutterTask task;
  uint8_t save_position;
};

class SuplaDeviceClass {
 protected:
  void *srpc;
  char registered;
  SuplaChannelPin *channel_pin;
  int channel_pin_count;
  int port;
  int connectionFailCounter;
  int networkIsNotReadyCounter;
  SuplaDeviceRollerShutter *roller_shutter;

  unsigned long last_iterate_time;
  unsigned long wait_for_iterate;

  _impl_arduino_status impl_arduino_status;
  int currentStatus;

  Supla::Uptime uptime;

  _impl_arduino_timer impl_arduino_timer;


  bool isInitialized(bool msg);
  void setString(char *dst, const char *src, int max_size);
  int addChannel(int pin1, int pin2, bool hiIsLo, bool bistable);
  void channelSetValue(int channel, char value, _supla_int_t DurationMS);


  SuplaDeviceRollerShutter *rsByChannelNumber(int channel_number);

  void iterate_impulse_counter(SuplaChannelPin *pin,
                               TDS_SuplaDeviceChannel_C *channel,
                               unsigned long time_diff,
                               int channel_number);


 private:
  void status(int status, const char *msg);

 public:
  SuplaDeviceClass();
  ~SuplaDeviceClass();

  void fillStateData(TDSC_ChannelState &channelState);

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

  // Adds impulse couner on "impulsePin" pin. "statusLedPin" is not implemented
  // currently. "detectLowToHigh" defines if counter counts changes from LOW to
  // HIGH state on impulsePin. With "false" it counts changes from HIGH to LOW
  // "inputPullup" defines if impulsePin is configured as "INPUT_PULLUP" or
  // "INPUT" "debounceDelay" defines how many ms is used to filter out bouncing
  // changes between LOW and HIGH during change on pin state
  bool addImpulseCounter(int impulsePin,
                         int statusLedPin = 0,
                         bool detectLowToHigh = false,
                         bool inputPullup = true,
                         unsigned long debounceDelay = 10);

  // Timer with 100 Hz frequency (10 ms)
  void onTimer(void);
  // TImer with 2000 Hz frequency (0.5 ms)
  void onFastTimer(void);
  void iterate(void);

  void setStatusFuncImpl(_impl_arduino_status impl_arduino_status);
  void setTimerFuncImpl(_impl_arduino_timer impl_arduino_timer);
  void setServerPort(int value);

  void onVersionError(TSDC_SuplaVersionError *version_error);
  void onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result);
  void channelSetValueByServer(TSD_SuplaChannelNewValue *new_value);
  void channelSetActivityTimeoutResult(
      TSDC_SuplaSetActivityTimeoutResult *result);

  void setSwVersion(const char *);
  int getCurrentStatus();
};

extern SuplaDeviceClass SuplaDevice;
#endif
