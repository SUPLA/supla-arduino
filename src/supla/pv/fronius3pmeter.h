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

#ifndef __fronius3pmeter_h
#define __fronius3pmeter_h

#include <IPAddress.h>
#include <supla/sensor/electricity_meter.h>

#if defined(ARDUINO_ARCH_AVR)
#include <Ethernet.h>
#else
#include <WiFiClient.h>
#endif

namespace Supla {
namespace PV {
class Fronius3pmeter : public Supla::Sensor::ElectricityMeter {
 public:
  Fronius3pmeter(IPAddress ip, int port = 80, int deviceId = 0);
  void readValuesFromDevice();
  void iterateAlways();
  bool iterateConnected(void *srpc);

 protected:
#if defined(ARDUINO_ARCH_AVR)
  EthernetClient pvClient;
#else
  WiFiClient pvClient;
#endif
  IPAddress ip;
  int port;
  char buf[80];
  unsigned _supla_int64_t FwdReactEnergy;
  unsigned _supla_int64_t RvrReactEnergy;
  unsigned _supla_int64_t FwdActEnergy;
  unsigned _supla_int64_t RvrActEnergy;
  _supla_int_t currentPower;
  _supla_int_t currentActivePower[3];
  _supla_int_t currentApparentPower[3];
  _supla_int_t currentReactivePower[3];
  _supla_int_t currentPowerFactor[3];
  unsigned _supla_int_t currentCurrent[3];
  unsigned _supla_int16_t currentFreq;
  unsigned _supla_int16_t currentVoltage[3];
  int bytesCounter;
  int retryCounter;
  float valueToFetch;
  int deviceId;
  bool startCharFound;
  bool dataIsReady;
  bool dataFetchInProgress;
  unsigned long connectionTimeoutMs;
};
};  // namespace PV
};  // namespace Supla

#endif
