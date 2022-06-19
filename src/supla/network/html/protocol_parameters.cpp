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

#include <stdio.h>
#include <string.h>
#include <supla/network/web_sender.h>
#include <supla/storage/storage.h>
#include <supla/tools.h>

#include "protocol_parameters.h"
#include "supla/network/network.h"

namespace Supla {

namespace Html {

ProtocolParameters::ProtocolParameters() : HtmlElement(HTML_SECTION_PROTOCOL) {
}

ProtocolParameters::~ProtocolParameters() {
}

void ProtocolParameters::send(Supla::WebSender* sender) {
  auto cfg = Supla::Storage::ConfigInstance();
  if (cfg) {
    // Protocol selector
    sender->send(
        "<div class=\"w\">"
        "<i><select name=\"pro\" onchange=\"protocolChanged();\" "
        "id=\"protocol\">"
        "<option value=\"0\"");
    sender->send(selected(cfg->isSuplaCommProtocolEnabled()));

    sender->send(
        ">Supla</option>"
        "<option value=\"1\"");
    sender->send(selected(cfg->isMqttCommProtocolEnabled()));
    sender->send(
        ">MQTT</option>"
        "</select>"
        "<label>Protocol</label>"
        "</i>"
        "</div>");

    // Parameters for Supla protocol
    sender->send(
        "<div class=\"w\" id=\"proto_supla\">"
        "<h3>Supla Settings</h3>"
        "<i><input name=\"svr\" maxlength=\"64\" value=\"");
    char buf[512];
    if (cfg->getSuplaServer(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\"><label>Server</label></i>"
        "<i><input name=\"eml\" maxlength=\"255\" value=\"");
    if (cfg->getEmail(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\"><label>E-mail</label></i>"
        "</div>");

    // Parameters for MQTT protocol
    sender->send(
        "<div class=\"w mqtt\">"
        "<h3>MQTT Settings</h3>"
        "<i><input name=\"mqttserver\" maxlength=\"64\" value=\"");
    if (cfg->getMqttServer(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\"><label>Server</label></i>"
        "<i><input name=\"mqttport\" min=\"1\" max=\"65535\" type=\"number\""
        " value=\"");
    sender->send(cfg->getMqttServerPort());
    sender->send(
        "\"><label>Port</label></i>"
        "<i><select name=\"mqtttls\">"
        "<option value=\"0\" ");
    sender->send(selected(!cfg->isMqttTlsEnabled()));
    sender->send(
        ">NO</option>"
        "<option value=\"1\" ");
    sender->send(selected(cfg->isMqttTlsEnabled()));
    sender->send(
        ">YES</option></select>"
        "<label>TLS</label></i>"
        "<i>"
        "<select name=\"mqttauth\" id=\"sel_mauth\" "
        "onchange=\"mAuthChanged();\">"
        "<option value=\"0\" ");
    sender->send(selected(!cfg->isMqttAuthEnabled()));
    sender->send(
        ">NO</option>"
        "<option value=\"1\" ");
    sender->send(selected(cfg->isMqttAuthEnabled()));
    sender->send(
        ">YES</option></select>"
        "<label>Auth</label>"
        "</i>"
        "<i id=\"mauth_usr\"><input name=\"mqttuser\" maxlength=\"22\" "
        "value=\"");
    if (cfg->getMqttUser(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\" maxlength=\"45\">"
        "<label>Username</label></i>"
        "<i id=\"mauth_pwd\">"
        "<input name=\"mqttpasswd\" maxlength=\"32\">"
        "<label>Password (Required. Max 32)</label></i>"
        "<i><input name=\"mqttprefix\" maxlength=\"48\" value=\"");
    if (cfg->getMqttPrefix(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\" maxlength=\"49\">"
        "<label>Topic prefix</label></i>"
        "<i><input name=\"mqttqos\" min=\"0\" max=\"2\" type=\"number\" "
        "value=\"");
    sender->send(cfg->getMqttQos());
    sender->send(
        "\"><label>QoS</label></i>"
        "<i><select name=\"mqttretain\">"
        "<option value=\"0\" ");
    sender->send(selected(!cfg->isMqttRetainEnabled()));
    sender->send(
        ">NO</option>"
        "<option value=\"1\" ");
    sender->send(selected(cfg->isMqttRetainEnabled()));
    sender->send(
        ">YES</option></select>"
        "<label>Retain</label></i>"
        "<i>"
        "<input name=\"mqttpooldelay\" min=\"0\" max=\"3600\" type=\"number\" "
        "value=\"");
    sender->send(cfg->getMqttPoolPublicationDelay());
    sender->send(
        "\"><label>Pool publication delay (sec.)</label></i>"
        "</div>");
  }
}

bool ProtocolParameters::handleResponse(const char* key, const char* value) {
  auto cfg = Supla::Storage::ConfigInstance();
  if (strcmp(key, "pro") == 0) {
    int protocol = stringToUInt(value);
    switch (protocol) {
      default:
      case 0: {
        cfg->setSuplaCommProtocolEnabled(true);
        cfg->setMqttCommProtocolEnabled(false);
        break;
      }
      case 1: {
        cfg->setSuplaCommProtocolEnabled(false);
        cfg->setMqttCommProtocolEnabled(true);
        break;
      }
    }
    return true;
  } else if (strcmp(key, "svr") == 0) {
    cfg->setSuplaServer(value);
    return true;
  } else if (strcmp(key, "eml") == 0) {
    cfg->setEmail(value);
    return true;
  } else if (strcmp(key, "mqttserver") == 0) {
    cfg->setMqttServer(value);
    return true;
  } else if (strcmp(key, "mqttport") == 0) {
    int port = stringToUInt(value);
    cfg->setMqttServerPort(port);
    return true;
  } else if (strcmp(key, "mqtttls") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttTlsEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "mqttauth") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttAuthEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "mqttuser") == 0) {
    cfg->setMqttUser(value);
    return true;
  } else if (strcmp(key, "mqttpasswd") == 0) {
    if (strlen(value) > 0) {
      cfg->setMqttPassword(value);
    }
    return true;
  } else if (strcmp(key, "mqttprefix") == 0) {
    cfg->setMqttPrefix(value);
    return true;
  } else if (strcmp(key, "mqttqos") == 0) {
    int qos = stringToUInt(value);
    cfg->setMqttQos(qos);
    return true;
  } else if (strcmp(key, "mqttretain") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttRetainEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "mqttpooldelay") == 0) {
    int poolDelay = stringToUInt(value);
    cfg->setMqttPoolPublicationDelay(poolDelay);
    return true;
  }

  return false;
}

};  // namespace Html
};  // namespace Supla
