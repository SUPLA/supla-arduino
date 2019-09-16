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

void SuplaImpulseCounter::create(int _channelNumber, int _impulsePin, int _statusLedPin, bool _detectLowToHigh, bool _inputPullup, unsigned long _debounceDelay) {
    new SuplaImpulseCounter(_channelNumber, _impulsePin, _statusLedPin, _detectLowToHigh, _inputPullup, _debounceDelay);
}

void SuplaImpulseCounter::iterateAll() {
    SuplaImpulseCounter *ptr = firstCounter;
    while (ptr != NULL) {
        ptr->iterate();
        ptr = ptr->nextCounter;
    }
}


_supla_int64_t SuplaImpulseCounter::getCounterValue(int _channel) {
    _supla_int64_t result = 0;
    SuplaImpulseCounter *ptr = firstCounter;
    while (ptr != NULL) {
        if (ptr->getChannelNumber() == _channel) {
            result = ptr->counter;
            break;
        }
    }
    return result;
}

SuplaImpulseCounter::SuplaImpulseCounter(int _channelNumber, int _impulsePin, int _statusLedPin, bool _detectLowToHigh, bool _inputPullup, unsigned long _debounceDelay) {
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

    channelNumber = _channelNumber;
    impulsePin = _impulsePin;
    statusLedPin = _statusLedPin;
    debounceDelay = _debounceDelay;
    detectLowToHigh = _detectLowToHigh;
    lastImpulseMillis = 0;
    prevState = HIGH;

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

    if (_inputPullup) {
        pinMode(impulsePin, INPUT_PULLUP);
    } else {
        pinMode(impulsePin, INPUT);
    }

    if (statusLedPin <= 0) {
        Serial.println("SuplaImpulseCounter - status LED disabled");
        statusLedPin = 0;
    } else {
        pinMode(statusLedPin, OUTPUT);
    }

    setCounter(0);

}

void SuplaImpulseCounter::debug() {
    Serial.print("SuplaImpulseCounter - channel(");
    Serial.print(channelNumber);
    Serial.print("), impulsePin(");
    Serial.print(impulsePin);
    Serial.print("), counter(");
    Serial.print(static_cast<int>(counter));
    Serial.println(")");
}

void SuplaImpulseCounter::setCounter(_supla_int64_t value) {
    counter = value;
    Serial.print("SuplaImpulseCounter - set counter to ");
    Serial.println(static_cast<int>(counter));
}

void SuplaImpulseCounter::iterate() {
   int currentState = digitalRead(impulsePin);
   if (prevState == (detectLowToHigh == true ? LOW : HIGH)) {
       if (millis() - lastImpulseMillis > debounceDelay) {
           if (currentState == (detectLowToHigh == true ? HIGH : LOW)) {
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

int SuplaImpulseCounter::getChannelNumber() {
    return channelNumber;
}
