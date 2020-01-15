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

#ifndef _electricity_meter_h
#define _electricity_meter_h

#include "channel_extended.h"
#include "element.h"

namespace Supla {
class ElectricityMeter : public Element {
 public:
  ElectricityMeter() {
    extChannel.setType(SUPLA_CHANNELTYPE_ELECTRICITY_METER);
    extChannel.setDefault(SUPLA_CHANNELFNC_ELECTRICITY_METER);
    memset(&emValue, 0, sizeof(emValue));
  }

  virtual void updateChannelValues() {
    // Prepare extended channel value
    srpc_evtool_v1_emextended2extended(&emValue,
                                       extChannel.getExtValue());
    extChannel.setNewValue(emValue);
  }

  void onInit() {
    supla_log(LOG_DEBUG, "EM on init");
    emValue.measured_values = 0xFFFF;
    emValue.period = 5;
    emValue.total_forward_active_energy[0] = 123;
    emValue.total_forward_active_energy[1] = 234;
    emValue.total_forward_active_energy[2] = 345;

    emValue.m[0].voltage[0] = 223;
    emValue.m[0].freq = 5000;
    emValue.m[0].voltage[0] = 223;
    emValue.m[0].voltage[1] = 223;
    emValue.m[0].voltage[2] = 223;
    emValue.m[0].current[0] = 1234;
    emValue.m[0].current[1] = 1240;
    emValue.m[0].current[2] = 1239;
    emValue.m[0].power_active[0] = 100000;
    emValue.m[0].power_active[1] = 200000;
    emValue.m[0].power_active[2] = 300000;
    emValue.m[0].power_reactive[0] = 300000;
    emValue.m[0].power_reactive[1] = 300000;
    emValue.m[0].power_reactive[2] = 300000;
    emValue.m[0].power_apparent[0] = 300000;
    emValue.m[0].power_apparent[1] = 300000;
    emValue.m[0].power_apparent[2] = 300000;
    emValue.m[0].power_factor[0] = 100;
    emValue.m[0].power_factor[1] = 100;
    emValue.m[0].power_factor[2] = 100;
    emValue.m[0].phase_angle[0] = 100;
    emValue.m[0].phase_angle[1] = 100;
    emValue.m[0].phase_angle[2] = 100;
    emValue.m_count = 1;
    updateChannelValues();
  }

 protected:
  Channel *getChannel() {
    return &extChannel;
  }
  TElectricityMeter_ExtendedValue emValue;
  ChannelExtended extChannel;
};

};  // namespace Supla

#endif
