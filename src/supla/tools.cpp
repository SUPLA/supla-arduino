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

#include "tools.h"

void float2DoublePacked(float number, uint8_t *bar, int byteOrder) {
  (void)(byteOrder);
  _FLOATCONV fl;
  fl.f = number;
  _DBLCONV dbl;
  dbl.p.s = fl.p.s;
  dbl.p.e = fl.p.e - 127 + 1023;  // exponent adjust
  dbl.p.m = fl.p.m;

#ifdef IEEE754_ENABLE_MSB
  if (byteOrder == LSBFIRST) {
#endif
    for (int i = 0; i < 8; i++) {
      bar[i] = dbl.b[i];
    }
#ifdef IEEE754_ENABLE_MSB
  } else {
    for (int i = 0; i < 8; i++) {
      bar[i] = dbl.b[7 - i];
    }
  }
#endif
}

float doublePacked2float(uint8_t *bar) {
  _FLOATCONV fl;
  _DBLCONV dbl;
  for (int i = 0; i < 8; i++) {
    dbl.b[i] = bar[i]; 
  }
  fl.p.s = dbl.p.s;
  fl.p.m = dbl.p.m;
  fl.p.e = dbl.p.e + 127 - 1023;  // exponent adjust

  return fl.f;
}
