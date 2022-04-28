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

#include "simple.h"
#include <sstream>

Supla::Parser::Simple::Simple(Supla::Source::Source *src, int valuesCount)
  : source(src), valuesCount(valuesCount) {
  for (int i = 0; i < valuesCount; i++) {
    values.push_back(0);
    multipliers.push_back(1);
  }
}

Supla::Parser::Simple::~Simple() {}

double Supla::Parser::Simple::getValue(int index) {
  if (index < 0 || index > valuesCount) {
    valid = false;
    return 0;
  }

  return values[index];

}


bool Supla::Parser::Simple::refreshSource() {
  if (source) {
    sourceContent = source->getContent();

    if (sourceContent.length() == 0) {
      valid = false;
      return false;
    }

    std::stringstream ss(sourceContent);
    std::string line;

    for (int i = 0; i < valuesCount; i++) {
      std::getline(ss, line, '\n');

      std::stringstream sline(line);
      sline >> values[i];
    }

    valid = true;
    return true;
  }
  valid = false;
  return false;
}

bool Supla::Parser::Simple::isValid() {
  return valid;
}

void Supla::Parser::Simple::setMultiplier(double multiplier, int index) {
  if (index < 0 || index > valuesCount) {
    return;
  }

  multipliers[index] = multiplier;
}
