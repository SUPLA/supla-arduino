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

#ifndef EXTRAS_PORTING_LINUX_NETWORK_CLIENT_H_
#define EXTRAS_PORTING_LINUX_NETWORK_CLIENT_H_

#include "IPAddress.h"

namespace Supla {
class NetworkClient {
 public:
  NetworkClient();
  virtual ~NetworkClient();

  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual std::size_t write(uint8_t);
  virtual std::size_t write(const uint8_t *buf, std::size_t size);
  virtual std::size_t print(const char *);
  virtual std::size_t println(const char *);
  virtual std::size_t println();

  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, std::size_t size);
  virtual int read(char *buf, std::size_t size);

  virtual void stop();
  virtual uint8_t connected();

 protected:
  int connectionFd = -1;
};
};  // namespace Supla

#endif  // EXTRAS_PORTING_LINUX_NETWORK_CLIENT_H_
