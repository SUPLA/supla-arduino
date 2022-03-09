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

#include "time.h"

#ifndef ARDUINO

#ifdef SUPLA_FREERTOS
// Plain FreeRTOS compilation
#include <FreeRTOS.h>
#include <task.h>

unsigned long millis(void) {
  if (portTICK_PERIOD_MS != 1) {
    // TODO
    // error
  }
  return xTaskGetTickCount();
}

void delay(unsigned long delayMs) {
// TODO  usleep(delayMs * 1000);
}

void delayMicroseconds(unsigned long delayMicro) {
// TODO usleep(delayMicro);
}

#elif defined(ESP_PLATFORM)
// ESP8266 RTOS SDK and ESP-IDF compilation
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unistd.h>

unsigned long millis(void) {
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void delay(unsigned long delayMs) {
  usleep(delayMs * 1000);
}

void delayMicroseconds(unsigned long delayMicro) {
  usleep(delayMicro);
}

#elif SUPLA_LINUX
#include <chrono>

std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

unsigned long millis() {
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
    .count();
}

void delay(unsigned long) {
// TODO
}

void delayMicroseconds(unsigned long delayMicro) {
// TODO
}

#else
#error "Please implement time functions for current target"
#endif

#endif
