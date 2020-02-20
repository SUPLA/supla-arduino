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

#ifndef _PzemV3_h
#define _PzemV3_h

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include "one_phase_electricity_meter.h"



namespace Supla {
namespace Sensor {

class PZEMv3 : public OnePhaseElectricityMeter {
    public:
        PZEMv3(int8_t pinRX,int8_t pinTX) : pzem(pinRX, pinTX) {
        }

        void onInit() {
            readValuesFromDevice();
            updateChannelValues();
        }

       virtual void readValuesFromDevice() {
	       
	      	double current = pzem.current();
    	 	double voltage = pzem.voltage();
	      	double active = pzem.power();
	      	double apparent = (voltage * current);
      		double reactive;
	      	if (apparent > active) {reactive = sqrt(apparent * apparent - active * active);}
    	     	   else {reactive =  0; }
		
        	setVoltage(0, voltage * 100);  
        	setCurrent(0, current * 1000);
        	setPowerActive(0,  active * 100000); 
        	setFwdActEnergy(0, pzem.energy() * 100000); 
        	setFreq( pzem.frequency() * 100);
        	setPowerFactor(0, pzem.pf() * 1000);
      		setPowerApparent(0, apparent * 100000);
	      	setPowerReactive(0, reactive* 10000);
        }
        
        PZEM004Tv30 pzem;

};

};  // namespace Sensor
};  // namespace Suplaa


#endif
