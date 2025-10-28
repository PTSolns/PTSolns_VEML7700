#pragma once
#include <Arduino.h>
#include <Wire.h>

#define VEML7700_I2C_ADDR 0x10

enum VEML7700Status : uint8_t {
  VEML_OK = 0,
  VEML_I2CError,
  VEML_NotInitialized,
  VEML_InvalidParam
};

enum VEML7700Gain : uint8_t {
  VEML_GAIN_1_8 = 0,
  VEML_GAIN_1_4 = 1,
  VEML_GAIN_1    = 2,
  VEML_GAIN_2    = 3
};

enum VEML7700IT : uint8_t {
  VEML_IT_25MS  = 0,
  VEML_IT_50MS  = 1,
  VEML_IT_100MS = 2,
  VEML_IT_200MS = 3,
  VEML_IT_400MS = 4,
  VEML_IT_800MS = 5
};

class PTSolns_VEML7700 {
public:
  PTSolns_VEML7700() {}
  bool begin(TwoWire& w = Wire, uint8_t i2cAddr = VEML7700_I2C_ADDR, uint32_t i2cClockHz = 100000);
  VEML7700Status readALSRaw(uint16_t& als);
  VEML7700Status readWhiteRaw(uint16_t& white);
  VEML7700Status readLux(float& lux);
  void enableAutoRange(bool en) { _autorange = en; }
  bool autoRangeEnabled() const { return _autorange; }
  VEML7700Status setGain(VEML7700Gain g);
  VEML7700Status setIntegrationTime(VEML7700IT it);
  VEML7700Status setPowerSave(bool enable, uint8_t mode = 0);
  VEML7700Status shutdown(bool en);
  VEML7700Status enableInterrupt(bool en);
  VEML7700Status setHighThreshold(uint16_t th);
  VEML7700Status setLowThreshold(uint16_t th);
  VEML7700Status readInterruptStatus(uint16_t& istat);
  uint8_t address() const { return _addr; }
  VEML7700Gain gain() const { return _gain; }
  VEML7700IT integrationTime() const { return _it; }
  VEML7700Status lastError() const { return _last; }
private:
  static const uint8_t REG_ALS_CONF   = 0x00;
  static const uint8_t REG_ALS_WH     = 0x01;
  static const uint8_t REG_ALS_WL     = 0x02;
  static const uint8_t REG_ALS_PSM    = 0x03;
  static const uint8_t REG_ALS_DATA   = 0x04;
  static const uint8_t REG_WHITE_DATA = 0x05;
  static const uint8_t REG_ALS_INT    = 0x06;
  VEML7700Status write16_(uint8_t reg, uint16_t value);
  VEML7700Status read16_(uint8_t reg, uint16_t& value);
  float          resolutionLuxPerCount_() const;
  VEML7700Status applyConfig_();
  VEML7700Status maybeAutoRange_(uint16_t lastAls);
  TwoWire* _wire = nullptr;
  uint8_t  _addr = VEML7700_I2C_ADDR;
  bool     _inited = false;
  bool     _autorange = false;
  VEML7700Gain _gain = VEML_GAIN_1;
  VEML7700IT   _it   = VEML_IT_100MS;
  VEML7700Status _last = VEML_OK;
};
