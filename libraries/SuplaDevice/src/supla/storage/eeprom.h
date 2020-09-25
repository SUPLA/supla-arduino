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

#ifndef _supla_eeprom_h
#define _supla_eeprom_h

#include "storage.h"

namespace Supla {

class Eeprom : public Storage {
 public:
  Eeprom(unsigned int storageStartingOffset = 0);
  bool init();
  bool saveStateAllowed(unsigned long ms);
  void commit();

 protected:
  int readStorage(int, unsigned char *, int, bool);
  int writeStorage(int, const unsigned char *, int);

  bool dataChanged;

  unsigned long lastWriteTimestamp;
};

};  // namespace Supla

#endif
