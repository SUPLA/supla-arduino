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

#ifndef _supla_clock_h
#define _supla_clock_h

#include <time.h>
#include <supla-common/proto.h>
#include <supla/element.h>

namespace Supla {

class Clock : public Element {
  public:
  Clock();
  virtual bool isReady();
  virtual int getYear();
  virtual int getMonth();
  virtual int getDay();
  virtual int getDayOfWeek(); // 1 - Sunday, 2 - Monday
  virtual int getHour();
  virtual int getMin();
  virtual int getSec();

  void onTimer();
  bool iterateConnected(void *srpc);

  virtual void parseLocaltimeFromServer(TSDC_UserLocalTimeResult *result);

  protected:
  time_t localtime;
  unsigned long lastServerUpdate;
  unsigned long lastMillis;
 
};

};

#endif
