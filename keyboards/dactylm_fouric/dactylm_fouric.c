/* Copyright 2020 Zachary Whitlock
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include "dactylm_fouric.h"
#include "wait.h"
#include "print.h"

bool         i2c_initialized = false;
i2c_status_t mcp23018_status = 0x20;

uint8_t init_mcp23018(void) {
    mcp23018_status = 0x20;

    if (!i2c_initialized) {
        print("init i2c\n");
        i2c_init(); // SDA=D1, SCL=D0 on Teensy 2.0
        i2c_initialized = true;
        wait_ms(1000);
    }
    print("init mcp23018\n");

    // Port A: columns (input), Port B: rows (output)
    // - unused  : input  : 1
    // - input   : input  : 1
    // - driving : output : 0
    uint8_t iodir[] = {0b00111111, 0b00000000};
    mcp23018_status = i2c_write_register(I2C_ADDR, IODIRA, iodir, 2, DACTYL_I2C_TIMEOUT);
    if (mcp23018_status) {
        return mcp23018_status;
    }

    // Pull-ups on column inputs
    uint8_t gppu[] = {0b00111111, 0b00000000};
    mcp23018_status = i2c_write_register(I2C_ADDR, GPPUA, gppu, 2, DACTYL_I2C_TIMEOUT);
    if (mcp23018_status) {
        return mcp23018_status;
    }

    // LED / open-drain row defaults: OLATA low, OLATB rows high
    uint8_t olat[] = {0b00000000, 0b00011111};
    mcp23018_status = i2c_write_register(I2C_ADDR, OLATA, olat, 2, DACTYL_I2C_TIMEOUT);

    return mcp23018_status;
}
