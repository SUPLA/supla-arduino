/*
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

#ifndef _supla_impulse_counter_h_
#define _supla_impulse_counter_h_

class SuplaImpulseCounter {
    public:
        SuplaImpulseCounter(int _impulsePin, int _statusLedPin = 0);

        void debug();

    protected:
        static int instanceCounter; // Stores how many instances of counters were created. It is used in EEPROM functions to read/write memory
        static *SuplaImpulseCounter firstCounter;

        *SuplaImpulseCounter nextCounter;
        int prevPinStatus;
        int impulsePin;
        int statusLedPin;

        int counter; 

        void iterate();

        void setCounter(int value);
        
};


#endif

