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

#include "status_led_parameters.h"
#include <supla/network/web_sender.h>
#include <supla/storage/storage.h>
#include <supla/storage/config.h>

namespace Supla {

namespace Html {

  StatusLedParameters::StatusLedParameters() : HtmlElement(HTML_SECTION_FORM) {
  }

  StatusLedParameters::~StatusLedParameters() {}

  void StatusLedParameters::send(Supla::WebSender *sender) {
    auto cfg = Supla::Storage::ConfigInstance();
    if (cfg) {
      int8_t value = 0;
      cfg->getInt8("statusled", value);

      sender->send(
          "<i><select name=\"led\">"
          "<option value=\"0\" "
          );
      if (value == 0) {
        sender->send("selected");
      }
      sender->send(
          ">ON - WHEN CONNECTED</option>"
          "<option value=\"1\" "
          );
      if (value == 1) {
        sender->send("selected");
      }
      sender->send(
          ">OFF - WHEN CONNECTED</option>"
          "<option value=\"2\" "
          );
      if (value == 2) {
        sender->send("selected");
      }
      sender->send(
          ">ALWAYS OFF</option></select>"
          "<label>Status LED</label></i>"
          );
    }
  }

};
};
