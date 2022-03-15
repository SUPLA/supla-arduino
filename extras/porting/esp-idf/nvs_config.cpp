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

#include "nvs_config.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <supla-common/log.h>

using namespace Supla;

NvsConfig::NvsConfig() {
}

NvsConfig::~NvsConfig() {
}

bool NvsConfig::init() {
  supla_log(LOG_DEBUG, "NvsConfig: initializing nvs based config storage");
  ESP_ERROR_CHECK(nvs_flash_init());
  nvs_stats_t nvs_stats;
  nvs_get_stats(NULL, &nvs_stats);
  supla_log(
      LOG_DEBUG,
      "NVS Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)",
      nvs_stats.used_entries,
      nvs_stats.free_entries,
      nvs_stats.total_entries);
  ESP_ERROR_CHECK(nvs_open("supla", NVS_READWRITE, &nvsHandle));
  return true;
}

bool NvsConfig::loadDeviceConfig() {
  return true;
}

bool NvsConfig::loadElementConfig() {
  return true;
}

// Generic getters and setters
bool NvsConfig::setString(const char* key, const char* value) {
  esp_err_t err = nvs_set_str(nvsHandle, key, value);
  return err == ESP_OK;
}

bool NvsConfig::getString(const char* key, char* value, size_t maxSize) {
  esp_err_t err =
    nvs_get_str(nvsHandle, key, value, &maxSize);
  return err == ESP_OK;
}

int NvsConfig::getStringSize(const char* key) {
  return -1;
}

bool NvsConfig::setBlob(const char* key, const char* value) {
  return false;
}

bool NvsConfig::getBlob(const char* key, char* value, size_t maxSize) {
  return false;
}

int NvsConfig::getBlobSize(const char* key) {
  return -1;
}

bool NvsConfig::getInt8(const char* key, int8_t& result) {
  return false;
}

bool NvsConfig::getUInt8(const char* key, uint8_t& result) {
  return false;
}

bool NvsConfig::getInt32(const char* key, int32_t& result) {
  return false;
}

bool NvsConfig::getUInt32(const char* key, uint32_t& result) {
  return false;
}


bool NvsConfig::setInt8(const char* key, const int8_t value) {
  return false;
}

bool NvsConfig::setUInt8(const char* key, const uint8_t value) {
  return false;
}

bool NvsConfig::setInt32(const char* key, const int32_t value) {
  return false;
}

bool NvsConfig::setUInt32(const char* key, const uint32_t value) {
  return false;
}


