# CHANGELOG.md

## 2.4.1 (unreleased)

  - (Arduino) Change: move WiFi events for ESP8266 Arduino WiFi class to protected section
  - (Arduino) Change: Arduino ESP32 boards switch to version 2.x. Older boards will not compile ([see instructions](https://github.com/SUPLA/supla-device/commit/c533e73a4c811c026187374635dd812d4e294c8b))
  - (all) Add: `Supla::Device::StatusLed` element (LED informing about currend device connection status)
  - (Linux) Add: support for compilation under Linux environment (partial implementation). Provided docker file for environment setup and example application.
  - (FreeRTOS) Add: support for compilation under FreeRTOS environment (partial implementation). Provided docker file for environment setup and example application.
  - (ESP8266 RTOS) Add: support for ESP8266 RTOS SDK envirotnment (partial implementation). Provided docker file for environment setup and example application.
  - (ESP IDF) Add: support for ESP IDF envirotnment (partial implementation). Provided docker file for environment setup and example application.
  - (all) Add: `Supla::Io::pulseIn` and `Supla::Io::analogWrite` to interface
  - (ESP8266 RTOS, ESP IDF) Add: logging via ESP IDF logging lib
  - (ESP8266 RTOS, ESP IDF) Add: implementation for `delay`, `delayMicroseconds`, `millis`
  - (ESP8266 RTOS, ESP IDF) Add: implementation for `pinMode`, `digitalRead`, `digitalWrite`
  - (ESP8266 RTOS, ESP IDF, Linux, FreeRTOS) Add: `ChannelState` basic data reporting
  - (ESP8266 RTOS, ESP IDF) Add: `Supla::Storage` implementation on `spiffs`
  
## 2.4.0 (2021-12-07)

All changes for this and older releases are for Arduino IDE target.

  - Add: Action Trigger support
  - Add: Conditions for Electricity meter
  - Add: MAX thermocouple
  - Change: Supla protocol version updated to 16
  - Fix: Roller shutter was sending invalid channel value for not calibrated state
  - Fix: Compilaton error and warnings cleanup (especially for ESP32)
