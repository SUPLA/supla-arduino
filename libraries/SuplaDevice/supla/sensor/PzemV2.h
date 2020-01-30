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

#ifndef _PzenV2_h
#define _PzemV2_h

#include <Arduino.h>
#include <PZEM004T.h>
#include <SoftwareSerial.h>
#include "supla/sensor/one_phase_electricity_meter.h"


namespace Supla {
namespace Sensor {

class PZEMv2 : public OnePhaseElectricityMeter {
    public:
        PZEMv2(int8_t pinRX,int8_t pinTX) : pzem(pinRX, pinTX), ip(192, 168, 1, 1) {
        }

        void onInit() {
            pzem.setAddress(ip);
            readValuesFromDevice();
            updateChannelValues();
        }

        virtual void readValuesFromDevice() {
            setVoltage(0, pzem.voltage(ip) * 100);
            setCurrent(0, pzem.current(ip) * 1000);
            setPowerActive(0, pzem.power(ip) * 100000);
            setFwdActEnergy(0, pzem.energy(ip) * 100);
        }


        PZEM004T pzem;
        IPAddress ip;


};
};  // namespace Sensor
};  // namespace Suplaa


#endif
