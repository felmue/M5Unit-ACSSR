#include "UNIT_ACSSR.h"

bool UNIT_ACSSR::begin(TwoWire *wire, uint8_t sda, uint8_t scl, uint8_t addr) {
    _wire = wire;
    _addr = addr;
    _sda  = sda;
    _scl  = scl;
    _wire->begin(_sda, _scl, 100000UL);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

bool UNIT_ACSSR::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                            uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(buffer, length);
    if (_wire->endTransmission() == 0) return true;
    return false;
}

bool UNIT_ACSSR::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                           uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();
    if (_wire->requestFrom(addr, length)) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[index++] = _wire->read();
        }
        return true;
    }
    return false;
}

bool UNIT_ACSSR::on() {
    uint8_t data = 1;
    return writeBytes(_addr, ACSSR_I2C_RELAY_REG, &data, 1);
}

bool UNIT_ACSSR::off() {
    uint8_t data = 0;
    return writeBytes(_addr, ACSSR_I2C_RELAY_REG, &data, 1);
}

bool UNIT_ACSSR::status() {
    uint8_t data = 0;
    readBytes(_addr, ACSSR_I2C_RELAY_REG, &data, 1);
    return data;
}

bool UNIT_ACSSR::setLEDColor(uint32_t colorHEX) {
    uint8_t color[3];
    // RED
    color[0] = (colorHEX >> 16) & 0xff;
    // GREEN
    color[1] = (colorHEX >> 8) & 0xff;
    // BLUE
    color[2] = colorHEX & 0xff;
    return writeBytes(_addr, ACSSR_I2C_LED_REG, color, 3);
}

uint32_t UNIT_ACSSR::getLEDColor() {
    uint8_t color[3];
    uint32_t colorHEX = 0;
    readBytes(_addr, ACSSR_I2C_LED_REG, color, 3);
    colorHEX = color[0];
    colorHEX = (colorHEX << 8) | color[1];
    colorHEX = (colorHEX << 8) | color[2];
    return colorHEX;
}

bool UNIT_ACSSR::setDeviceAddr(uint8_t addr) {
    uint8_t data = addr;
    if (writeBytes(_addr, ACSSR_I2C_ADDR_REG, &data, 1)) {
        _addr = addr;
        return true;
    } else {
        return false;
    }
}

uint8_t UNIT_ACSSR::getVersion() {
    uint8_t data = 0;
    readBytes(_addr, ACSSR_I2C_VERSION_REG, &data, 1);
    return data;
}
