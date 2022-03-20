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

#include <esp_netif.h>
#include <fcntl.h>
#include <supla/supla_lib_config.h>
#include <nvs_flash.h>

#include <cstring>

#include "esp_idf_wifi.h"

static Supla::EspIdfWifi *netIntfPtr = nullptr;

extern const uint8_t suplaOrgCertPemStart[] asm(
    "_binary_supla_org_cert_pem_start");
extern const uint8_t suplaOrgCertPemEnd[] asm("_binary_supla_org_cert_pem_end");

Supla::EspIdfWifi::EspIdfWifi(const char *wifiSsid,
    const char *wifiPassword,
    unsigned char *ip)
  : Network(ip) {
    ssid[0] = '\0';
    password[0] = '\0';
    setSsid(wifiSsid);
    setPassword(wifiPassword);
    netIntfPtr = this;
  }

int Supla::EspIdfWifi::read(void *buf, int count) {
  esp_tls_conn_read(client, nullptr, 0);
  int tlsErr = 0;
  esp_tls_get_and_clear_last_error(client->error_handle, &tlsErr, nullptr);
  if (tlsErr != 0 && -tlsErr != ESP_TLS_ERR_SSL_WANT_READ &&
      -tlsErr != ESP_TLS_ERR_SSL_WANT_WRITE) {
    supla_log(LOG_ERR, "Connection error %d", tlsErr);
    Disconnect();
    return 0;
  }
  _supla_int_t size = esp_tls_get_bytes_avail(client);
  if (size < 0) {
    supla_log(LOG_ERR, "error in esp tls get bytes avail %d", size);
    Disconnect();
    return 0;
  }
  if (size > 0) {
    int ret = 0;
    do {
      ret = esp_tls_conn_read(client, buf, count);
      if (ret == ESP_TLS_ERR_SSL_WANT_READ ||
          ret == ESP_TLS_ERR_SSL_WANT_WRITE) {
        vTaskDelay(1);
        continue;
      }
      if (ret < 0) {
        supla_log(LOG_ERR, "esp_tls_conn_read  returned -0x%x", -ret);
        ret = 0;
        break;
      }
      if (ret == 0) {
        supla_log(LOG_INFO, "connection closed");
        Disconnect();
        return 0;
      }
      break;
    } while (1);

#ifdef SUPLA_COMM_DEBUG
    printData("Recv", buf, ret);
#endif
    return ret;
  }

  return -1;
}

int Supla::EspIdfWifi::write(void *buf, int count) {
#ifdef SUPLA_COMM_DEBUG
  printData("Send", buf, count);
#endif
  int sendSize = esp_tls_conn_write(client, buf, count);
  if (sendSize == 0) {
    isServerConnected = false;
  }
  return sendSize;
}

int Supla::EspIdfWifi::connect(const char *server, int port) {
  if (client != nullptr) {
    supla_log(LOG_ERR, "client ptr should be null when trying to connect");
    return 0;
  }

  esp_tls_cfg_t cfg = {};
  cfg.cacert_pem_buf = suplaOrgCertPemStart;
  cfg.cacert_pem_bytes =
    static_cast<unsigned int>(suplaOrgCertPemEnd - suplaOrgCertPemStart);
  cfg.timeout_ms = timeoutMs;
  cfg.non_block = false;

  int connectionPort = (isSecured ? 2016 : 2015);
  if (port != -1) {
    connectionPort = port;
  }

  supla_log(LOG_INFO,
      "Establishing connection with: %s (port: %d)",
      server,
      connectionPort);

  client = esp_tls_init();
  int result = esp_tls_conn_new_sync(
      server, strlen(server), connectionPort, &cfg, client);

  if (result == 1) {
    isServerConnected = true;
    int socketFd = 0;
    if (esp_tls_get_conn_sockfd(client, &socketFd) == ESP_OK) {
      fcntl(socketFd, F_SETFL, O_NONBLOCK);
    }

  } else {
   disconnect();
  }

  // SuplaDevice expects 1 on success, which is the same
  // as esp_tls_conn_new_sync returned values
  return result;
}

bool Supla::EspIdfWifi::connected() {
  return isServerConnected;
}

void Supla::EspIdfWifi::disconnect() {
  if (client != nullptr) {
    esp_tls_conn_delete(client);
    client = nullptr;
  }
  isServerConnected = false;
}

bool Supla::EspIdfWifi::isReady() {
  return isWifiConnected && isIpReady;
}

static void eventHandler(void *arg,
    esp_event_base_t eventBase,
    int32_t eventId,
    void *eventData) {
  if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) {
    supla_log(LOG_DEBUG, "Starting connection to AP");
    esp_wifi_connect();
  } else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_CONNECTED) {
    if (netIntfPtr) {
      netIntfPtr->setWifiConnected(true);
    }
    supla_log(LOG_DEBUG, "Connected to AP");
  } else if (eventBase == WIFI_EVENT &&
      eventId == WIFI_EVENT_STA_DISCONNECTED) {
    if (netIntfPtr) {
      netIntfPtr->setWifiConnected(false);
    }
    esp_wifi_connect();
    supla_log(LOG_DEBUG, "connect to the AP fail. Trying again");
  } else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = static_cast<ip_event_got_ip_t *>(eventData);
    if (netIntfPtr) {
      netIntfPtr->setIpReady(true);
      netIntfPtr->setIpv4Addr(event->ip_info.ip.addr);
    }
    supla_log(LOG_INFO, "got ip " IPSTR, IP2STR(&event->ip_info.ip));

  } else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_LOST_IP) {
    if (netIntfPtr) {
      netIntfPtr->setIpReady(false);
      netIntfPtr->setIpv4Addr(0);
    }
    supla_log(LOG_DEBUG, "lost ip");
  }
}

void Supla::EspIdfWifi::setup() {
  setWifiConnected(false);
  setIpReady(false);
  if (!initDone) {
    nvs_flash_init();
    esp_netif_init();
    wifiEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef SUPLA_DEVICE_ESP32
    esp_netif_create_default_wifi_sta();
#endif /*SUPLA_DEVICE_ESP32*/

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_event_handler_register(
          WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
          IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
          IP_EVENT, IP_EVENT_STA_LOST_IP, &eventHandler, NULL));
    esp_wifi_set_ps(WIFI_PS_NONE);
  } else {
    supla_log(LOG_DEBUG, "WiFi: resetting WiFi connection");
    disconnect();
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_stop());
  }

  initDone = true;

  supla_log(LOG_INFO, "WiFi: establishing connection with SSID: \"%s\"", ssid);
  wifi_config_t wifi_config = {};
  memcpy(wifi_config.sta.ssid, ssid, MAX_SSID_SIZE);
  memcpy(wifi_config.sta.password, password, MAX_WIFI_PASSWORD_SIZE);
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
  wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

  if (strlen((char *)(wifi_config.sta.password))) {
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void Supla::EspIdfWifi::enableSSL(bool value) {
  isSecured = value;
}

void Supla::EspIdfWifi::setSsid(const char *wifiSsid) {
  if (wifiSsid) {
    strncpy(ssid, wifiSsid, MAX_SSID_SIZE);
  }
}

void Supla::EspIdfWifi::setPassword(const char *wifiPassword) {
  if (wifiPassword) {
    strncpy(password, wifiPassword, MAX_WIFI_PASSWORD_SIZE);
  }
}

void Supla::EspIdfWifi::setTimeout(int newTimeoutMs) {
  if (newTimeoutMs > 0) {
    timeoutMs = newTimeoutMs;
  }
}

void Supla::EspIdfWifi::fillStateData(TDSC_ChannelState &channelState) {
  channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_IPV4 |
    SUPLA_CHANNELSTATE_FIELD_MAC |
    SUPLA_CHANNELSTATE_FIELD_WIFIRSSI |
    SUPLA_CHANNELSTATE_FIELD_WIFISIGNALSTRENGTH;

  esp_read_mac(channelState.MAC, ESP_MAC_WIFI_STA);
  channelState.IPv4 = ipv4;

  wifi_ap_record_t ap;
  esp_wifi_sta_get_ap_info(&ap);
  int rssi = ap.rssi;
  channelState.WiFiRSSI = rssi;
  if (rssi > -50) {
    channelState.WiFiSignalStrength = 100;
  } else if (rssi <= -100) {
    channelState.WiFiSignalStrength = 0;
  } else {
    channelState.WiFiSignalStrength = 2 * (rssi + 100);
  }
}

void Supla::EspIdfWifi::setIpReady(bool ready) {
  isIpReady = ready;
}

void Supla::EspIdfWifi::setWifiConnected(bool state) {
  isWifiConnected = state;
}

void Supla::EspIdfWifi::setIpv4Addr(unsigned _supla_int_t ip) {
  ipv4 = ip;
}
