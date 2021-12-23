#ifndef _hygro_meter_h
#define _hygro_meter_h

#include "thermometer.h"

#define HUMIDITY_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class HygroMeter : public Thermometer {
 public:
  HygroMeter();
  virtual double getHumi();
  void iterateAlways();

};

};  // namespace Sensor
};  // namespace Supla

#endif

