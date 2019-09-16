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

#include <proto.h>

class SuplaImpulseCounter {
    public:
        static int count();
        static void create(int _channelNumber, int _impulsePin, int _statusLedPin = 0, bool _detectLowToHigh = false, bool inputPullup = true, unsigned long _debounceDelay = 10);
        static void iterateAll();
        static _supla_int64_t getCounterValue(int _channel);  // returns value of counter for a given Supla channel number

        void debug();
        int getChannelNumber();

    protected:
        SuplaImpulseCounter(int _channelNumber, int _impulsePin, int _statusLedPin = 0, bool _detectLowToHigh = false, bool inputPullup = true, unsigned long _debounceDelay = 10);
        static int instanceCounter; // Stores how many instances of counters were created. It is used in EEPROM functions to read/write memory

        // Pointer to first counter
        static SuplaImpulseCounter *firstCounter;

        // Returns pointer to last counter
        static SuplaImpulseCounter * getLastCounter();

        // Pointer to next counter
        SuplaImpulseCounter *nextCounter;

        int channelNumber;  // Supla Channel number
        int prevState; // Store previous state of pin (LOW/HIGH). It is used to track changes on pin state.
        int impulsePin;   // Pin where impulses are counted
        int statusLedPin; // Pin used to connect additional LED to visually verify if impulses are counted correctly (value 0 can be used to not configure this pin)
        unsigned long lastImpulseMillis; // Stores timestamp of last impulse (used to ignore changes of state during 10 ms timeframe)
        unsigned long debounceDelay;
        bool detectLowToHigh; // defines if we count raining (LOW to HIGH) or falling (HIGH to LOW) edge

        _supla_int64_t counter;      // Actual count of impulses

        // Method check current status of an impulse pin and updates counter
        void iterate();

        // Set counter to a given value
        void setCounter(_supla_int64_t value);

        // Increment the counter by 1
        void incCounter();
        
};


#endif

