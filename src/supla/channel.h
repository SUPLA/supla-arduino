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

#include <stdint.h>

#include "supla-common/proto.h"
#include "local_action.h"

namespace Supla {

class Channel : public LocalAction {
 public:
  Channel();
  ~Channel();

  void setNewValue(double dbl);
  void setNewValue(double temp, double humi);
  void setNewValue(_supla_int_t value);
  void setNewValue(bool value);
  void setNewValue(TElectricityMeter_ExtendedValue_V2 &emValue);
  void setNewValue(uint8_t red,
                   uint8_t green,
                   uint8_t blue,
                   uint8_t colorBrightness,
                   uint8_t brightness);
  void setNewValue(_supla_int64_t value);
  bool setNewValue(char *newValue);

  double getValueDouble();
  double getValueDoubleFirst();
  double getValueDoubleSecond();
  _supla_int_t getValueInt32();
  _supla_int64_t getValueInt64();
  bool getValueBool();
  uint8_t getValueRed();
  uint8_t getValueGreen();
  uint8_t getValueBlue();
  uint8_t getValueColorBrightness();
  uint8_t getValueBrightness();

  virtual bool isExtended();
  bool isUpdateReady();
  int getChannelNumber();
  _supla_int_t getChannelType();

  void setType(_supla_int_t type);
  void setDefault(_supla_int_t value);
  void setFlag(_supla_int_t flag);
  void unsetFlag(_supla_int_t flag);
  void setFuncList(_supla_int_t functions);
  void clearUpdateReady();
  void sendUpdate(void *srpc);
  virtual TSuplaChannelExtendedValue *getExtValue();

  static unsigned long lastCommunicationTimeMs;
  static TDS_SuplaRegisterDevice_E reg_dev;

 protected:
  void setUpdateReady();

  bool valueChanged;
  int channelNumber;

};

};  // namespace Supla

#endif
