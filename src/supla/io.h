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

#ifndef _supla_io_h
#define _supla_io_h

#include <stdint.h>

namespace Supla {
// This class can be used to override digitalRead and digitalWrite methods.
// If you want to add custom behavior i.e. during read/write from some
// digital pin, you can inherit from Supla::Io class, implement your
// own customDigitalRead and customDigitalWrite methods and create instance
// of this class. It will automatically register and SuplaDevice will use it.
//
// Example use: implement some additional logic, when relay state is
// changed.
class Io {
 public:
  static void pinMode(uint8_t pin, uint8_t mode);
  static int digitalRead(uint8_t pin);
  static void digitalWrite(uint8_t pin, uint8_t val);
  static void pinMode(int channelNumber, uint8_t pin, uint8_t mode);
  static int digitalRead(int channelNumber, uint8_t pin);
  static void digitalWrite(int channelNumber, uint8_t pin, uint8_t val);

  static Io *ioInstance;

  Io();
  virtual ~Io();
  virtual void customPinMode(int channelNumber, uint8_t pin, uint8_t mode);
  virtual int customDigitalRead(int channelNumber, uint8_t pin);
  virtual void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);
};
};  // namespace Supla

#endif
