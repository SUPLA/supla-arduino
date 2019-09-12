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

SuplaImpulseCounter * SuplaImpulseCounter::firstCounter = NULL;

SuplaImpulseCounter * SuplaImpulseCounter::getLastCounter() {
    SuplaImpulseCounter *ptr = firstCounter;
    if (ptr != NULL) {
        for (; ptr->nextCounter; ptr = ptr->nextCounter);
    }
    return ptr;
}

int SuplaImpulseCounter::count() {
    int counter = 0;
    SuplaImpulseCounter *ptr = firstCounter;
    if (firstCounter != NULL) {
        counter++;
        while (ptr->nextCounter != NULL) {
            counter++;
            ptr = ptr->nextCounter;
        }
    }
    return counter;
}

void SuplaImpulseCounter::create(int _impulsePin, int _statusLedPin, unsigned long _debounceDelay) {
    new SuplaImpulseCounter(_impulsePin, _statusLedPin, _debounceDelay);
}

void SuplaImpulseCounter::iterateAll() {
    SuplaImpulseCounter *ptr = firstCounter;
    while (ptr != NULL) {
        ptr->iterate();
        ptr = ptr->nextCounter;
    }
}

SuplaImpulseCounter::SuplaImpulseCounter(int _impulsePin, int _statusLedPin, unsigned long _debounceDelay) {
    // Init list of pointers to the first element
    if (firstCounter == NULL) {
        firstCounter = this;
    }
    // Add current instance to the last position on list
    nextCounter = NULL;
    SuplaImpulseCounter *ptr = getLastCounter();
    if (ptr != this) {
        ptr->nextCounter = this;
    }

    impulsePin = _impulsePin;
    statusLedPin = _statusLedPin;
    debounceDelay = _debounceDelay;
    lastImpulseMillis = 0;

    Serial.print("Initializing SuplaImpulseCounter with: impulsePin(");
    Serial.print(impulsePin);
    Serial.print("), statusLedPin(");
    Serial.print(statusLedPin);
    Serial.print("), delay(");
    Serial.print(debounceDelay);
    Serial.println(" ms)");
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

void SuplaImpulseCounter::iterate() {
   int currentState = digitalRead(impulsePin);
   if (prevState == LOW) {
       if (millis() - lastImpulseMillis > 50) {
           if (currentState == HIGH) {
               incCounter();
               lastImpulseMillis = millis();
           }
       }
   }
   prevState = currentState;

}

void SuplaImpulseCounter::incCounter() {
    counter++;
    debug();
}

