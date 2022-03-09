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

#include <iostream>
#include <supla-common/log.h>
#include <supla/time.h>
#include <SuplaDevice.h>
#include <linux_network.h>

#include <supla/control/virtual_relay.h>
#include <supla/control/rgb_leds.h>
#include <supla/control/dimmer_leds.h>
#include <supla/control/rgbw_leds.h>

#include <unistd.h>

// Below includes are added just for CI compilation check. Some of them
// are not used in any cpp file, so they would not be compiled otherwise.
// Remove them and keep only required one in real application.
#include <supla/sensor/binary.h>
#include <supla/sensor/distance.h>
#include <supla/sensor/electricity_meter.h>
#include <supla/sensor/general_purpose_measurement_base.h>
#include <supla/sensor/hygro_meter.h>
#include <supla/sensor/normally_open.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/sensor/pressure.h>
#include <supla/sensor/rain.h>
#include <supla/sensor/therm_hygro_meter.h>
#include <supla/sensor/therm_hygro_press_meter.h>
#include <supla/sensor/thermometer.h>
#include <supla/sensor/virtual_binary.h>
#include <supla/sensor/weight.h>
#include <supla/sensor/wind.h>
#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/at_channel.h>
#include <supla/channel.h>
#include <supla/channel_element.h>
#include <supla/channel_extended.h>
#include <supla/condition.h>
#include <supla/condition_getter.h>
#include <supla/correction.h>
#include <supla/crc16.h>
#include <supla/definitions.h>
#include <supla/element.h>
#include <supla/events.h>
#include <supla/IEEE754tools.h>
#include <supla/io.h>
#include <supla/local_action.h>
#include <supla/status.h>
#include <supla/supla_lib_config.h>
#include <supla/time.h>
#include <supla/timer.h>
#include <supla/tools.h>
#include <supla/uptime.h>

int main() {
  supla_log(LOG_DEBUG, "Hello Linux with Supla!");

  Supla::LinuxNetwork network;

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
  auto r3 = new Supla::Control::VirtualRelay();
  auto r4 = new Supla::Control::VirtualRelay();

  new Supla::Control::RGBWLeds(1,2,3,4);
  new Supla::Control::RGBLeds(1,2,3);
  new Supla::Control::DimmerLeds(1);

  new Supla::Control::RGBWLeds(1,2,3,4);
  new Supla::Control::RGBLeds(1,2,3);
  SuplaDevice.setServerPort(2016);
  SuplaDevice.begin(GUID, "svrX.supla.org", "happy@supla.org", AUTHKEY);

  while (true) {
    SuplaDevice.iterate();
    usleep(100);
  }

  supla_log(LOG_DEBUG, "we are %d ms since app start", millis());

  return 0;
}
