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

#ifndef _SUPLA_SENSOR_IMPULSE_COUNTER_PARSED_H_
#define _SUPLA_SENSOR_IMPULSE_COUNTER_PARSED_H_

#include <supla/channel_element.h>
#include <supla/parser/parser.h>
#include "sensor_parsed.h"
#include <string>

namespace Supla {
  namespace Parser {
    const char Counter[] = "counter";
  };

  namespace Sensor {

    class ImpulseCounterParsed : public ChannelElement, public SensorParsed {
      public:
        ImpulseCounterParsed(Supla::Parser::Parser *);

        virtual unsigned _supla_int64_t getValue();
        virtual void onInit() override;
        virtual void iterateAlways() override;

      protected:
        unsigned long lastReadTime = 0;
        bool isDataErrorLogged = false;
    };
  };  // namespace Source
};  // namespace Supla

#endif /*_SUPLA_SENSOR_IMPULSE_COUNTER_PARSED_H_*/

