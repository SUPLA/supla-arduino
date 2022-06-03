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

#if !defined(ARDUINO_ARCH_AVR)
// don't compile it on Arduino Mega

#include <LittleFS.h>
#include "littlefs_config.h"
#include <supla-common/log.h>

namespace Supla {
  const char ConfigFileName[] = "/supla-dev.cfg";
};

#define SUPLA_LITTLEFS_CONFIG_BUF_SIZE 1024

Supla::LittleFsConfig::LittleFsConfig() {}

Supla::LittleFsConfig::~LittleFsConfig() {}

bool Supla::LittleFsConfig::init() {
  if (first) {
    supla_log(LOG_WARNING,
        "LittleFsConfig: init called on non empty database. Aborting");
    // init can be done only on empty storage
    return false;
  }
  bool result = LittleFS.begin();
  if (!result) {
    supla_log(LOG_WARNING, "LittleFsConfig: formatting partition");
    LittleFS.format();
  }
  result = LittleFS.begin();
  if (result) {
    supla_log(LOG_DEBUG, "LittleFsConfig: init successful");
  } else {
    supla_log(LOG_ERR,
        "LittleFsConfig: failed to mount and to format partition");
  }

  if (LittleFS.exists(ConfigFileName)) {
    File cfg = LittleFS.open(ConfigFileName, "r");
    if (!cfg) {
      supla_log(LOG_ERR, "LittleFsConfig: failed to open config file");
      LittleFS.end();
      return false;
    }

    int fileSize = cfg.size();

    if (fileSize > SUPLA_LITTLEFS_CONFIG_BUF_SIZE) {
      supla_log(LOG_ERR, "LittleFsConfig: config file is too big");
      cfg.close();
      LittleFS.end();
      return false;
    }

    uint8_t buf[SUPLA_LITTLEFS_CONFIG_BUF_SIZE] = {};
    int bytesRead = cfg.read(buf, fileSize);

    cfg.close();
    LittleFS.end();
    if (bytesRead != fileSize) {
      supla_log(LOG_DEBUG,
          "LittleFsConfig: read bytes %d, while file is %d bytes",
          bytesRead,
          fileSize);
      return false;
    }

    supla_log(LOG_DEBUG, "LittleFsConfig: initializing storage from file...");
    return initFromMemory(buf, fileSize);
  } else {
    supla_log(LOG_DEBUG, "LittleFsConfig:: config file missing");
  }
  LittleFS.end();
  return true;
}

void Supla::LittleFsConfig::commit() {
  uint8_t buf[SUPLA_LITTLEFS_CONFIG_BUF_SIZE];

  size_t dataSize = serializeToMemory(buf, SUPLA_LITTLEFS_CONFIG_BUF_SIZE);

  auto result = LittleFS.begin();
  if (result) {
    supla_log(LOG_DEBUG, "LittleFsConfig: init successful");
  }
  File cfg = LittleFS.open(ConfigFileName, "w");
  if (!cfg) {
    supla_log(LOG_ERR, "LittleFsConfig: failed to open config file for write");
    LittleFS.end();
    return;
  }

  cfg.write(buf, dataSize);
  cfg.close();
  LittleFS.end();
}

#endif
