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

#include <IPAddress.h>

namespace Supla {
class Network {
 public:
  static Network *Instance() {
    return netIntf;
  }

  virtual int read(void *buf, int count) = 0;
  virtual int write(void *buf, int count) = 0;
  virtual bool connect(const char *server, int port) = 0;
  virtual bool connected() = 0;
  virtual void disconnect() = 0;
  virtual void setup(uint8_t mac[6], IPAddress *ip) = 0;

 protected:
  static Network *netIntf;
};

};  // namespace Supla

#endif
