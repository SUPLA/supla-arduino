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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <syslog.h>

#include "log.h"

extern int runAsDaemon;
extern int logLevel;

void supla_vlog(int __pri, const char *message) {
  if (__pri > logLevel) {
    return;
  }

  if (message == NULL) {
    return;
  }

  if (runAsDaemon == 1) {
    syslog(__pri, "%s", message);
  } else {
    switch (__pri) {
      case LOG_EMERG:
        printf("EMERG");
        break;
      case LOG_ALERT:
        printf("ALERT");
        break;
      case LOG_CRIT:
        printf("CRIT");
        break;
      case LOG_ERR:
        printf("ERR");
        break;
      case LOG_WARNING:
        printf("WARNING");
        break;
      case LOG_NOTICE:
        printf("NOTICE");
        break;
      case LOG_INFO:
        printf("INFO");
        break;
      case LOG_DEBUG:
        printf("DEBUG");
        break;
      case LOG_VERBOSE:
        printf("VERBOSE");
        break;
    }

    struct timeval now = {};
    gettimeofday(&now, NULL);
    printf("[%li.%li] ", (uint64_t)now.tv_sec, (uint64_t)now.tv_usec);
    printf("%s", message);
    printf("\n");
    fflush(stdout);
  }
}
