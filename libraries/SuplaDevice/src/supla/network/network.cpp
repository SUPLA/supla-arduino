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

#include "SuplaDevice.h"
#include "supla-common/log.h"
#include "supla-common/srpc.h"
#include "supla/element.h"
#include "supla/network/network.h"

namespace Supla {

Network *Network::netIntf = NULL;

_supla_int_t data_read(void *buf, _supla_int_t count, void *sdc) {
  return Supla::Network::Read(buf, count);
}

_supla_int_t data_write(void *buf, _supla_int_t count, void *sdc) {
  _supla_int_t r = Supla::Network::Write(buf, count);
  if (r > 0) {
    Network::Instance()->updateLastSent();
  }
  return r;
}

void message_received(void *_srpc,
                      unsigned _supla_int_t rr_id,
                      unsigned _supla_int_t call_type,
                      void *_sdc,
                      unsigned char proto_version) {
  TsrpcReceivedData rd;
  char result;

  Network::Instance()->updateLastResponse();

  if (SUPLA_RESULT_TRUE == (result = srpc_getdata(_srpc, &rd, 0))) {
    switch (rd.call_type) {
      case SUPLA_SDC_CALL_VERSIONERROR:
        ((SuplaDeviceClass *)_sdc)->onVersionError(rd.data.sdc_version_error);
        break;
      case SUPLA_SD_CALL_REGISTER_DEVICE_RESULT:
        ((SuplaDeviceClass *)_sdc)
            ->onRegisterResult(rd.data.sd_register_device_result);
        break;
      case SUPLA_SD_CALL_CHANNEL_SET_VALUE: {
        auto element = Supla::Element::getElementByChannelNumber(
            rd.data.sd_channel_new_value->ChannelNumber);
        if (element) {
          int actionResult =
              element->handleNewValueFromServer(rd.data.sd_channel_new_value);
          if (actionResult != -1) {
            srpc_ds_async_set_channel_result(
                _srpc,
                rd.data.sd_channel_new_value->ChannelNumber,
                rd.data.sd_channel_new_value->SenderID,
                actionResult);
          }
        } else {
          ((SuplaDeviceClass *)_sdc)
              ->channelSetValueByServer(rd.data.sd_channel_new_value);
        }
        break;
      }
      case SUPLA_SDC_CALL_SET_ACTIVITY_TIMEOUT_RESULT:
        ((SuplaDeviceClass *)_sdc)
            ->channelSetActivityTimeoutResult(
                rd.data.sdc_set_activity_timeout_result);
        break;
    }

    srpc_rd_free(&rd);

  } else if (result == SUPLA_RESULT_DATA_ERROR) {
    supla_log(LOG_DEBUG, "DATA ERROR!");
  }
}

Network::Network(IPAddress *ip) {
  lastSentMs = 0;
  srpc = NULL;
  lastPingTimeMs = 0;
  serverActivityTimeoutS = 30;
  lastResponseMs = 0;

  netIntf = this;

  if (ip == NULL) {
    useLocalIp = false;
  } else {
    useLocalIp = true;
    localIp = *ip;
  }
}

bool Network::iterate() {
}

void Network::updateLastSent() {
  lastSentMs = millis();
}

void Network::updateLastResponse() {
  lastResponseMs = millis();
}

void Network::setSrpc(void *_srpc) {
  srpc = _srpc;
}

bool Network::ping() {
  _supla_int64_t _millis = millis();
  // If time from last response is longer than "server_activity_timeout + 10 s",
  // then inform about failure in communication
  if ((_millis - lastResponseMs) / 1000 >= (serverActivityTimeoutS + 10)) {
    return false;
  } else if (_millis - lastPingTimeMs >= 5000 &&
             ((_millis - lastResponseMs) / 1000 >=
                  (serverActivityTimeoutS - 5) ||
              (_millis - lastSentMs) / 1000 >= (serverActivityTimeoutS - 5))) {
    lastPingTimeMs = _millis;
    srpc_dcs_async_ping_server(srpc);
  }
  return true;
}

void Network::clearTimeCounters() {
  _supla_int64_t currentTime = millis();
  lastSentMs = currentTime;
  lastResponseMs = currentTime;
  lastPingTimeMs = currentTime;
}

void Network::setActivityTimeout(_supla_int_t activityTimeoutSec) {
  serverActivityTimeoutS = activityTimeoutSec;
}

};  // namespace Supla
