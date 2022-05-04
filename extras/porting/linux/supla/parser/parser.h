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

#ifndef _SUPLA_PARSER_PARSER_H_
#define _SUPLA_PARSER_PARSER_H_

#include <supla/source/source.h>
#include <string>
#include <map>

namespace Supla {
  namespace Parser {

    class Parser {
      public:
        Parser(Supla::Source::Source *);
        virtual ~Parser(){};
        virtual bool refreshSource() = 0;

        virtual void addKey(const std::string& key, int index);
        virtual double getValue(const std::string &key) = 0;

        virtual bool isValid();
        void enableDebug();
      protected:
        std::map<std::string, int> keys;
        bool valid = false;
        bool debug = false;
        Supla::Source::Source *source = nullptr;
    };
  };  // namespace Parser
};  // namespace Supla
#endif /*_SUPLA_PARSER_PARSER_H_*/

