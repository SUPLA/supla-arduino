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

/*
 * Linux YAML based config file.
 *
 * It contain two parts:
 * 1. etc/supla-device.yaml file with user defined read-only configuration
 * 2. var/supla-device.yaml file used for supla-device read/write storage for
 *    GUID and AUTHKEY
 *
 * All "set" methods are executed against read/write storage or disabled.
 * It overrides some "get" methods to use read only storage with different
 * keys than those used by default in supla-device.
 *
 * Example config yaml file. Names and values are case-sensitive.
 * Please pay attantion to spaces, as those are important in yaml files.

name: Device name
# log_level - optional, values: info (default), debug, verbose
log_level: debug

supla:
  server: svrXYZ.supla.org
  mail: mail@user.com

# Order in channels is important. First item is channel 0, then 1, etc.
# Supla Cloud doesn't like changes in channel list, so you can only add channels
# at the end of the list. Otherwise, you will have to remove device from
# Cloud and register it again (whole measurement history will be lost).
channels:
  - type: VirtualRelay

  - type: ImpulseCounterParsed
    source:
      type: File
      file: /home/something
    parser:
      type: Json
      counter: total_m3

 */

#ifndef _supla_linux_yaml_config_h
#define _supla_linux_yaml_config_h

#include <supla/storage/config.h>
#include <string>
#include <yaml-cpp/yaml.h>
#include <map>
#include <supla/channel_element.h>
#include <supla/sensor/electricity_meter_parsed.h>
#include <supla/parser/parser.h>
#include <supla/source/source.h>

namespace Supla {

  class LinuxYamlConfig : public Config {
    public:
      LinuxYamlConfig(const std::string& file);
      virtual ~LinuxYamlConfig();

      bool isDebug();
      bool isVerbose();

      bool loadChannels();

      bool init() override;
      void removeAll() override;

      bool generateGuidAndAuthkey() override;

      // Generic getters and setters
      bool setString(const char* key, const char* value) override;
      bool getString(const char* key, char* value, size_t maxSize) override;
      int getStringSize(const char* key) override;

      bool setBlob(const char* key, const char* value, size_t blobSize) override;
      bool getBlob(const char* key, char* value, size_t blobSize) override;
      int getBlobSize(const char* key) override;

      bool getInt8(const char* key, int8_t* result) override;
      bool getUInt8(const char* key, uint8_t* result) override;
      bool getInt32(const char* key, int32_t* result) override;
      bool getUInt32(const char* key, uint32_t* result) override;

      bool setInt8(const char* key, const int8_t value) override;
      bool setUInt8(const char* key, const uint8_t value) override;
      bool setInt32(const char* key, const int32_t value) override;
      bool setUInt32(const char* key, const uint32_t value) override;

      void commit() override;

      // Device generic config
      virtual bool setGUID(const char* guid) override;
      virtual bool getGUID(char* result) override;
      virtual bool setAuthKey(const char* authkey) override;
      virtual bool getAuthKey(char* result) override;

      virtual bool setDeviceName(const char *name) override; // disabled
      virtual bool getDeviceName(char *result) override;

      // Supla protocol config
      virtual bool setSuplaCommProtocolEnabled(
          bool enabled) override;  // disabled
      virtual bool setSuplaServer(const char* server) override; // disabled
      virtual bool setSuplaServerPort(int32_t port) override; // disabled
      virtual bool setEmail(const char* email) override; // disabled

      virtual bool isSuplaCommProtocolEnabled() override;
      virtual bool getSuplaServer(char* result) override;
      virtual int32_t getSuplaServerPort() override;
      virtual bool getEmail(char* result) override;

      std::string getStateFilesPath();

    protected:
      bool parseChannel(const YAML::Node& ch, int channelNumber);
      Supla::Parser::Parser* addParser(const YAML::Node& parser,
          Supla::Source::Source *src);
      Supla::Source::Source* addSource(const YAML::Node& ch);

      bool addVirtualRelay(const YAML::Node& ch, int channelNumber);
      bool addFronius(const YAML::Node& ch, int channelNumber);
      bool addThermometerParsed(const YAML::Node& ch,
          int channelNumber,
          Supla::Parser::Parser *parser);
      bool addImpulseCounterParsed(const YAML::Node& ch,
          int channelNumber,
          Supla::Parser::Parser *parser);
      bool addElectricityMeterParsed(const YAML::Node& ch,
          int channelNumber,
          Supla::Parser::Parser *parser);
      bool addBinaryParsed(const YAML::Node& ch,
          int channelNumber,
          Supla::Parser::Parser *parser);
      void loadGuidAuthFromPath(const std::string& path);
      bool saveGuidAuth(const std::string& path);

      std::string file;
      YAML::Node config;
      std::string stateFilesLocaltion;

      std::string guid;
      std::string authkey;
      std::map<std::string, int> channelNames;
      std::map<std::string, int> parserNames;
      std::map<std::string, int> sourceNames;
      std::map<int, Supla::Parser::Parser*> parsers;
      std::map<int, Supla::Source::Source*> sources;

      int paramCount = 0;
      int parserCount = 0;
      int sourceCount = 0;
  };
};

#endif /*_supla_yaml_config_h*/

