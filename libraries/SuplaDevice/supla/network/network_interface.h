#ifndef _network_interface_h
#define _network_interface_h

namespace Supla {
    class NetworkInterface {
        public:
            virtual long read(void *buf, _supla_int_t count) = 0;
            virtual long write(void *buf, _supla_int_t count) = 0;
            bool connect(const char *server, long port) = 0;
            bool connected() = 0;
            void disconnect() = 0;
            void setup(uint8_t mac[6], IPAddress *ip) = 0;
    };

};

#endif

