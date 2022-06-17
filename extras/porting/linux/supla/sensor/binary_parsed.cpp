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

#include <supla-common/log.h>

#include "binary_parsed.h"

Supla::Sensor::BinaryParsed::BinaryParsed(Supla::Parser::Parser *parser)
    : SensorParsed(parser) {
}

bool Supla::Sensor::BinaryParsed::getValue() {
  bool value = false;

  if (isParameterConfigured(Supla::Parser::State)) {
    if (refreshParserSource()) {
      double result = getParameterValue(Supla::Parser::State);
      if (result - 0.1 <= 1 && 1 <= result + 0.1) {
        value = true;
      }
      if (!parser->isValid()) {
        value = false;
      }
    }
  }
  return value;
}
