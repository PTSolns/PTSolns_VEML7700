#include "PTSolns_VEML7700.h"

bool PTSolns_VEML7700::begin(TwoWire& w, uint8_t i2cAddr, uint32_t i2cClockHz) {
  _wire = &w;
  _addr = i2cAddr;
  _wire->begin();
#if defined(TWBR) || ARDUINO >= 100
  if (i2cClockHz >= 100000UL && i2cClockHz <= 400000UL) {
    _wire->setClock(i2cClockHz);
  }
#endif
  _gain = VEML_GAIN_1;
  _it   = VEML_IT_100MS;
  _autorange = false;
  VEML7700Status st = applyConfig_();
  _inited = (st == VEML_OK);
  _last = st;
  return _inited;
}

VEML7700Status PTSolns_VEML7700::applyConfig_() {
  uint16_t conf = 0;
  switch (_it) {
    case VEML_IT_25MS:  conf |= (0x0 << 4); break;
    case VEML_IT_50MS:  conf |= (0x1 << 4); break;
    case VEML_IT_100MS: conf |= (0x2 << 4); break;
    case VEML_IT_200MS: conf |= (0x3 << 4); break;
    case VEML_IT_400MS: conf |= (0x4 << 4); break;
    case VEML_IT_800MS: conf |= (0x5 << 4); break;
  }
  uint16_t gainBits = 0;
  switch (_gain) {
    case VEML_GAIN_1_8: gainBits = 0b11; break;
    case VEML_GAIN_1_4: gainBits = 0b10; break;
    case VEML_GAIN_1:   gainBits = 0b00; break;
    case VEML_GAIN_2:   gainBits = 0b01; break;
  }
  conf |= (gainBits << 9);
  return write16_(REG_ALS_CONF, conf);
}

VEML7700Status PTSolns_VEML7700::setGain(VEML7700Gain g) {
  _gain = g;
  return _last = applyConfig_();
}

VEML7700Status PTSolns_VEML7700::setIntegrationTime(VEML7700IT it) {
  _it = it;
  return _last = applyConfig_();
}

VEML7700Status PTSolns_VEML7700::shutdown(bool en) {
  uint16_t conf;
  VEML7700Status st = read16_(REG_ALS_CONF, conf);
  if (st != VEML_OK) return _last = st;
  if (en) conf |= 0x0001;
  else    conf &= ~0x0001;
  return _last = write16_(REG_ALS_CONF, conf);
}

VEML7700Status PTSolns_VEML7700::setPowerSave(bool enable, uint8_t mode) {
  if (mode > 3) mode = 3;
  uint16_t val = 0;
  if (enable) {
    val |= 0x0001;               // PSM_EN bit
    val |= ((uint16_t)mode << 1);// PSM mode bits
  }
  return _last = write16_(REG_ALS_PSM, val);
}

VEML7700Status PTSolns_VEML7700::enableInterrupt(bool en) {
  uint16_t conf;
  VEML7700Status st = read16_(REG_ALS_CONF, conf);
  if (st != VEML_OK) return _last = st;
  if (en) conf |=  (1u << 1);
  else    conf &= ~(1u << 1);
  st = write16_(REG_ALS_CONF, conf);
  return _last = st;
}

VEML7700Status PTSolns_VEML7700::setHighThreshold(uint16_t th) {
  return _last = write16_(REG_ALS_WH, th);
}

VEML7700Status PTSolns_VEML7700::setLowThreshold(uint16_t th) {
  return _last = write16_(REG_ALS_WL, th);
}

VEML7700Status PTSolns_VEML7700::readInterruptStatus(uint16_t& istat) {
  return _last = read16_(REG_ALS_INT, istat);
}

VEML7700Status PTSolns_VEML7700::readALSRaw(uint16_t& als) {
  if (!_inited) return _last = VEML_NotInitialized;
  return _last = read16_(REG_ALS_DATA, als);
}

VEML7700Status PTSolns_VEML7700::readWhiteRaw(uint16_t& white) {
  if (!_inited) return _last = VEML_NotInitialized;
  return _last = read16_(REG_WHITE_DATA, white);
}

float PTSolns_VEML7700::resolutionLuxPerCount_() const {
  float base = 0.0576f; // lux per count at Gain 1, IT 100 ms
  float gScale = 1.0f;
  switch (_gain) {
    case VEML_GAIN_2:   gScale = 0.5f;  break;
    case VEML_GAIN_1:   gScale = 1.0f;  break;
    case VEML_GAIN_1_4: gScale = 4.0f;  break;
    case VEML_GAIN_1_8: gScale = 8.0f;  break;
  }
  float itScale = 1.0f;
  switch (_it) {
    case VEML_IT_25MS:  itScale = 4.0f;   break;
    case VEML_IT_50MS:  itScale = 2.0f;   break;
    case VEML_IT_100MS: itScale = 1.0f;   break;
    case VEML_IT_200MS: itScale = 0.5f;   break;
    case VEML_IT_400MS: itScale = 0.25f;  break;
    case VEML_IT_800MS: itScale = 0.125f; break;
  }
  return base * gScale * itScale;
}

VEML7700Status PTSolns_VEML7700::maybeAutoRange_(uint16_t lastAls) {
  if (!_autorange) return VEML_OK;
  if (lastAls > 0xFF00) {
    if (_gain == VEML_GAIN_2)   { _gain = VEML_GAIN_1;   return applyConfig_(); }
    if (_gain == VEML_GAIN_1)   { _gain = VEML_GAIN_1_4; return applyConfig_(); }
    if (_gain == VEML_GAIN_1_4) { _gain = VEML_GAIN_1_8; return applyConfig_(); }
    if (_it != VEML_IT_25MS)    { _it = (VEML7700IT)((uint8_t)_it - 1); return applyConfig_(); }
  } else if (lastAls < 100) {
    if (_gain == VEML_GAIN_1_8) { _gain = VEML_GAIN_1_4; return applyConfig_(); }
    if (_gain == VEML_GAIN_1_4) { _gain = VEML_GAIN_1;   return applyConfig_(); }
    if (_gain == VEML_GAIN_1)   { _gain = VEML_GAIN_2;   return applyConfig_(); }
    if (_it != VEML_IT_800MS)   { _it = (VEML7700IT)((uint8_t)_it + 1); return applyConfig_(); }
  }
  return VEML_OK;
}

VEML7700Status PTSolns_VEML7700::readLux(float& lux) {
  uint16_t als;
  VEML7700Status st = readALSRaw(als);
  if (st != VEML_OK) return _last = st;
  maybeAutoRange_(als);
  float res = resolutionLuxPerCount_();
  lux = als * res;
  return _last = VEML_OK;
}

VEML7700Status PTSolns_VEML7700::write16_(uint8_t reg, uint16_t value) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(value & 0xFF);
  _wire->write((value >> 8) & 0xFF);
  if (_wire->endTransmission() != 0) return VEML_I2CError;
  return VEML_OK;
}

VEML7700Status PTSolns_VEML7700::read16_(uint8_t reg, uint16_t& value) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) return VEML_I2CError;
  if (_wire->requestFrom((int)_addr, 2) != 2) return VEML_I2CError;
  uint8_t l = _wire->read();
  uint8_t h = _wire->read();
  value = ((uint16_t)h << 8) | l;
  return VEML_OK;
}
