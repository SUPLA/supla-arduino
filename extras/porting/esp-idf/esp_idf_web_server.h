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

#ifndef _supla_esp_idf_web_server_h_
#define _supla_esp_idf_web_server_h_

#include <supla/network/web_server.h>
#include <supla/network/web_sender.h>
#include <esp_http_server.h>

namespace Supla {

class EspIdfSender : public Supla::WebSender {
  public:
    EspIdfSender(httpd_req_t *req);
    ~EspIdfSender();
    void send(const char*, int) override;

  protected:
    httpd_req_t* reqHandler;
};

class EspIdfWebServer : public Supla::WebServer {
  public:
    EspIdfWebServer(HtmlGenerator *generator = nullptr);
    virtual ~EspIdfWebServer();
    virtual void start() override;
    virtual void stop() override;

    bool dataSaved = false;
  protected:
    httpd_handle_t server;
};

};

#endif /*_supla_esp_idf_web_server_h_*/

