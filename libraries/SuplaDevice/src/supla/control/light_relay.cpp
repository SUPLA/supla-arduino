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

/* Relay class
 * This class is used to control any type of relay that can be controlled
 * by setting LOW or HIGH output on selected GPIO.
 */

#include "light_relay.h"

using namespace Supla;
using namespace Control;

LightRelay::LightRelay(int pin, bool highIsOn = true)
    : Relay(pin,
            highIsOn,
            SUPLA_BIT_FUNC_LIGHTSWITCH | SUPLA_BIT_FUNC_STAIRCASETIMER) {
  channel.setFlag(SUPLA_CHANNEL_FLAG_LIGHTSOURCELIFESPAN_SETTABLE);
}

void LightRelay::handleGetChannelState(TDSC_ChannelState &channelState) {
  channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_LIGHTSOURCELIFESPAN;
  channelState.LightSourceLifespan = 1000;
  channelState.LightSourceLifespanLeft = 500;
}

/*
 * 53 55 50 4C 41 SUPLA
 * C 7 0 0 0 
 * CC 1 0 0 -  SUPLA_SD_CALL_DEVICE_CALCFG_REQUEST
 * 19 0 0 0 sender id
 * 34 10 0 0 channel numbwer
 * 5 0 0 0 cmd
 * 70 17 0 0 result?
 * 1 0 ]
 * 0 0 0 4 0 0 0 
 * =
 * 1 0 E8 3 
 * 53 55 50 4C 41  SUPLA
 * 11:30:23.071 -> Received unknown message from server!
 * (reset + 1000)
 *                                                     0x28         SIZE      SENDER     CHANNEL      CMD     AUTH
 * 11:31:06.679 -> Received: [53 55 50 4C 41 C B 0 0 0 CC 1 0 0 | 19 0 0 0 | 34 10 0 0 | 5 0 0 0 | 70 17 0 0 |  1  | 
 * DATA type?   DATA size
 * 0 0 0 0    | 4 0 0 0   | 0 1 10 27  | 53 55 50 4C 41 ]
 * (no reset + 10000)
 *
 *   TCS_DeviceCalCfgRequest ( TCalCfg_LightSourceLifespan )
 *
 */
