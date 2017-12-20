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

#include "proto.h"
#include <IPAddress.h>

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

typedef _supla_int_t (*_cb_arduino_rw)(void *buf, _supla_int_t count);
typedef void (*_cb_arduino_eth_setup)(uint8_t mac[6], IPAddress *ip);
typedef bool (*_cb_arduino_connect)(const char *server, _supla_int_t port);
typedef bool (*_cb_arduino_connected)(void);
typedef void (*_cb_arduino_stop)(void);
typedef double (*_cb_arduino_get_temperature)(int channelNumber, double last_val);
typedef void (*_cb_arduino_get_temperature_and_humidity)(int channelNumber, double *temp, double *humidity);
typedef void (*_cb_arduino_get_rgbw_value)(int channelNumber, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *color_brightness, unsigned char *brightness);
typedef void (*_cb_arduino_set_rgbw_value)(int channelNumber, unsigned char red, unsigned char green, unsigned char blue, unsigned char color_brightness, unsigned char brightness);
typedef double (*_cb_arduino_get_distance)(int channelNumber, double distance);
typedef int (*_impl_arduino_digitalRead)(int channelNumber, uint8_t pin);
typedef void (*_impl_arduino_digitalWrite)(int channelNumber, uint8_t pin, uint8_t val);
typedef void (*_impl_arduino_status)(int status, const char *msg);

typedef struct SuplaDeviceCallbacks {
	
	_cb_arduino_rw tcp_read;
	_cb_arduino_rw tcp_write;
	_cb_arduino_eth_setup eth_setup;
	_cb_arduino_connected svr_connected;
	_cb_arduino_connect svr_connect;
	_cb_arduino_stop svr_disconnect;
	_cb_arduino_get_temperature get_temperature;
	_cb_arduino_get_temperature_and_humidity get_temperature_and_humidity;
	_cb_arduino_get_rgbw_value get_rgbw_value;
	_cb_arduino_set_rgbw_value set_rgbw_value;
    _cb_arduino_get_distance get_distance;
	
}SuplaDeviceCallbacks;

typedef struct SuplaDeviceParams {
	
	bool use_local_ip;
	IPAddress local_ip;
	
	SuplaDeviceCallbacks cb;
	TDS_SuplaRegisterDevice_B reg_dev;
	uint8_t mac[6];
	char *server;
	
}SuplaDeviceParams;

typedef struct SuplaChannelPin {
	int pin1;
	int pin2;
	bool hiIsLo;
	bool bistable;
	
	unsigned long time_left;
	unsigned long bi_time_left;
	unsigned long vc_time;
	
	union {
		uint8_t last_val;
		double last_val_dbl1;
		double last_val_dbl2;
	};
};

typedef struct SuplaDevicePrefs {
    
    byte tag[6];
    
};

typedef struct SuplaDeviceRollerShutter {
    int channel_idx;
};


class SuplaDeviceClass
{
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
	void channelSetTempAndHumidityValue(int channelNum, double temp, double humidity);
	void setRGBWvalue(int channelNum, char value[SUPLA_CHANNELVALUE_SIZE]);
	void channelSetRGBWvalue(int channel, char value[SUPLA_CHANNELVALUE_SIZE]);
    
    bool chceckEepromSize();
    bool prefsRead(bool init);
    bool prefsWrite(void);
    
	SuplaDeviceParams Params;
	_supla_int_t server_activity_timeout, last_response;
	SuplaChannelPin *channel_pin;
    
    int rs_count;
    SuplaDeviceRollerShutter *roller_shutter;

	unsigned long last_iterate_time;
    unsigned long wait_for_iterate;
	bool ping_flag;
    
    int eeprom_address;
    SuplaDevicePrefs prefs;
    
	_impl_arduino_digitalRead impl_arduino_digitalRead;
	_impl_arduino_digitalWrite impl_arduino_digitalWrite;
    _impl_arduino_status impl_arduino_status;

    void iterate_relay(SuplaChannelPin *pin, TDS_SuplaDeviceChannel_B *channel, unsigned long time_diff, int channel_idx);
    void iterate_sensor(SuplaChannelPin *pin, TDS_SuplaDeviceChannel_B *channel, unsigned long time_diff, int channel_idx);
    void iterate_thermometer(SuplaChannelPin *pin, TDS_SuplaDeviceChannel_B *channel, unsigned long time_diff, int channel_idx);
    void iterate_rollershutter(SuplaDeviceRollerShutter *rs, SuplaChannelPin *pin, TDS_SuplaDeviceChannel_B *channel);
    
private:
	int suplaDigitalRead(int channelNumber, uint8_t pin);
    bool suplaDigitalRead_isHI(int channelNumber, uint8_t pin);
	void suplaDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);
    void suplaDigitalWrite_setHI(int channelNumber, uint8_t pin, bool hi);
    void status(int status, const char *msg);
public:
   SuplaDeviceClass();
   ~SuplaDeviceClass();
   
   void channelValueChanged(int channel_number, char v);
   void channelDoubleValueChanged(int channel_number, double v);
    
   bool begin(IPAddress *local_ip, char GUID[SUPLA_GUID_SIZE], uint8_t mac[6], const char *Server,
		      int LocationID, const char *LocationPWD);
   
   bool begin(char GUID[SUPLA_GUID_SIZE], uint8_t mac[6], const char *Server,
		      int LocationID, const char *LocationPWD);
   
   void setName(const char *Name);
   
   int addRelay(int relayPin1, int relayPin2, bool hiIsLo, bool bistable, _supla_int_t functions);
   bool addRelay(int relayPin1, int relayPin2, bool hiIsLo);
   bool addRelay(int relayPin1, bool hiIsLo);
   bool addRelay(int relayPin1);
   bool addRollerShutterRelays(int relayPin1, int relayPin2, bool hiIsLo);
   bool addRollerShutterRelays(int relayPin1, int relayPin2);
   bool addSensorNO(int sensorPin, bool pullUp);
   bool addSensorNO(int sensorPin);
   bool addDS18B20Thermometer(void);
   bool addDHT11(void);
   bool addDHT22(void);
   bool addAM2302(void);
   bool addRgbControllerAndDimmer(void);
   bool addRgbController(void);
   bool addDimmer(void);
   bool addDistanceSensor(void);
    
   void setEepromAddress(int address);
    
   bool relayOn(int channel_number, _supla_int_t DurationMS);
   bool relayOff(int channel_number);
    
   bool rollerShutterReveal(int channel_number);
   bool rollerShutterShut(int channel_number);
   bool rollerShutterStop(int channel_number);
    
   void iterate(void);
   
   SuplaDeviceCallbacks getCallbacks(void);
   void setTemperatureCallback(_cb_arduino_get_temperature get_temperature);
   void setTemperatureHumidityCallback(_cb_arduino_get_temperature_and_humidity get_temperature_and_humidity);
   void setRGBWCallbacks(_cb_arduino_get_rgbw_value get_rgbw_value, _cb_arduino_set_rgbw_value set_rgbw_value);
   void setDistanceCallback(_cb_arduino_get_distance get_distance);
   
   void setDigitalReadFuncImpl(_impl_arduino_digitalRead impl_arduino_digitalRead);
   void setDigitalWriteFuncImpl(_impl_arduino_digitalWrite impl_arduino_digitalWrite);
   void setStatusFuncImpl(_impl_arduino_status impl_arduino_status);
    
   void onResponse(void);
   void onVersionError(TSDC_SuplaVersionError *version_error);
   void onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result);
   void onSensorInterrupt(void);
   void channelSetValue(TSD_SuplaChannelNewValue *new_value);
   void channelSetActivityTimeoutResult(TSDC_SuplaSetActivityTimeoutResult *result);
};

#include "supla_main_helper._cpp_"			
			
extern SuplaDeviceClass SuplaDevice;
extern SuplaDeviceCallbacks supla_arduino_get_callbacks(void);
#endif
