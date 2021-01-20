#ifndef _ntc10k_h
#define _ntc10k_h

#include <Arduino.h>

#include "supla/sensor/thermometer.h"

namespace Supla {
namespace Sensor {

class NTC10K : public Thermometer {
public:
  NTC10K(int8_t pin, uint32_t Rs, double Vcc) {
    _Pin = pin;
    _Vcc = Vcc;
    _Rs = Rs;
  }

  double getTemp() {
    int val = 0;
    for (int i = 0; i < 10; i++) {
      val += analogRead(_Pin);
      delay(1);
    }
    val = val / 10;
    double V_NTC = (double)val / 1023;
    double R_NTC = (_Rs * V_NTC) / (_Vcc - V_NTC);
    R_NTC = log(R_NTC);
    double t = 1 / (0.001129148 +
                    (0.000234125 + (0.0000000876741 * R_NTC * R_NTC)) * R_NTC);					
    t = t - 276.15;
    return t;
  }

  void iterateAlways() {
    if (lastReadTime + 10000 < millis()) {
      lastReadTime = millis();
      channel.setNewValue(getTemp());
    }
  }

  void onInit() { channel.setNewValue(getTemp()); }

protected:
  int8_t _Pin;
  double _Vcc;
  uint32_t _Rs;
};

}; // namespace Sensor
}; // namespace Supla

#endif
