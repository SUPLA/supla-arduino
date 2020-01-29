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

#define ACTIVITY_TIMEOUT 30

#define STATUS_ALREADY_INITIALIZED     2
#define STATUS_CB_NOT_ASSIGNED         3
#define STATUS_INVALID_GUID            4
#define STATUS_UNKNOWN_SERVER_ADDRESS  5
#define STATUS_UNKNOWN_LOCATION_ID     6
#define STATUS_INITIALIZED             7
#define STATUS_CHANNEL_LIMIT_EXCEEDED  8
#define STATUS_DISCONNECTED            9
#define STATUS_REGISTER_IN_PROGRESS    10
#define STATUS_ITERATE_FAIL            11
#define STATUS_PROTOCOL_VERSION_ERROR  12
#define STATUS_BAD_CREDENTIALS         13
#define STATUS_TEMPORARILY_UNAVAILABLE 14
#define STATUS_LOCATION_CONFLICT       15
#define STATUS_CHANNEL_CONFLICT        16
#define STATUS_REGISTERED_AND_READY    17
#define STATUS_DEVICE_IS_DISABLED      18
#define STATUS_LOCATION_IS_DISABLED    19
#define STATUS_DEVICE_LIMIT_EXCEEDED   20

typedef double (*_cb_arduino_get_double)(int channelNumber,
                                         double current_value);
typedef void (*_cb_arduino_get_temperature_and_humidity)(int channelNumber,
                                                         double *temp,
                                                         double *humidity);
typedef void (*_cb_arduino_get_rgbw_value)(int channelNumber,
                                           unsigned char *red,
                                           unsigned char *green,
                                           unsigned char *blue,
                                           unsigned char *color_brightness,
                                           unsigned char *brightness);
typedef void (*_cb_arduino_set_rgbw_value)(int channelNumber,
                                           unsigned char red,
                                           unsigned char green,
                                           unsigned char blue,
                                           unsigned char color_brightness,
                                           unsigned char brightness);
typedef void (*_impl_arduino_status)(int status, const char *msg);

typedef void (*_impl_rs_save_position)(int channelNumber, int position);
typedef void (*_impl_rs_load_position)(int channelNumber, int *position);
typedef void (*_impl_rs_save_settings)(int channelNumber,
                                       unsigned int full_opening_time,
                                       unsigned int full_closing_time);
typedef void (*_impl_rs_load_settings)(int channelNumber,
                                       unsigned int *full_opening_time,
                                       unsigned int *full_closing_time);

typedef void (*_impl_arduino_timer)(void);

typedef struct SuplaDeviceCallbacks {
  _cb_arduino_get_rgbw_value get_rgbw_value;
  _cb_arduino_set_rgbw_value set_rgbw_value;

} SuplaDeviceCallbacks;

typedef struct SuplaDeviceParams {
  SuplaDeviceCallbacks cb;
} SuplaDeviceParams;

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
  bool isInitialized(bool msg);
  void setString(char *dst, const char *src, int max_size);
  int addChannel(int pin1, int pin2, bool hiIsLo, bool bistable);
  void channelValueChanged(int channel_number, char v, double d, char var);
  void channelSetValue(int channel, char value, _supla_int_t DurationMS);
  void channelSetDoubleValue(int channelNum, double value);
  void setDoubleValue(char value[SUPLA_CHANNELVALUE_SIZE], double v);
  bool addDHT(int Type);
  void channelSetTempAndHumidityValue(int channelNum,
                                      double temp,
                                      double humidity);
  void setRGBWvalue(int channelNum, char value[SUPLA_CHANNELVALUE_SIZE]);
  void channelSetRGBWvalue(int channel, char value[SUPLA_CHANNELVALUE_SIZE]);

  SuplaDeviceParams Params;
  SuplaChannelPin *channel_pin;
  int channel_pin_count;

  int rs_count;
  SuplaDeviceRollerShutter *roller_shutter;

  SuplaDeviceRollerShutter *rsByChannelNumber(int channel_number);

  unsigned long last_iterate_time;
  unsigned long wait_for_iterate;

  _impl_arduino_status impl_arduino_status;

  _impl_rs_save_position impl_rs_save_position;
  _impl_rs_load_position impl_rs_load_position;
  _impl_rs_save_settings impl_rs_save_settings;
  _impl_rs_load_settings impl_rs_load_settings;

  _impl_arduino_timer impl_arduino_timer;

  void rs_save_position(SuplaDeviceRollerShutter *rs);
  void rs_load_position(SuplaDeviceRollerShutter *rs);
  void rs_save_settings(SuplaDeviceRollerShutter *rs);
  void rs_load_settings(SuplaDeviceRollerShutter *rs);
  void rs_cvr_processing(SuplaDeviceRollerShutter *rs,
                         SuplaChannelPin *pin,
                         SuplaDeviceRollerShutterCVR *cvr);
  void rs_set_relay(SuplaDeviceRollerShutter *rs,
                    SuplaChannelPin *pin,
                    uint8_t value,
                    bool cancel_task,
                    bool stop_delay);
  void rs_set_relay(int channel_number, uint8_t value);
  void rs_calibrate(SuplaDeviceRollerShutter *rs,
                    unsigned long full_time,
                    unsigned long time,
                    int dest_pos);
  void rs_move_position(SuplaDeviceRollerShutter *rs,
                        SuplaChannelPin *pin,
                        unsigned long full_time,
                        unsigned long *time,
                        bool up);
  bool rs_time_margin(unsigned long full_time, unsigned long time, uint8_t m);
  void rs_task_processing(SuplaDeviceRollerShutter *rs, SuplaChannelPin *pin);
  void rs_add_task(SuplaDeviceRollerShutter *rs, unsigned char percent);
  void rs_cancel_task(SuplaDeviceRollerShutter *rs);
  bool rs_button_released(SuplaDeviceRollerShutterButton *btn);
  void rs_buttons_processing(SuplaDeviceRollerShutter *rs);

  void iterate_relay(SuplaChannelPin *pin,
                     TDS_SuplaDeviceChannel_C *channel,
                     unsigned long time_diff,
                     int channel_idx);
  void iterate_sensor(SuplaChannelPin *pin,
                      TDS_SuplaDeviceChannel_C *channel,
                      unsigned long time_diff,
                      int channel_idx);
  void iterate_thermometer(SuplaChannelPin *pin,
                           TDS_SuplaDeviceChannel_C *channel,
                           unsigned long time_diff,
                           int channel_idx);
  void iterate_rollershutter(SuplaDeviceRollerShutter *rs,
                             SuplaChannelPin *pin,
                             TDS_SuplaDeviceChannel_C *channel);
  void iterate_impulse_counter(SuplaChannelPin *pin,
                               TDS_SuplaDeviceChannel_C *channel,
                               unsigned long time_diff,
                               int channel_number);

  void begin_thermometer(SuplaChannelPin *pin,
                         TDS_SuplaDeviceChannel_C *channel,
                         int channel_number);

 private:
  bool suplaDigitalRead_isHI(int channelNumber, uint8_t pin);
  void suplaDigitalWrite_setHI(int channelNumber, uint8_t pin, bool hi);
  void status(int status, const char *msg);

 public:
  SuplaDeviceClass();
  ~SuplaDeviceClass();

  void channelValueChanged(int channel_number, char v);
  void channelDoubleValueChanged(int channel_number, double v);

  bool begin(char GUID[SUPLA_GUID_SIZE],
             const char *Server,
             const char *email,
             char authkey[SUPLA_AUTHKEY_SIZE],
             unsigned char version = 10);

  void setName(const char *Name);

  int addRelay(int relayPin1,
               int relayPin2,
               bool hiIsLo,
               bool bistable,
               _supla_int_t functions);
  bool addRelay(int relayPin1, int relayPin2, bool hiIsLo);
  bool addRelay(int relayPin1, bool hiIsLo);
  bool addRelay(int relayPin1);
  bool addRollerShutterRelays(int relayPin1, int relayPin2, bool hiIsLo);
  bool addRollerShutterRelays(int relayPin1, int relayPin2);
  void setRollerShutterButtons(int channel_number,
                               int btnUpPin,
                               int btnDownPin);
  bool addSensorNO(int sensorPin, bool pullUp);
  bool addSensorNO(int sensorPin);
  bool addRgbControllerAndDimmer(void);
  bool addRgbController(void);
  bool addDimmer(void);
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

  bool relayOn(int channel_number, _supla_int_t DurationMS);
  bool relayOff(int channel_number);

  void rollerShutterReveal(int channel_number);
  void rollerShutterShut(int channel_number);
  void rollerShutterStop(int channel_number);
  bool rollerShutterMotorIsOn(int channel_number);

  // Timer with 100 Hz frequency (10 ms)
  void onTimer(void);
  // TImer with 2000 Hz frequency (0.5 ms)
  void onFastTimer(void);
  void iterate(void);

  SuplaDeviceCallbacks getCallbacks(void);
  void setRGBWCallbacks(_cb_arduino_get_rgbw_value get_rgbw_value,
                        _cb_arduino_set_rgbw_value set_rgbw_value);
  void setRollerShutterFuncImpl(_impl_rs_save_position impl_save_position,
                                _impl_rs_load_position impl_load_position,
                                _impl_rs_save_settings impl_save_settings,
                                _impl_rs_load_settings impl_load_settings);

  void setStatusFuncImpl(_impl_arduino_status impl_arduino_status);
  void setTimerFuncImpl(_impl_arduino_timer impl_arduino_timer);

  void onVersionError(TSDC_SuplaVersionError *version_error);
  void onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result);
  void channelSetValueByServer(TSD_SuplaChannelNewValue *new_value);
  void channelSetActivityTimeoutResult(
      TSDC_SuplaSetActivityTimeoutResult *result);
};

extern SuplaDeviceClass SuplaDevice;
extern SuplaDeviceCallbacks supla_arduino_get_callbacks(void);
#endif
