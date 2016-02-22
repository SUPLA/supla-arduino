/*
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

 Author: Przemyslaw Zygmunt <przemek@supla.org>
*/

#ifndef SUPLADEVICE_H
#define SUPLADEVICE_H

#include "proto.h"

#define ACTIVITY_TIMEOUT 30

typedef _supla_int_t (*_cb_arduino_rw)(void *buf, _supla_int_t count);
typedef void (*_cb_arduino_eth_setup)(uint8_t mac[6]);
typedef bool (*_cb_arduino_connect)(const char *server, _supla_int_t port);
typedef bool (*_cb_arduino_connected)(void);
typedef void (*_cb_arduino_stop)(void);

typedef struct SuplaDeviceCallbacks {
	
	_cb_arduino_rw tcp_read;
	_cb_arduino_rw tcp_write;
	_cb_arduino_eth_setup eth_setup;
	_cb_arduino_connected svr_connected;
	_cb_arduino_connect svr_connect;
	_cb_arduino_stop svr_disconnect;

}SuplaDeviceCallbacks;

typedef struct SuplaDeviceParams {
	
	SuplaDeviceCallbacks cb;
	TDS_SuplaRegisterDevice_B reg_dev;
	uint8_t mac[6];
	char *server;
	
}SuplaDeviceParams;

typedef struct SuplaChannelPin {
	int pin1;
	int pin2;
	bool hiIsLo;
	
	_supla_int_t time_left;
	uint8_t last_val;
};

class SuplaDeviceClass
{
protected:
	void *srpc;
	char registered;
	bool isInitialized(bool msg);
	void setString(char *dst, const char *src, int max_size);
	int addChannel(int pin1, int pin2, bool hiIsLo);
	void channelValueChanged(int channel_number, char v);
	void channelSetValue(int channel, char value, _supla_int_t DurationMS);
	
	SuplaDeviceParams Params;
	_supla_int_t server_activity_timeout, last_response;
	SuplaChannelPin *channel_pin;

	unsigned long last_iterate_time;

	
public:
   SuplaDeviceClass();
   ~SuplaDeviceClass();
   void begin(char GUID[SUPLA_GUID_SIZE], uint8_t mac[6], const char *Server,
		      int LocationID, const char *LocationPWD);
   void setName(const char *Name);
   
   bool addRelay(int relayPin1, int relayPin2, bool hiIsLo, _supla_int_t functions);
   bool addRelay(int relayPin1, int relayPin2, bool hiIsLo);
   bool addRelay(int relayPin1, bool hiIsLo);
   bool addRelay(int relayPin1);
   bool addRollerShutterRelays(int relayPin1, int relayPin2, bool hiIsLo);
   bool addRollerShutterRelays(int relayPin1, int relayPin2);
   bool addSensorNO(int sensorPin);
   
   void iterate(void);
   
   SuplaDeviceCallbacks getCallbacks(void);
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
