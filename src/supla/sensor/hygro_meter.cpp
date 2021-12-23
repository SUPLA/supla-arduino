#include "hygro_meter.h"

Supla::Sensor::HygroMeter::HygroMeter() {
  channel.setType(SUPLA_CHANNELTYPE_HUMIDITYSENSOR);
  channel.setDefault(SUPLA_CHANNELFNC_HUMIDITY);
}

double Supla::Sensor::HygroMeter::getHumi() {
  return HUMIDITY_NOT_AVAILABLE;
}

void Supla::Sensor::HygroMeter::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    lastReadTime = millis();
    channel.setNewValue(TEMPERATURE_NOT_AVAILABLE, getHumi());
  }
}

