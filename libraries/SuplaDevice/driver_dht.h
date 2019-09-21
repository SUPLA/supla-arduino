#ifndef _DRIVER_DHT_H
#define _DRIVER_DHT_H

#include <DHT.h>

class DriverDHT {
    public:
        static DriverDHT * create(int channel, int pin, int DHTType);

        bool isChanged();
        void resetIsChanged();

        double getTemp();
        double getHumi();
        
        void update();

        static DriverDHT * getDhtByChannel(int _channel);

    protected:
        static DriverDHT *firstDHT;
        DriverDHT *nextDHT;
        int pin;
        int channel;
        int DHTType;
        bool isValueChanged;

        double lastValidTemp;
        double lastValidHumi;
        int invalidReadCounter;

        DHT *dhtPtr;

        DriverDHT(int _channel, int _pin, int _DHTType);
        static DriverDHT * getLastDHT();
};

#endif
