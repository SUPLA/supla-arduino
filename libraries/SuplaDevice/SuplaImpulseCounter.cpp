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

#include <Arduino.h>
#include <SuplaImpulseCounter.h>
#include <EEPROM.h>

SuplaImpulseCounter::SuplaImpulseCounter(int _impulsePin, int _statusLedPin) {
    impulsePin = _impulsePin;
    statusLedPin = _statusLedPin;

    Serial.print("Initializing SuplaImpulseCounter with: impulsePin(");
    Serial.print(impulsePin);
    Serial.print("), statusLedPin(");
    Serial.print(statusLedPin);
    Serial.println(")");
    if (impulsePin <= 0) {
        Serial.println("SuplaImpulseCounter ERROR - incorrect impulse pin number");
        return;
    } 

    pinMode(impulsePin, INPUT_PULLUP);

    if (statusLedPin <= 0) {
        Serial.println("SuplaImpulseCounter - status LED disabled");
        statusLedPin = 0;
    } else {
        pinMode(statusLedPin, OUTPUT);
    }

    setCounter(0);

}

void SuplaImpulseCounter::debug() {
    Serial.print("SuplaImpulseCounter - impulsePin(");
    Serial.print(impulsePin);
    Serial.print("), counter(");
    Serial.print(counter);
    Serial.println(")");
}

void SuplaImpulseCounter::setCounter(int value) {
    counter = value;
    Serial.print("SuplaImpulseCounter - set counter to ");
    Serial.println(counter);
}

