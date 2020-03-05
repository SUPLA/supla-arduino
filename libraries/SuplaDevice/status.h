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

#ifndef _supla_status_h
#define _supla_status_h

namespace Supla {
class Status {
 public:
   void notInitialized();
   void initialized();

   void alreadyInitialized();
   void channelLimitExceeded();
   void missingNetworkInterface();
   void invalidGuid();
   void missingCredentials();
   void unknownServerAddress();

   void connectingToNetworkInterface();
   void connectingToSuplaServer();
   void registerInProgress();
   void registeredAndReady();

   void networkDisconnected();
   void serverDisconnected();


   void iterateFail();
   void protocolVersionError();
   void badCredentials();
   void temporarilyUnavailable();
   void locationConflict();
   void channelConflict();
   void deviceIsDisabled();
   void locationIsDisabled();
   void deviceLimitExceeded();
   void registrationDisabled();

   
};
};  // namespace Supla

#endif
