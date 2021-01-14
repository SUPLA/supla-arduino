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

#ifndef _electricity_meter_h
#define _electricity_meter_h

#include <Arduino.h>

#include "../channel_extended.h"
#include "../element.h"
#include <supla-common/srpc.h>

#define MAX_PHASES 3

namespace Supla {
namespace Sensor {
class ElectricityMeter : public Element {
 public:
  ElectricityMeter();

  virtual void updateChannelValues();

  // energy in 0.00001 kWh
  void setFwdActEnergy(int phase, unsigned _supla_int64_t energy);

  // energy in 0.00001 kWh
  void setRvrActEnergy(int phase, unsigned _supla_int64_t energy);

  // energy in 0.00001 kWh
  void setFwdReactEnergy(int phase, unsigned _supla_int64_t energy);

  // energy in 0.00001 kWh
  void setRvrReactEnergy(int phase, unsigned _supla_int64_t energy);

  // voltage in 0.01 V
  void setVoltage(int phase, unsigned _supla_int16_t voltage);

  // current in 0.001 A
  void setCurrent(int phase, unsigned _supla_int_t current);

  // Frequency in 0.01 Hz
  void setFreq(unsigned _supla_int16_t freq);

  // power in 0.00001 kW
  void setPowerActive(int phase, _supla_int_t power);

  // power in 0.00001 kvar
  void setPowerReactive(int phase, _supla_int_t power);

  // power in 0.00001 kVA
  void setPowerApparent(int phase, _supla_int_t power);

  // power in 0.001
  void setPowerFactor(int phase, _supla_int_t powerFactor);

  // phase angle in 0.1 degree
  void setPhaseAngle(int phase, _supla_int_t phaseAngle);

  void resetReadParameters();

  // Please implement this class for reading value from elecricity meter device.
  // It will be called every 5 s. Use set methods defined above in order to
  // set values on channel. Don't use any other method to modify channel values.
  virtual void readValuesFromDevice();

  // Put here initialization code for electricity meter device.
  // It will be called within SuplaDevce.begin method.
  // It should also read first data set, so at the end it should call those two
  // methods:
  // readValuesFromDevice();
  // updateChannelValues();
  void onInit();

  void iterateAlways();

  // Implement this method to reset stored energy value (i.e. to set energy
  // counter back to 0 kWh
  virtual void resetStorage();

  void setResreshRate(unsigned int sec);

  Channel *getChannel();

 protected:
  TElectricityMeter_ExtendedValue_V2 emValue;
  ChannelExtended extChannel;
  unsigned _supla_int_t rawCurrent[MAX_PHASES];
  bool valueChanged;
  bool currentMeasurementAvailable;
  unsigned long lastReadTime;
  unsigned int refreshRateSec;
};

};  // namespace Sensor
};  // namespace Supla

#endif

