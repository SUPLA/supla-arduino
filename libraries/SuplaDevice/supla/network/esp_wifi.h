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

#ifndef esp_wifi_h__
#define esp_wifi_h__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#ifdef ESP_SSL
#include <WiFiClientSecure.h>
#endif

#include "../../supla_lib_config.h"
#include "network.h"

#define MAX_SSID_SIZE          32
#define MAX_WIFI_PASSWORD_SIZE 64

#ifdef ESP_SSL
#define MAX_FINGERPRINT_SIZE 60
#endif

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

// TODO: change logs to supla_log

namespace Supla {
class ESPWifi : public Supla::Network {
 public:
   ESPWifi(const char *wifiSsid, const char *wifiPassword, IPAddress *ip = NULL)
       : Network(ip) {
		   
     strcpy(ssid, wifiSsid);
     strcpy(password, wifiPassword);
#ifdef ESP_SSL		
	 fingerprint = NULL;
#endif
  }
  
#ifdef ESP_SSL	  
  void setFingerprint(char* certFingerprint) {
    if (fingerprint) {
	  free(fingerprint);
	  fingerprint = NULL;
	}
	
	if (certFingerprint)
      fingerprint = strndup(certFingerprint, MAX_FINGERPRINT_SIZE);
  }
#endif
  
  int read(void *buf, int count) {
    _supla_int_t size = client.available();

    if (size > 0) {
      if (size > count) size = count;
      long readSize = client.read((uint8_t *)buf, size);
#ifdef SUPLA_COMM_DEBUG
      Serial.print("Received: [");
      for (int i = 0; i < readSize; i++) {
        Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
        Serial.print(" ");
      }
      Serial.println("]");
#endif

      return readSize;
    }
    return -1;
  }

  int write(void *buf, int count) {
#ifdef SUPLA_COMM_DEBUG
    Serial.print("Sending: [");
    for (int i = 0; i < count; i++) {
      Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
      Serial.print(" ");
    }
    Serial.println("]");
#endif
    long sendSize = client.write((const uint8_t *)buf, count);
    return sendSize;
  }

  bool connect(const char *server, int port) {
    
	bool result = client.connect(server, port);
	
#ifdef ESP_SSL
	/* server certificate validation */
	if (result && fingerprint)	{
	  if (client.verify(fingerprint, host)) {
#ifdef SUPLA_COMM_DEBUG
		Serial.println("certificate matches");
#endif
      } else {
		client.stop();
		result = false;
#ifdef SUPLA_COMM_DEBUG
        Serial.println("certificate doesn't match");
#endif
	  }
	}
#endif
	
	return result;
  }

  bool connected() {
    return client.connected();
  }

  bool isReady() {
    return WiFi.status() == WL_CONNECTED;
  }

  void disconnect() {
    client.stop();
  }

  // TODO: add handling of custom local ip
  void setup() {
    gotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
          Serial.print("local IP: ");
          Serial.println(WiFi.localIP());
          Serial.print("subnetMask: ");
          Serial.println(WiFi.subnetMask());
          Serial.print("gatewayIP: ");
          Serial.println(WiFi.gatewayIP());
          long rssi = WiFi.RSSI();
          Serial.print("Signal Strength (RSSI): ");
          Serial.print(rssi);
          Serial.println(" dBm");
        });
    disconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &event) {
          Serial.println("wifi Station disconnected");
        });
	
#ifdef ESP_SSL
	if (fingerprint)
      client.setFingerprint(fingerprint);
	else
	  client.insecure();
	}
#endif

    Serial.print("WIFI: establishing connection with SSID: \"");
    Serial.print(ssid);
    Serial.println("\"");

    WiFi.begin(ssid, password);
	yield();
  }
 protected:
#ifdef ESP_SSL
   WiFiClientSecure client;
#else
   WiFiClient client;
#endif
   char ssid[MAX_SSID_SIZE];
   char password[MAX_WIFI_PASSWORD_SIZE];
#ifdef ESP_SSL
   char* fingerprint;
#endif
};

};  // namespace Supla

#endif
