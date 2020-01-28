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

#ifndef _channel_h
#define _channel_h

#include <Arduino.h>

#include "../supla-common/log.h"
#include "../supla-common/proto.h"
#include "../supla-common/srpc.h"
#include "tools.h"

namespace Supla {

/*
class Channel;

class ChannelIterator {
public:
ChannelIterator& operator++() {
  if (currentPtr) {
    currentPtr = currentPtr->nextPtr;
  }
  return *this;
}

ChannelIterator(Element *ptr) {
  currentPtr = ptr;
}

protected:
Channel *currentPtr;
}
 */

class Channel {
 public:
  Channel() {
    valueChanged = false;
    channelNumber = -1;
    if (reg_dev.channel_count < SUPLA_CHANNELMAXCOUNT) {
      channelNumber = reg_dev.channel_count;
      reg_dev.channels[channelNumber].Number = channelNumber;
      reg_dev.channel_count++;
    }

    if (firstPtr == nullptr) {
      firstPtr = this;
    } else {
      last()->nextPtr = this;
    }
    nextPtr = nullptr;
  }

  static Channel *begin() {
    return firstPtr;
  }

  static Channel *last() {
    Channel *ptr = firstPtr;
    while (ptr && ptr->nextPtr) {
      ptr = ptr->nextPtr;
    }
    return ptr;
  }

  static int size() {
    int count = 0;
    Channel *ptr = firstPtr;
    if (ptr) {
      count++;
    }
    while (ptr->nextPtr) {
      count++;
      ptr = ptr->nextPtr;
    }
    return count;
  }

  bool isUpdateReady() {
    return valueChanged;
  };

  void setNewValue(double dbl) {
    char newValue[SUPLA_CHANNELVALUE_SIZE];
    if (sizeof(double) == 8) {
      memcpy(newValue, &dbl, 8);
    } else if (sizeof(double) == 4) {
      float2DoublePacked(dbl, (uint8_t *)(newValue));
    }
    if (setNewValue(newValue)) {
      supla_log(
          LOG_DEBUG, "Channel(%d) value changed to %f", channelNumber, dbl);
    }
  }

  void setNewValue(double temp, double humi) {
    char newValue[SUPLA_CHANNELVALUE_SIZE];
    long t = temp * 1000.00;
    long h = humi * 1000.00;

    memcpy(newValue, &t, 4);
    memcpy(&(newValue[4]), &h, 4);

    if (setNewValue(newValue)) {
      supla_log(LOG_DEBUG,
                "Channel(%d) value changed to temp(%f), humi(%f)",
                channelNumber,
                temp,
                humi);
    }
  }

  void setNewValue(TElectricityMeter_ExtendedValue &emValue) {
    // Prepare standard channel value
    if (sizeof(TElectricityMeter_Value) <= SUPLA_CHANNELVALUE_SIZE) {
      TElectricityMeter_Measurement *m = nullptr;
      TElectricityMeter_Value v;
      memset(&v, 0, sizeof(TElectricityMeter_Value));

      unsigned _supla_int64_t fae_sum = emValue.total_forward_active_energy[0] +
                                        emValue.total_forward_active_energy[1] +
                                        emValue.total_forward_active_energy[2];

      v.total_forward_active_energy = fae_sum / 1000;

      if (emValue.m_count && emValue.measured_values & EM_VAR_VOLTAGE) {
        m = &emValue.m[emValue.m_count - 1];

        if (m->voltage[0] > 0) {
          v.flags |= EM_VALUE_FLAG_PHASE1_ON;
        }

        if (m->voltage[1] > 0) {
          v.flags |= EM_VALUE_FLAG_PHASE2_ON;
        }

        if (m->voltage[2] > 0) {
          v.flags |= EM_VALUE_FLAG_PHASE3_ON;
        }
      }

      memcpy(reg_dev.channels[channelNumber].value,
             &v,
             sizeof(TElectricityMeter_Value));
      setUpdateReady();
    }
  }

  bool setNewValue(char *newValue) {
    if (memcmp(newValue, reg_dev.channels[channelNumber].value, 8) != 0) {
      setUpdateReady();
      memcpy(reg_dev.channels[channelNumber].value, newValue, 8);
      return true;
    }
    return false;
  }

  virtual bool isExtended() {
    return false;
  }

  void setType(int type) {
    if (channelNumber >= 0) {
      reg_dev.channels[channelNumber].Type = type;
    }
  }

  void setDefault(int value) {
    if (channelNumber >= 0) {
      reg_dev.channels[channelNumber].Default = value;
    }
  }

  void setFlag(int flag) {
    if (channelNumber >= 0) {
      reg_dev.channels[channelNumber].Flags |= flag;
    }
  }

  int getChannelNumber() {
    return channelNumber;
  }

  static TDS_SuplaRegisterDevice_E reg_dev;
  void clearUpdateReady() {
    valueChanged = false;
  };

  void sendUpdate(void *srpc) {
    srpc_ds_async_channel_value_changed(
        srpc, channelNumber, reg_dev.channels[channelNumber].value);
    
    // returns null for non-extended channels
    TSuplaChannelExtendedValue *extValue = getExtValue();  
    if (extValue) {
      srpc_ds_async_channel_extendedvalue_changed(
          srpc, channelNumber, extValue);
    }

    clearUpdateReady();
  }

  virtual TSuplaChannelExtendedValue *getExtValue() {
    return nullptr;
  }

  static void clearAllUpdateReady() {
    for (auto channel = begin(); channel != nullptr; channel = channel->next()) {
      if (!channel->isExtended()) {
        channel->clearUpdateReady();
      }
    }
  }

  Channel *next() {
    return nextPtr;
  }

  static unsigned long nextCommunicationTimeMs;

 protected:
  void setUpdateReady() {
    valueChanged = true;
  };

  bool valueChanged;
  int channelNumber;

  Channel *nextPtr;
  static Channel *firstPtr;
};

};  // namespace Supla

#endif
