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

#ifndef __SUPLA_SHA256_H_
#define __SUPLA_SHA256_H_

#include "nettle/sha2.h"

/*
 * Simple wrapper for nettle SHA256 methods.
 */

namespace Supla {

  class Sha256 {
    public:
      Sha256();
      ~Sha256();
      void update(const uint8_t *data, const int size);
      struct sha256_ctx* getHash();

    protected:
      struct sha256_ctx hash;

  };

};

#endif /*__SUPLA_SHA256_H_*/
