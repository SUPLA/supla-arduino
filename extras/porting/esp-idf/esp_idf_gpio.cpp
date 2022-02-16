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

#include <supla-common/log.h>
#include <supla/io.h>

#ifndef ESP_PLATFORM
#error This file is for ESP-IDF platform
#endif

void pinMode(uint8_t pin, uint8_t mode) {
  supla_log(LOG_DEBUG, " *** GPIO %d set mode %d", pin, mode);
}

int digitalRead(uint8_t pin) {
  return 0;
}

void digitalWrite(uint8_t pin, uint8_t val) {
  supla_log(LOG_DEBUG, " *** GPIO %d digital write %d", pin, val);
}

void analogWrite(uint8_t pin, int val) {
  supla_log(LOG_DEBUG, " *** GPIO %d analog write %d", pin, val);
}

unsigned int pulseIn(uint8_t pin, uint8_t val, unsigned long timeoutMicro) {
  supla_log(LOG_DEBUG, " *** GPIO %d pulse in %d", pin, val);
  return 0;
}
