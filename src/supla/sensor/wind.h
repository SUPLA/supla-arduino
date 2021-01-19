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

#ifndef _wind_h
#define _wind_h

#include "supla/channel_element.h"

#define WIND_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class Wind: public ChannelElement {
 public:
  Wind() : lastReadTime(0) {
    channel.setType(SUPLA_CHANNELTYPE_WINDSENSOR);
    channel.setDefault(SUPLA_CHANNELFNC_WINDSENSOR);
    channel.setNewValue(WIND_NOT_AVAILABLE);
  }

  virtual double getValue() {
    return WIND_NOT_AVAILABLE;
  }

  void iterateAlways() {
    if (lastReadTime + 10000 < millis()) {
      lastReadTime = millis();
      channel.setNewValue(getValue());
    }
  }

 protected:
  unsigned long lastReadTime;
};

};  // namespace Sensor
};  // namespace Supla

#endif
