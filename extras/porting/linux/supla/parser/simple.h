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

#ifndef _SUPLA_PARSER_SIMPLE_H_
#define _SUPLA_PARSER_SIMPLE_H_

#include <supla/source/source.h>
#include "parser.h"
#include <string>
#include <vector>

namespace Supla {

  namespace Parser {
    class Simple : public Parser {
      public:
        Simple(Supla::Source::Source *, int valuesCount = 1);
        virtual ~Simple();

        virtual bool refreshSource() override;

        virtual double getValue(int index = 0) override;
        virtual void setMultiplier(double multiplier, int index = 0);

        virtual bool isValid() override;
      protected:
        Supla::Source::Source *source = nullptr;
        std::string sourceContent;

        bool valid = false;
        int valuesCount = 1;
        std::vector<double> values;
        std::vector<double> multipliers;
    };
  };  // namespace Source
};  // namespace Supla

#endif /*_SUPLA_PARSER_SIMPLE_H_*/

