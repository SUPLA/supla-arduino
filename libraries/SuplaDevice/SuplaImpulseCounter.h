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

#ifndef _supla_impulse_counter_h_
#define _supla_impulse_counter_h_

#include "supla-common/proto.h"

class SuplaImpulseCounter {
 public:
  // Returns how many impulse counters are used
  static int count();

  // Creates impulse counter (it is called automatically from SuplaDevice - no
  // need to use it manually)
  static void create(int _channelNumber,
                     int _impulsePin,
                     int _statusLedPin = 0,
                     bool _detectLowToHigh = false,
                     bool inputPullup = true,
                     unsigned long _debounceDelay = 10);

  // Iterates all impulse counters to check if there is new impulse to count
  // (used by SuplaDevice timer)
  static void iterateAll();

  // Returns value of a counter at given Supla channel
  _supla_int64_t getCounter();

  // use to clear internal EEPROM storage value (i.e. to reset counters to 0);
  static void clearStorage();

  // method used internally to store counters to EEPROM memory. You can call it
  // in your program to save current values of counters i.e. to initialize
  // counters with a given values
  static void writeToStorage();

  // Load counter values from EEPROM memory
  static void loadStorage();

  // Updates counters to EEPROM memory every 2 minutes
  static void updateStorageOccasionally();

  // Returns pointer to counter for a given Supla channel
  static SuplaImpulseCounter *getCounterByChannel(int channel);

  // Print debug value of impulse counter
  void debug();

  // Retuns Supla channel number for current impulse counter
  int getChannelNumber();

  // Set counter to a given value
  void setCounter(_supla_int64_t value);

  // Increment the counter by 1
  void incCounter();

  //
  bool isChanged();
  void clearIsChanged();

 protected:
  SuplaImpulseCounter(int _channelNumber,
                      int _impulsePin,
                      int _statusLedPin = 0,
                      bool _detectLowToHigh = false,
                      bool inputPullup = true,
                      unsigned long _debounceDelay = 10);
  static int
      instanceCounter;  // Stores how many instances of counters were created.
                        // It is used in EEPROM functions to read/write memory

  // Pointer to first counter
  static SuplaImpulseCounter *firstCounter;

  // Returns pointer to last counter
  static SuplaImpulseCounter *getLastCounter();

  // Pointer to next counter
  SuplaImpulseCounter *nextCounter;

  int channelNumber;  // Supla Channel number
  int prevState;  // Store previous state of pin (LOW/HIGH). It is used to track
                  // changes on pin state.
  int impulsePin;    // Pin where impulses are counted
  int statusLedPin;  // Pin used to connect additional LED to visually verify if
                     // impulses are counted correctly (value 0 can be used to
                     // not configure this pin)
  unsigned long
      lastImpulseMillis;  // Stores timestamp of last impulse (used to ignore
                          // changes of state during 10 ms timeframe)
  unsigned long debounceDelay;
  bool detectLowToHigh;  // defines if we count raining (LOW to HIGH) or falling
                         // (HIGH to LOW) edge
  bool isValueChanged;

  _supla_int64_t counter;  // Actual count of impulses

  // Method check current status of an impulse pin and updates counter
  void iterate();

};

#endif
