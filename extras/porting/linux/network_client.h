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

#ifndef __SUPLA_NETWORK_CLIENT_H_
#define __SUPLA_NETWORK_CLIENT_H_

#include "IPAddress.h"

namespace Supla {
  class NetworkClient {
    public:
      NetworkClient();
      virtual ~NetworkClient();

      virtual int connect(IPAddress ip, uint16_t port);
      virtual int connect(const char *host, uint16_t port);
      virtual size_t write(uint8_t);
      virtual size_t write(const uint8_t *buf, size_t size) override;

      virtual int available();
      virtual int read();
      virtual int read(uint8_t* buf, size_t size);
      int read(char* buf, size_t size);

      virtual void stop();
      virtual uint8_t connected();

  };
};

#endif /*__SUPLA_RSA_VERIFICATOR_H_*/
