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
#include <supla/version.h>
#include <supla-common/tools.h>

#include <supla/control/virtual_relay.h>
#include <supla/control/rgb_leds.h>
#include <supla/control/dimmer_leds.h>
#include <supla/control/rgbw_leds.h>

#include <unistd.h>
#include <cxxopts.hpp>

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
#include <supla/pv/fronius.h>
#include <supla/pv/afore.h>

#include <fstream>

#include <supla/sha256.h>
#include <supla/rsa_verificator.h>

#include <supla/sensor/thermometer_parsed.h>
#include <supla/sensor/impulse_counter_parsed.h>
#include <supla/sensor/electricity_meter_parsed.h>
#include <supla/parser/simple.h>
#include <supla/parser/json.h>
#include <supla/source/cmd.h>
#include <supla/source/file.h>
#include <supla/sensor/binary_parsed.h>

// reguired by linux_log.c
int logLevel = LOG_INFO;
int runAsDaemon = 0;

int main(int argc, char *argv[]) {
  try {
    cxxopts::Options options(argv[0], "Supla device client. See www.supla.org");

    options.add_options()
      ("D,debug", "Enable debug logs")
      ("V,verbose", "Enable verbose debug logs", cxxopts::value<bool>()->default_value("false"))
      ("i,integer", "Int param", cxxopts::value<int>())
      ("c,config", "Config file name", cxxopts::value<std::string>()->default_value("etc/supla-device.cfg"))
      ("d,daemon", "Run in daemon mode (run in background and log to syslog)")
      ("h,help", "Show this help")
      ("v,version", "Show version")
      ;

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    if (result.count("version"))
    {
      std::cout << argv[0] << " version: " << suplaDeviceVersion << std::endl;
      exit(0);
    }


    if (result.count("debug")) {
      logLevel = LOG_DEBUG;
    }

    if (result.count("verbose")) {
      logLevel = LOG_VERBOSE;
    }

    if (result.count("daemon")) {
      runAsDaemon = 1;
      if (!st_try_fork()) {
        supla_log(LOG_ERR, "Can't start daemon");
        exit(1);
      }
    }

    st_hook_signals();

    std::string cfgFile = result["config"].as<std::string>();
    supla_log(LOG_INFO, "Using config file %s", cfgFile.c_str());
    if (!st_file_exists(cfgFile.c_str())) {
      supla_log(LOG_ERR, "Config file does not exists");
//      exit(1);
    }


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

    while (st_app_terminate == 0) {
      SuplaDevice.iterate();
      delay(10);
    }
    supla_log(LOG_INFO, "Exit");
    exit(0);

  } catch (const cxxopts::OptionException& e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

