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

#ifndef _network_interface_h
#define _network_interface_h

#include "supla-common/log.h"
#include "supla-common/proto.h"

namespace Supla {
class Network {
 public:
  static Network *Instance() {
    return netIntf;
  }

  static bool Connected() {
    if (Instance() != NULL) {
      return Instance()->connected();
    }
    return false;
  }

  static int Read(void *buf, int count) {
    if (Instance() != NULL) {
      return Instance()->read(buf, count);
    }
    return -1;
  }

  static int Write(void *buf, int count) {
    if (Instance() != NULL) {
      return Instance()->write(buf, count);
    }
    return -1;
  }

  static int Connect(const char *server, int port = -1) {
    if (Instance() != NULL) {
      Instance()->clearTimeCounters();
      return Instance()->connect(server, port);
    }
    return 0;
  }

  static void Disconnect() {
    if (Instance() != NULL) {
      return Instance()->disconnect();
    }
    return;
  }

  static void Setup() {
    if (Instance() != NULL) {
      return Instance()->setup();
    }
    return;
  }

  static bool IsReady() {
    if (Instance() != NULL) {
      return Instance()->isReady();
    }
    return false;
  }

  static bool Iterate() {
    if (Instance() != NULL) {
      return Instance()->iterate();
    }
    return false;
  }

  static void SetSrpc(void *_srpc) {
    if (Instance() != NULL) {
      Instance()->setSrpc(_srpc);
    }
  }

  static bool Ping() {
    if (Instance() != NULL) {
      return Instance()->ping();
    }
    return false;
  }

  Network(uint8_t ip[4]);
  virtual int read(void *buf, int count) = 0;
  virtual int write(void *buf, int count) = 0;
  virtual int connect(const char *server, int port = -1) = 0;
  virtual bool connected() = 0;
  virtual void disconnect() = 0;
  virtual void setup() = 0;
  virtual void setTimeout(int);

  virtual bool isReady() = 0;
  virtual bool iterate();
  virtual bool ping();

  virtual void fillStateData(TDSC_ChannelState &channelState);

  void setSrpc(void *_srpc);
  void updateLastSent();
  void updateLastResponse();
  void clearTimeCounters();
  void setActivityTimeout(_supla_int_t activityTimeoutSec);

 protected:
  static Network *netIntf;
  _supla_int64_t lastSentMs;
  _supla_int64_t lastResponseMs;
  _supla_int64_t lastPingTimeMs;
  _supla_int_t serverActivityTimeoutS;
  void *srpc;

  bool useLocalIp;
  unsigned char localIp[4];
};

// Method passed to SRPC as a callback to read raw data from network interface
_supla_int_t data_read(void *buf, _supla_int_t count, void *sdc);
// Method passed to SRPC as a callback to write raw data to network interface
_supla_int_t data_write(void *buf, _supla_int_t count, void *sdc);
// Method passed to SRPC as a callback to handle response from Supla server
void message_received(void *_srpc,
                      unsigned _supla_int_t rr_id,
                      unsigned _supla_int_t call_type,
                      void *_sdc,
                      unsigned char proto_version);

};  // namespace Supla

#endif
