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

/*
 Example of empty supla-device project for ESP8266 with EPS8266 RTOS SDK
 */


#include <FreeRTOS.h>
#include <esp_idf_wifi.h>
#include <supla-common/log.h>
#include <SuplaDevice.h>
#include <supla/control/virtual_relay.h>
#include <supla/time.h>
#include "esp_heap_caps.h"
#include <supla/control/rgb_leds.h>
#include <supla/control/rgbw_leds.h>
#include <supla/control/dimmer_leds.h>
#include <supla/control/roller_shutter.h>


extern "C" void cpp_main(void*);

void cpp_main(void* param)
{

  Supla::EspIdfWifi wifi("wifi_ssid", "wifi_password");

  // Replace the falowing GUID with value that you can retrieve from
  // https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] =
   {0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00};

  // Replace the following AUTHKEY with value that you can retrieve from:
  // https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] =
   {0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00};

  auto r1 = new Supla::Control::VirtualRelay();
  auto r2 = new Supla::Control::VirtualRelay();

  new Supla::Control::RGBLeds(1,2,3);
  new Supla::Control::RGBLeds(4,5,6);
  new Supla::Control::DimmerLeds(7);
  new Supla::Control::RGBWLeds(8,9,10,11);
  new Supla::Control::RollerShutter(12, 13, true);


  supla_log(LOG_DEBUG, "Free heep: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
  SuplaDevice.setServerPort(2016);
  SuplaDevice.begin(GUID, "svrX.supla.org", "happy@supla.org", AUTHKEY);
  supla_log(LOG_DEBUG, "Free heep: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

  unsigned int lastTime = 0;
  unsigned int lastTimeHeap = 0;
  supla_log(LOG_DEBUG, "Starting main loop");
  while (true) {
    SuplaDevice.iterate();
    if (millis() - lastTime > 10) {
      vTaskDelay(1);
      lastTime = millis();
    }
    if (millis() - lastTimeHeap > 3000) {
      lastTimeHeap = millis();
      supla_log(LOG_DEBUG, "Free heep: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    }
  }
}

extern "C" {
  void app_main() {
    xTaskCreate(&cpp_main, "cpp_main", 8192, NULL, 5, NULL);
  }
}

