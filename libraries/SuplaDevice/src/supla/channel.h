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

#include "../supla-common/proto.h"

namespace Supla {

class Channel {
 public:
  Channel();

  static Channel *begin();
  static Channel *last();
  static int size();

  void setNewValue(double dbl);
  void setNewValue(double temp, double humi);
  void setNewValue(int value);
  void setNewValue(bool value);
  void setNewValue(TElectricityMeter_ExtendedValue &emValue);
  void setNewValue(uint8_t red,
                   uint8_t green,
                   uint8_t blue,
                   uint8_t colorBrightness,
                   uint8_t brightness);
  bool setNewValue(char *newValue);

  virtual bool isExtended();
  bool isUpdateReady();
  int getChannelNumber();
  int getChannelType();

  void setType(int type);
  void setDefault(int value);
  void setFlag(int flag);
  void setFuncList(int functions);
  void clearUpdateReady();
  void sendUpdate(void *srpc);
  virtual TSuplaChannelExtendedValue *getExtValue();
  static void clearAllUpdateReady();
  Channel *next();

  static unsigned long lastCommunicationTimeMs;
  static TDS_SuplaRegisterDevice_E reg_dev;

 protected:
  void setUpdateReady();

  bool valueChanged;
  int channelNumber;

  Channel *nextPtr;
  static Channel *firstPtr;
};

};  // namespace Supla

#endif
