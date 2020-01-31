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

#ifndef _PzemV2_h
#define _PzemV2_h

#include <Arduino.h>
#include <PZEM004T.h>
#include <SoftwareSerial.h>
#include "one_phase_electricity_meter.h"


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
		
	 double current = pzem.current(ip);
	 double PowerFactor;
         double reactive;
         double voltage = pzem.voltage(ip);
	 double active = pzem.power(ip);
	 double apparent = (voltage * current);
            if (apparent > active) {reactive = sqrt(apparent * apparent - active * active);}
              else {reactive =  0; }
	 if (active > apparent) {PowerFactor = 1;}
              else if (apparent == 0) {PowerFactor = 0;}
	      else{PowerFactor = (active / apparent);}
		
            setVoltage(0, voltage * 100);
            setCurrent(0, current * 1000);
            setPowerActive(0, active * 100000);
            setFwdActEnergy(0, pzem.energy(ip) * 100);
	    setPowerApparent(0, apparent * 100000);
	    setPowerReactive(0, reactive * 10000);
	    setPowerFactor(0, PowerFactor * 1000);
        }


        PZEM004T pzem;
        IPAddress ip;


};
};  // namespace Sensor
};  // namespace Suplaa


#endif
