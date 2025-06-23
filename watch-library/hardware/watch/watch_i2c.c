/*
 * MIT License
 *
 * Copyright (c) 2020 Joey Castillo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "watch_i2c.h"
#include "i2c.h"

#ifdef I2C_SERCOM

void watch_enable_i2c(void) {
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
    i2c_init();
    i2c_enable();
}

void watch_disable_i2c(void) {
    i2c_disable();
}

void watch_i2c_send(int16_t addr, uint8_t *buf, uint16_t length) {
    i2c_write(addr, buf, length);
}

void watch_i2c_receive(int16_t addr, uint8_t *buf, uint16_t length) {
    i2c_read(addr, buf, length);
}

void watch_i2c_write8(int16_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    watch_i2c_send(addr, (uint8_t *)&buf, 2);
}

uint8_t watch_i2c_read8(int16_t addr, uint8_t reg) {
    uint8_t data;

    watch_i2c_send(addr, (uint8_t *)&reg, 1);
    watch_i2c_receive(addr, (uint8_t *)&data, 1);

    return data;
}

uint16_t watch_i2c_read16(int16_t addr, uint8_t reg) {
    uint16_t data;

    watch_i2c_send(addr, (uint8_t *)&reg, 1);
    watch_i2c_receive(addr, (uint8_t *)&data, 2);

    return data;
}

uint32_t watch_i2c_read24(int16_t addr, uint8_t reg) {
    uint32_t data;
    data = 0;

    watch_i2c_send(addr, (uint8_t *)&reg, 1);
    watch_i2c_receive(addr, (uint8_t *)&data, 3);

    return data << 8;
}

uint32_t watch_i2c_read32(int16_t addr, uint8_t reg) {
    uint32_t data;

    watch_i2c_send(addr, (uint8_t *)&reg, 1);
    watch_i2c_receive(addr, (uint8_t *)&data, 4);

    return data;
}

#endif // I2C_SERCOM
