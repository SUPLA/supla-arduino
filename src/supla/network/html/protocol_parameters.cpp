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

#include "protocol_parameters.h"
#include <supla/storage/storage.h>
#include <supla/network/web_sender.h>
#include "supla/network/network.h"
#include <stdio.h>

namespace Supla {

namespace Html {

  ProtocolParameters::ProtocolParameters() : HtmlElement(HTML_SECTION_PROTOCOL) {}

  ProtocolParameters::~ProtocolParameters() {}

  void ProtocolParameters::send(Supla::WebSender *sender) {
    auto cfg = Supla::Storage::ConfigInstance();
    if (cfg) {
      // Protocol selector
      sender->send(
          "<div class=\"w\">"
          "<i><select name=\"pro\" onchange=\"protocolChanged();\" "
          "id=\"protocol\">"
          "<option value=\"0\" "
          );
      if (cfg->isSuplaCommProtocolEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">Supla</option>"
          "<option value=\"1\" "
          );
      if (cfg->isMqttCommProtocolEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">MQTT</option>"
          "</select>"
          "<label>Protocol</label>"
          "</i>"
          "</div>"
          );

      // Parameters for Supla protocol
      sender->send(
          "<div class=\"w\" id=\"proto_supla\">"
          "<h3>Supla Settings</h3>"
          "<i><input name=\"svr\" value=\""
          );
      char buf[512];
      if (cfg->getSuplaServer(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\"><label>Server</label></i>"
          "<i><input name=\"eml\" value=\""
          );
      if (cfg->getEmail(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\"><label>E-mail</label></i>"
         "</div>"
         );

      // Parameters for MQTT protocol
      sender->send(
          "<div class=\"w mqtt\">"
          "<h3>MQTT Settings</h3>"
          "<i><input name=\"mqttserver\" value=\""
          );
      if (cfg->getMqttServer(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\"><label>Server</label></i>"
          "<i><input name=\"mqttport\" min=\"1\" max=\"65535\" type=\"number\""
          " value=\""
          );
      int port = cfg->getMqttServerPort();
      snprintf(buf, 512, "%d", port);
      sender->send(buf);
      sender->send(
          "\"><label>Port</label></i>"
          "<i><select name=\"mqtttls\">"
          "<option value=\"0\" "
          );
      if (!cfg->isMqttTlsEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">NO</option>"
          "<option value=\"1\" "
          );
      if (cfg->isMqttTlsEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">YES</option></select>"
          "<label>TLS</label></i>"
          "<i>"
          "<select name=\"mqttauth\" id=\"sel_mauth\" onchange=\"mAuthChanged();\">"
          "<option value=\"0\" "
          );
      if (!cfg->isMqttAuthEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">NO</option>"
          "<option value=\"1\" "
          );
      if (cfg->isMqttAuthEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">YES</option></select>"
          "<label>Auth</label>"
          "</i>"
          "<i id=\"mauth_usr\"><input name=\"mqttuser\" value=\""
          );
      if (cfg->getMqttUser(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\" maxlength=\"45\">"
          "<label>Username</label></i>"
          "<i id=\"mauth_pwd\">"
          "<input name=\"mqttpasswd\" maxlength=\"32\">"
          "<label>Password (Required. Max 32)</label></i>"
          "<i><input name=\"mqttprefix\" value=\""
          );
      if (cfg->getMqttPrefix(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\" maxlength=\"49\">"
          "<label>Topic prefix</label></i>"
          "<i><input name=\"mqttqos\" min=\"0\" max=\"2\" type=\"number\" value=\""
          );
      int qos = cfg->getMqttQos();
      snprintf(buf, 512, "%d", qos);
      sender->send(buf);
      sender->send(
          "\"><label>QoS</label></i>"
          "<i><select name=\"mqttretain\">"
          "<option value=\"0\" "
          );
      if (!cfg->isMqttRetainEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">NO</option>"
          "<option value=\"1\" "
          );
      if (cfg->isMqttRetainEnabled()) {
        sender->send("selected");
      }
      sender->send(
          ">YES</option></select>"
          "<label>Retain</label></i>"
          "<i>"
          "<input name=\"mqttpooldelay\" min=\"0\" max=\"3600\" type=\"number\" value=\""
          );
      int ppd = cfg->getMqttPoolPublicationDelay();
      snprintf(buf, 512, "%d", ppd);
      sender->send(buf);
      sender->send(
          "\"><label>Pool publication delay (sec.)</label></i>"
          "</div>"
          );
    }
  }

  //    virtual bool handleResponse() = 0;

};
};


