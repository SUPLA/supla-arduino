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

#define SUPLADEVICE_CPP

#include <Arduino.h>
#include "IEEE754tools.h"
#include "SuplaDevice.h"
#include "srpc.h"
#include "log.h"


_supla_int_t supla_arduino_data_read(void *buf, _supla_int_t count, void *sdc) {
	return ((SuplaDeviceClass*)sdc)->getCallbacks().tcp_read(buf, count);
}

_supla_int_t supla_arduino_data_write(void *buf, _supla_int_t count, void *sdc) {
	return ((SuplaDeviceClass*)sdc)->getCallbacks().tcp_write(buf, count);
}



void supla_arduino_on_remote_call_received(void *_srpc, unsigned _supla_int_t rr_id, unsigned _supla_int_t call_type, void *_sdc, unsigned char proto_version) {

	TsrpcReceivedData rd;
	char result;

	((SuplaDeviceClass*)_sdc)->onResponse();

	if ( SUPLA_RESULT_TRUE == ( result = srpc_getdata(_srpc, &rd, 0)) ) {
		
		switch(rd.call_type) {
		case SUPLA_SDC_CALL_VERSIONERROR:
			((SuplaDeviceClass*)_sdc)->onVersionError(rd.data.sdc_version_error);
			break;
		case SUPLA_SD_CALL_REGISTER_DEVICE_RESULT:
			((SuplaDeviceClass*)_sdc)->onRegisterResult(rd.data.sd_register_device_result);
			break;
		case SUPLA_SD_CALL_CHANNEL_SET_VALUE:
			((SuplaDeviceClass*)_sdc)->channelSetValue(rd.data.sd_channel_new_value);
			break;
		case SUPLA_SDC_CALL_SET_ACTIVITY_TIMEOUT_RESULT:
			((SuplaDeviceClass*)_sdc)->channelSetActivityTimeoutResult(rd.data.sdc_set_activity_timeout_result);
			break;
		}

		srpc_rd_free(&rd);

	} else if ( result == SUPLA_RESULT_DATA_ERROR ) {

		supla_log(LOG_DEBUG, "DATA ERROR!");
	}
	
}


SuplaDeviceClass::SuplaDeviceClass() {

	char a;
	srpc = NULL;
	registered = 0;
	last_iterate_time = 0;
	channel_pin = NULL;
	
	impl_arduino_digitalRead = NULL;
	impl_arduino_digitalWrite = NULL;
	
	memset(&Params, 0, sizeof(SuplaDeviceParams));
	
	for(a=0;a<6;a++)
		Params.mac[a] = a;
	
	Params.cb = supla_arduino_get_callbacks();
}

SuplaDeviceClass::~SuplaDeviceClass() {
	if ( Params.server != NULL ) {
		free(Params.server);
		Params.server = NULL;
	}
	
	if ( channel_pin != NULL ) {
		free(channel_pin);
		channel_pin = NULL;
	}
	
}

int SuplaDeviceClass::suplaDigitalRead(int channelNumber, uint8_t pin) {
	
	if ( impl_arduino_digitalRead != NULL )
		return impl_arduino_digitalRead(channelNumber, pin);
	
	return digitalRead(pin);
}

void SuplaDeviceClass::suplaDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
	
	if ( impl_arduino_digitalWrite != NULL )
		 impl_arduino_digitalWrite(channelNumber, pin, val);
	
	digitalWrite(pin, val);
	
}

void SuplaDeviceClass::setDigitalReadFuncImpl(_impl_arduino_digitalRead impl_arduino_digitalRead) {
	
	this->impl_arduino_digitalRead = impl_arduino_digitalRead;
	
}

void SuplaDeviceClass::setDigitalWriteFuncImpl(_impl_arduino_digitalWrite impl_arduino_digitalWrite) {
	
	this->impl_arduino_digitalWrite = impl_arduino_digitalWrite;	
	
}

bool SuplaDeviceClass::isInitialized(bool msg) {
	if ( srpc != NULL ) {
		
		if ( msg )
		   supla_log(LOG_DEBUG, "SuplaDevice is already initialized");
		
		return true;
	}
	
	return false;
}

bool SuplaDeviceClass::begin(IPAddress *local_ip, char GUID[SUPLA_GUID_SIZE], uint8_t mac[6], const char *Server,
	                         int LocationID, const char *LocationPWD) {

	unsigned char a;
	if ( isInitialized(true) ) return false;
	
	if ( Params.cb.tcp_read == NULL
	     || Params.cb.tcp_write == NULL
	     || Params.cb.eth_setup == NULL
	     || Params.cb.svr_connected == NULL
	     || Params.cb.svr_connect == NULL
	     || Params.cb.svr_disconnect == NULL ) {
		
		supla_log(LOG_DEBUG, "Callbacks not assigned!");
		return false;
	}
	
	if ( local_ip ) {
		Params.local_ip = *local_ip;
		Params.use_local_ip = true;
	} else {
		Params.use_local_ip = false;
	}
	
	memcpy(Params.reg_dev.GUID, GUID, SUPLA_GUID_SIZE);
	memcpy(Params.mac, mac, 6);
	Params.reg_dev.LocationID = LocationID;
	setString(Params.reg_dev.LocationPWD, LocationPWD, SUPLA_LOCATION_PWD_MAXSIZE);
	Params.server = strdup(Server);
	
	for(a=0;a<SUPLA_GUID_SIZE;a++)
		if ( Params.reg_dev.GUID[a] != 0 ) break;
	
	if ( a == SUPLA_GUID_SIZE ) {
		supla_log(LOG_DEBUG, "Invalid GUID");
		return false;
	}
	
	if ( Params.server == NULL 
			|| Params.server[0] == NULL ) {
		supla_log(LOG_DEBUG, "Unknown server address");
		return false;
	}
	
	if ( Params.reg_dev.LocationID == 0 ) {
		supla_log(LOG_DEBUG, "Unknown LocationID");
		return false;
	}
	
	setString(Params.reg_dev.Name, "ARDUINO", SUPLA_DEVICE_NAME_MAXSIZE);
	setString(Params.reg_dev.SoftVer, "1.1", SUPLA_SOFTVER_MAXSIZE);
	
	Params.cb.eth_setup(Params.mac, Params.use_local_ip ? &Params.local_ip : NULL);

	TsrpcParams srpc_params;
	srpc_params_init(&srpc_params);
	srpc_params.data_read = &supla_arduino_data_read;
	srpc_params.data_write = &supla_arduino_data_write;
	srpc_params.on_remote_call_received = &supla_arduino_on_remote_call_received;
	srpc_params.user_params = this;
	

	srpc = srpc_init(&srpc_params);
	supla_log(LOG_DEBUG, "SuplaDevice initialized");
}

bool SuplaDeviceClass::begin(char GUID[SUPLA_GUID_SIZE], uint8_t mac[6], const char *Server,
	                         int LocationID, const char *LocationPWD) {
	
	return begin(NULL, GUID, mac, Server, LocationID, LocationPWD);
}

void SuplaDeviceClass::setName(const char *Name) {
	
	if ( isInitialized(true) ) return;
	setString(Params.reg_dev.Name, Name, SUPLA_DEVICE_NAME_MAXSIZE);
}

int SuplaDeviceClass::addChannel(int pin1, int pin2, bool hiIsLo, bool bistable) {
	if ( isInitialized(true) ) return -1;
	
	if ( Params.reg_dev.channel_count >= SUPLA_CHANNELMAXCOUNT ) {
		supla_log(LOG_DEBUG, "Channel limit exceeded");
		return -1;
	}
	
	if ( bistable && ( pin1 == 0 || pin2 == 0 ) )
		bistable = false;
	
	Params.reg_dev.channels[Params.reg_dev.channel_count].Number = Params.reg_dev.channel_count;
	channel_pin = (SuplaChannelPin*)realloc(channel_pin, sizeof(SuplaChannelPin)*(Params.reg_dev.channel_count+1));
	channel_pin[Params.reg_dev.channel_count].pin1 = pin1; 
	channel_pin[Params.reg_dev.channel_count].pin2 = pin2; 
	channel_pin[Params.reg_dev.channel_count].hiIsLo = hiIsLo;
	channel_pin[Params.reg_dev.channel_count].bistable = bistable;
	channel_pin[Params.reg_dev.channel_count].time_left = 0;
	channel_pin[Params.reg_dev.channel_count].vc_time = 0;
	channel_pin[Params.reg_dev.channel_count].bi_time_left = 0;
	channel_pin[Params.reg_dev.channel_count].last_val = suplaDigitalRead(Params.reg_dev.channel_count, bistable ? pin2 : pin1);
	
	Params.reg_dev.channel_count++;
	
	return Params.reg_dev.channel_count-1;
}

bool SuplaDeviceClass::addRelay(int relayPin1, int relayPin2, bool hiIsLo, bool bistable, _supla_int_t functions) {
	
	int c = addChannel(relayPin1, relayPin2, hiIsLo, bistable);
	if ( c == -1 ) return false; 
	
	uint8_t _HI = hiIsLo ? LOW : HIGH;
	uint8_t _LO = hiIsLo ? HIGH : LOW;
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_RELAY;
	Params.reg_dev.channels[c].FuncList = functions;
	
	if ( relayPin1 != 0 ) {
		pinMode(relayPin1, OUTPUT); 
		suplaDigitalWrite(Params.reg_dev.channels[c].Number, relayPin1, hiIsLo ? HIGH : LOW); 
		
		if ( bistable == false )
			Params.reg_dev.channels[c].value[0] = suplaDigitalRead(Params.reg_dev.channels[c].Number, relayPin1) == _HI ? 1 : 0;
	}

	if ( relayPin2 != 0 )
	  if ( bistable ) {
		  
		  pinMode(relayPin2, INPUT); 
		  Params.reg_dev.channels[c].value[0] = suplaDigitalRead(Params.reg_dev.channels[c].Number, relayPin2) == HIGH ? 1 : 0;
		  
	  } else {
		  pinMode(relayPin2, OUTPUT); 
		  suplaDigitalWrite(Params.reg_dev.channels[c].Number, relayPin2, hiIsLo ? HIGH : LOW); 
			
		  if ( Params.reg_dev.channels[c].value[0] == 0
				&& suplaDigitalRead(Params.reg_dev.channels[c].Number, relayPin2) == _HI )
			Params.reg_dev.channels[c].value[0] = 2;
	}

	
	return true;
}

bool SuplaDeviceClass::addRelay(int relayPin, bool hiIsLo) {
	return addRelay(relayPin, 0, hiIsLo, false, SUPLA_BIT_RELAYFUNC_CONTROLLINGTHEGATEWAYLOCK
                              | SUPLA_BIT_RELAYFUNC_CONTROLLINGTHEGATE
                              | SUPLA_BIT_RELAYFUNC_CONTROLLINGTHEGARAGEDOOR
                              | SUPLA_BIT_RELAYFUNC_CONTROLLINGTHEDOORLOCK
                              | SUPLA_BIT_RELAYFUNC_POWERSWITCH
                              | SUPLA_BIT_RELAYFUNC_LIGHTSWITCH);
}

bool SuplaDeviceClass::addRelay(int relayPin1) {
	return addRelay(relayPin1, false);
}

bool SuplaDeviceClass::addRollerShutterRelays(int relayPin1, int relayPin2, bool hiIsLo) {
	return addRelay(relayPin1, relayPin2, hiIsLo, false, SUPLA_BIT_RELAYFUNC_CONTROLLINGTHEROLLERSHUTTER);
}

bool SuplaDeviceClass::addRollerShutterRelays(int relayPin1, int relayPin2) {
	return addRollerShutterRelays(relayPin1, relayPin2, false);
}

bool SuplaDeviceClass::addSensorNO(int sensorPin, bool pullUp) {
	
	int c = addChannel(sensorPin, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_SENSORNO;
	pinMode(sensorPin, INPUT); 
	suplaDigitalWrite(Params.reg_dev.channels[c].Number, sensorPin, pullUp ? HIGH : LOW);
	
	Params.reg_dev.channels[c].value[0] = suplaDigitalRead(Params.reg_dev.channels[c].Number, sensorPin) == HIGH ? 1 : 0;
	
	return true;
}

void SuplaDeviceClass::setDoubleValue(char value[SUPLA_CHANNELVALUE_SIZE], double v) {
	
	if ( sizeof(double) == 8 ) {
		memcpy(value, &v, 8);
	} else if ( sizeof(double) == 4 ) {
		float2DoublePacked(v, (byte*)value);
	}

}

void SuplaDeviceClass::channelSetDoubleValue(int channelNum, double value) {
	setDoubleValue(Params.reg_dev.channels[channelNum].value, value);
}

void SuplaDeviceClass::channelSetTempAndHumidityValue(int channelNum, double temp, double humidity) {
	
	long t = temp*1000.00;
	long h = humidity*1000.00;
	
	memcpy(Params.reg_dev.channels[channelNum].value, &t, 4);
	memcpy(&Params.reg_dev.channels[channelNum].value[4], &h, 4);
}

void SuplaDeviceClass::setRGBWvalue(int channelNum, char value[SUPLA_CHANNELVALUE_SIZE]) {
	
	if ( Params.cb.get_rgbw_value ) {
		
		unsigned char red = 0;
		unsigned char green = 0xFF;
		unsigned char blue = 0;
		unsigned char color_brightness = 0;
		unsigned char brightness = 0;
		
		Params.cb.get_rgbw_value(Params.reg_dev.channels[channelNum].Number, &red, &green, &blue, &color_brightness, &brightness);
		
		value[0] = brightness;
		value[1] = color_brightness;
		
		value[2] = blue;
		value[3] = green;
		value[4] = red;
	}
	
}

bool SuplaDeviceClass::addDS18B20Thermometer(void) {
	
	int c = addChannel(0, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_THERMOMETERDS18B20;
	channel_pin[c].last_val_dbl1 = -275;	
	channelSetDoubleValue(c, channel_pin[c].last_val_dbl1);

}

bool SuplaDeviceClass::addDHT(int Type) {
	
	int c = addChannel(0, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = Type;
	channel_pin[c].last_val_dbl1 = -275;	
	channel_pin[c].last_val_dbl2 = -1;	
	
	channelSetTempAndHumidityValue(c, channel_pin[c].last_val_dbl1, channel_pin[c].last_val_dbl2);
	
}

bool SuplaDeviceClass::addDHT11(void) {
	return addDHT(SUPLA_CHANNELTYPE_DHT11);
}

bool SuplaDeviceClass::addDHT22(void) {
	return addDHT(SUPLA_CHANNELTYPE_DHT22);
}

bool SuplaDeviceClass::addAM2302(void) {
	return addDHT(SUPLA_CHANNELTYPE_AM2302);
}

bool SuplaDeviceClass::addRgbControllerAndDimmer(void) {
	
	int c = addChannel(0, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_DIMMERANDRGBLED;
	setRGBWvalue(c, Params.reg_dev.channels[c].value);
}

bool SuplaDeviceClass::addRgbController(void) {
	
	int c = addChannel(0, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_RGBLEDCONTROLLER;
	setRGBWvalue(c, Params.reg_dev.channels[c].value);
}

bool SuplaDeviceClass::addRgbDimmer(void) {
	
	int c = addChannel(0, 0, false, false);
	if ( c == -1 ) return false; 
	
	Params.reg_dev.channels[c].Type = SUPLA_CHANNELTYPE_DIMMER;
	setRGBWvalue(c, Params.reg_dev.channels[c].value);
}

bool SuplaDeviceClass::addSensorNO(int sensorPin) {
	return addSensorNO(sensorPin, false);
}

SuplaDeviceCallbacks SuplaDeviceClass::getCallbacks(void) {
	return Params.cb;
}


void SuplaDeviceClass::setString(char *dst, const char *src, int max_size) {
	
	if ( src == NULL ) {
		dst[0] = 0;
		return;
	}
	
	int size = strlen(src);
	
	if ( size+1 > max_size )
		size = max_size-1;
	
	memcpy(dst, src, size);
}

void SuplaDeviceClass::setTemperatureCallback(_cb_arduino_get_temperature get_temperature) {
	 Params.cb.get_temperature = get_temperature;
}

void SuplaDeviceClass::setTemperatureHumidityCallback(_cb_arduino_get_temperature_and_humidity get_temperature_and_humidity) {
	Params.cb.get_temperature_and_humidity = get_temperature_and_humidity;
}

void SuplaDeviceClass::setRGBWCallbacks(_cb_arduino_get_rgbw_value get_rgbw_value, _cb_arduino_set_rgbw_value set_rgbw_value) {
	Params.cb.get_rgbw_value = get_rgbw_value;
	Params.cb.set_rgbw_value = set_rgbw_value;
}

void SuplaDeviceClass::iterate(void) {
	
	if ( !isInitialized(false) ) return;
	
	if ( !Params.cb.svr_connected() ) {
		
		supla_log(LOG_DEBUG, "Not connected");
	    registered = 0;
	    last_response = 0;
	    ping_flag = false;
	    
		if ( !Params.cb.svr_connect(Params.server, 2015) ) {
			
		    	supla_log(LOG_DEBUG, "Connection fail. Server: %s", Params.server);
		    	Params.cb.svr_disconnect();
				delay(2000);
				return;
		}
	}
	
	unsigned long _millis = millis();

	
	if ( registered == 0 ) {
		
		registered = -1;
		srpc_ds_async_registerdevice_b(srpc, &Params.reg_dev);
		supla_log(LOG_DEBUG, "Register in progress");
		
	} else if ( registered == 1 ) {
		// PING
		if ( (_millis-last_response)/1000 >= (server_activity_timeout+10)  ) {
			
			supla_log(LOG_DEBUG, "TIMEOUT");
			Params.cb.svr_disconnect();

		} else if ( ping_flag == false 
				    && (_millis-last_response)/1000 >= (server_activity_timeout-5) ) {
			ping_flag = true;
			srpc_dcs_async_ping_server(srpc);
		}
	}
	
	if ( last_iterate_time != 0 ) {
		
		unsigned long td = abs(_millis - last_iterate_time);
		
		for(int a=0;a<Params.reg_dev.channel_count;a++) {
			
			if ( channel_pin[a].bi_time_left != 0 ) {
				if ( td >= channel_pin[a].bi_time_left ) {
										
					suplaDigitalWrite(Params.reg_dev.channels[a].Number, channel_pin[a].pin1, channel_pin[a].hiIsLo ? HIGH : LOW); 
					channel_pin[a].bi_time_left = 0;
					
				} else if ( channel_pin[a].bi_time_left > 0 ) {
					channel_pin[a].bi_time_left-=td;
				}
			}
			
			if ( channel_pin[a].time_left != 0 ) {
				if ( td >= channel_pin[a].time_left ) {
					
					channel_pin[a].time_left = 0;
					
					if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_SENSORNO ) 
						channel_pin[a].last_val = -1;
					
					else if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_RELAY )
						channelSetValue(a, 0, 0);
					
				} else if ( channel_pin[a].time_left > 0 ) {
					channel_pin[a].time_left-=td;
				}
			}
			
			if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_RELAY 
				 && channel_pin[a].bistable	)
				if ( td >= channel_pin[a].vc_time ) {
					
					channel_pin[a].vc_time-=td;
					
				} else {

				uint8_t val = suplaDigitalRead(Params.reg_dev.channels[a].Number, channel_pin[a].pin2);
				
				if ( val != channel_pin[a].last_val ) {
					
					channel_pin[a].last_val = val;	
					channel_pin[a].vc_time = 200;
					
					channelValueChanged(Params.reg_dev.channels[a].Number, val == HIGH ? 1 : 0); 
						
				}		
			}
			
			if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_SENSORNO ) {
				
				uint8_t val = suplaDigitalRead(Params.reg_dev.channels[a].Number, channel_pin[a].pin1);
				
				if ( val != channel_pin[a].last_val ) {
					
					channel_pin[a].last_val = val;
					
					if ( channel_pin[a].time_left <= 0 ) {
						channel_pin[a].time_left = 500;
						channelValueChanged(Params.reg_dev.channels[a].Number, val == HIGH ? 1 : 0); 
					}
						
				}		
			}
			
			if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_THERMOMETERDS18B20
				 && Params.cb.get_temperature != NULL ) {
				
				
				if ( channel_pin[a].time_left <= 0 ) {
				
					channel_pin[a].time_left = 10000;
					double val = Params.cb.get_temperature(a, channel_pin[a].last_val_dbl1);
					
					if ( val != channel_pin[a].last_val_dbl1 ) {
						channel_pin[a].last_val_dbl1 = val;
						channelDoubleValueChanged(a, val); 	
					}
										
				}		
			}
			
			
			if ( ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_DHT11
				   || Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_DHT22
				   || Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_AM2302 )
				 && Params.cb.get_temperature_and_humidity != NULL ) {
				
				
				if ( channel_pin[a].time_left <= 0 ) {
				
					channel_pin[a].time_left = 10000;
					
					double t = channel_pin[a].last_val_dbl1;
					double h = channel_pin[a].last_val_dbl2;
					
					Params.cb.get_temperature_and_humidity(a, &t, &h);
					
					if ( t != channel_pin[a].last_val_dbl1 
							|| h != channel_pin[a].last_val_dbl2 ) {
						
						channel_pin[a].last_val_dbl1 = t;
						channel_pin[a].last_val_dbl2 = h;
						
						channelSetTempAndHumidityValue(a, t, h);
						srpc_ds_async_channel_value_changed(srpc, a, Params.reg_dev.channels[a].value);
					}
										
				}		
			}
			
 			
		}

	}

	
	last_iterate_time = millis();
	
	if( srpc_iterate(srpc) == SUPLA_RESULT_FALSE ) {
		supla_log(LOG_DEBUG, "Iterate fail");
		Params.cb.svr_disconnect();
		delay(5000);
	}
	
	
}

void SuplaDeviceClass::onResponse(void) {
	last_response = millis();
	ping_flag = false;
}

void SuplaDeviceClass::onVersionError(TSDC_SuplaVersionError *version_error) {
	supla_log(LOG_ERR, "Protocol version error");
	Params.cb.svr_disconnect();
	delay(5000);
}

void SuplaDeviceClass::onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result) {

	switch(register_device_result->result_code) {
	case SUPLA_RESULTCODE_BAD_CREDENTIALS:
		supla_log(LOG_ERR, "Bad credentials!");
		break;

	case SUPLA_RESULTCODE_TEMPORARILY_UNAVAILABLE:
		supla_log(LOG_NOTICE, "Temporarily unavailable!");
		break;

	case SUPLA_RESULTCODE_LOCATION_CONFLICT:
		supla_log(LOG_ERR, "Location conflict!");
		break;

	case SUPLA_RESULTCODE_CHANNEL_CONFLICT:
		supla_log(LOG_ERR, "Channel conflict!");
		break;
	case SUPLA_RESULTCODE_TRUE:

		server_activity_timeout = register_device_result->activity_timeout;
		registered = 1;

		supla_log(LOG_DEBUG, "Registered and ready.");

		if ( server_activity_timeout != ACTIVITY_TIMEOUT ) {

			TDCS_SuplaSetActivityTimeout at;
			at.activity_timeout = ACTIVITY_TIMEOUT;
			srpc_dcs_async_set_activity_timeout(srpc, &at);

		}

		return;

	case SUPLA_RESULTCODE_DEVICE_DISABLED:
		supla_log(LOG_NOTICE, "Device is disabled!");
		break;

	case SUPLA_RESULTCODE_LOCATION_DISABLED:
		supla_log(LOG_NOTICE, "Location is disabled!");
		break;

	case SUPLA_RESULTCODE_DEVICE_LIMITEXCEEDED:
		supla_log(LOG_NOTICE, "Device limit exceeded!");
		break;

	case SUPLA_RESULTCODE_GUID_ERROR:
		supla_log(LOG_NOTICE, "Incorrect device GUID!");
		break;
	}

	Params.cb.svr_disconnect();
	delay(5000);
}

void SuplaDeviceClass::channelValueChanged(int channel_number, char v, double d, char var) {

	if ( srpc != NULL
		 && registered == 1 ) {

		char value[SUPLA_CHANNELVALUE_SIZE];
		memset(value, 0, SUPLA_CHANNELVALUE_SIZE);
		
		if ( var == 1 )
			value[0] = v;
		else if ( var == 2 ) 
			setDoubleValue(value, d);

		srpc_ds_async_channel_value_changed(srpc, channel_number, value);
	}

}

void SuplaDeviceClass::channelDoubleValueChanged(int channel_number, double v) {
	channelValueChanged(channel_number, 0, v, 2);
	
}

void SuplaDeviceClass::channelValueChanged(int channel_number, char v) {

	channelValueChanged(channel_number, v, 0, 1);

}

void SuplaDeviceClass::channelSetValue(int channel, char value, _supla_int_t DurationMS) {
	
	bool success = false;
	
	uint8_t _HI = channel_pin[channel].hiIsLo ? LOW : HIGH;
	uint8_t _LO = channel_pin[channel].hiIsLo ? HIGH : LOW;

	if ( Params.reg_dev.channels[channel].Type == SUPLA_CHANNELTYPE_RELAY ) {
		
		if ( channel_pin[channel].bistable ) 
		   if ( channel_pin[channel].bi_time_left > 0
				 || suplaDigitalRead(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2)  == value ) {
			   value = -1;
		   } else {
			   value = 1;
			   channel_pin[channel].bi_time_left = 500;
		   }
		
		if ( value == 0 ) {
			
			if ( channel_pin[channel].pin1 != 0 ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin1, _LO); 
				
				success = suplaDigitalRead(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin1) == _LO;
			}
				

			if ( channel_pin[channel].pin2 != 0 
					&& channel_pin[channel].bistable == false ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2, _LO); 
				
				if ( !success )
					success = suplaDigitalRead(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2) == _LO;
			}
				
			
		} else if ( value == 1 ) {
			
			if ( channel_pin[channel].pin2 != 0
					&& channel_pin[channel].bistable == false ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2, _LO); 
				delay(50);
			}
			
			if ( channel_pin[channel].pin1 != 0 ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin1, _HI); 
				
				if ( !success )
					success = suplaDigitalRead(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin1) == _HI;
				
				if ( DurationMS > 0 )
					channel_pin[channel].time_left = DurationMS;
			}
				
			
			
		} else if ( value == 2 
				    && channel_pin[channel].bistable == false ) {
			
			if ( channel_pin[channel].pin1 != 0 ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin1, _LO); 
				delay(50);
			}
			
			if ( channel_pin[channel].pin2 != 0  ) {
				suplaDigitalWrite(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2, _HI); 
				
				if ( !success )
					success = suplaDigitalRead(Params.reg_dev.channels[channel].Number, channel_pin[channel].pin2) == _HI;
			}
				
		}
			
		if ( channel_pin[channel].bistable ) {
			success = false;
			delay(50);
		}
		
	};

	if ( success
			&& registered == 1 
			&& srpc ) {
		channelValueChanged(Params.reg_dev.channels[channel].Number, value);
	}

	
}

void SuplaDeviceClass::channelSetRGBWvalue(int channel, char value[SUPLA_CHANNELVALUE_SIZE]) {
	
	unsigned char red = (unsigned char)value[4];
	unsigned char green = (unsigned char)value[3];
	unsigned char blue = (unsigned char)value[2];
	char color_brightness = (unsigned char)value[1];
	char brightness = (unsigned char)value[0];
	
	Params.cb.set_rgbw_value(Params.reg_dev.channels[channel].Number, red, green, blue, color_brightness, brightness);
	
	if ( srpc != NULL
		 && registered == 1 ) {

		char value[SUPLA_CHANNELVALUE_SIZE];
		memset(value, 0, SUPLA_CHANNELVALUE_SIZE);
		
		setRGBWvalue(channel, value);

		srpc_ds_async_channel_value_changed(srpc, Params.reg_dev.channels[channel].Number, value);
	}
	
}

void SuplaDeviceClass::channelSetValue(TSD_SuplaChannelNewValue *new_value) {

	for(int a=0;a<Params.reg_dev.channel_count;a++) 
		if ( new_value->ChannelNumber == Params.reg_dev.channels[a].Number ) {
			
			if ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_RELAY ) {
				
				channelSetValue(new_value->ChannelNumber, new_value->value[0], new_value->DurationMS);
				
			} else if ( ( Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_DIMMER
						   || Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER
						   || Params.reg_dev.channels[a].Type == SUPLA_CHANNELTYPE_DIMMERANDRGBLED )
							&& Params.cb.set_rgbw_value ) {
				
				channelSetRGBWvalue(a, new_value->value);
			}
			break;
		}

}

void SuplaDeviceClass::channelSetActivityTimeoutResult(TSDC_SuplaSetActivityTimeoutResult *result) {
	server_activity_timeout = result->activity_timeout;
}

SuplaDeviceClass SuplaDevice;
