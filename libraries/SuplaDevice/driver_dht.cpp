#include <arduino.h>
#include <driver_dht.h>

DriverDHT * DriverDHT::firstDHT = NULL;

        
DriverDHT * DriverDHT::create(int channel, int pin, int DHTType) {
    return new DriverDHT(channel, pin, DHTType);
}

DriverDHT::DriverDHT(int _channel, int _pin, int _DHTType) {
    Serial.print("Creating DHT on channel ");
    Serial.print(_channel);
    Serial.print("; pin ");
    Serial.print(_pin);
    Serial.print("; DHTType ");
    Serial.print(_DHTType);
    Serial.println();

    channel = _channel;
    pin = _pin;
    DHTType = _DHTType;

    dhtPtr = new DHT(pin, DHTType);
    dhtPtr->begin();

    lastValidTemp = -275;
    lastValidHumi = -1;
    invalidReadCounter = 0;
    isValueChanged = true;

    if (firstDHT == NULL) {
        firstDHT = this;
    } else {
        getLastDHT()->nextDHT = this;
    }
    nextDHT = NULL;

}

DriverDHT * DriverDHT::getLastDHT() {
    DriverDHT *ptr = firstDHT;
    if (ptr != NULL) {
        for (; ptr->nextDHT; ptr = ptr->nextDHT);
    }
    return ptr;
}

bool DriverDHT::isChanged() {
    return isValueChanged;
}

DriverDHT * DriverDHT::getDhtByChannel(int _channel) {
    DriverDHT *ptr = firstDHT;

    while (ptr != NULL) {
        if (ptr->channel == _channel) break;
        ptr = ptr->nextDHT;
    }
    if (ptr && ptr->channel != _channel) {
        ptr = NULL;
    }
    return ptr;
}

void DriverDHT::update() {
    double temp = dhtPtr->readTemperature();
    double humi = dhtPtr->readHumidity();

    if (isnan(temp) || isnan(humi)) {
        invalidReadCounter++;
    } else {
        invalidReadCounter = 0;
    }

    // Ignoring 3 sequential invalid reads from DHT device 
    if (invalidReadCounter > 2) {
        if (isnan(temp)) {
            temp = -275;
        }
        if (isnan(humi)) {
            humi = -1;
        }
        invalidReadCounter = 0;
    }

    if (invalidReadCounter == 0) {
        if (temp != lastValidTemp || humi != lastValidHumi) {
            isValueChanged = true;
        }

        lastValidTemp = temp;
        lastValidHumi = humi;
    }

    Serial.print("DriverDHT channel ");
    Serial.print(channel);
    Serial.print("; temp = ");
    Serial.print(temp);
    Serial.print("; humi = ");
    Serial.print(humi);
    Serial.print("; changed? ");
    Serial.println(isValueChanged);
}

double DriverDHT::getTemp() {
    return lastValidTemp;
}

double DriverDHT::getHumi() {
    return lastValidHumi;
}

void DriverDHT::resetIsChanged() {
    isValueChanged = false;
}

