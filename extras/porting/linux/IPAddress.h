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

#ifndef EXTRAS_PORTING_LINUX_IPADDRESS_H_
#define EXTRAS_PORTING_LINUX_IPADDRESS_H_

#include <cstdint>
#include <string>

class IPAddress {
 public:
  IPAddress();
  IPAddress(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
  explicit IPAddress(const std::string &ip);

  union {
    uint8_t addr[4] = {};
    uint32_t full;
  };
};

#endif  // EXTRAS_PORTING_LINUX_IPADDRESS_H_
