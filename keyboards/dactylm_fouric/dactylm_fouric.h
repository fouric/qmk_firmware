/* Copyright 2020 Zachary Whitlock
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "quantum.h"
#include "i2c_master.h"

/* MCP23018 I2C address (7-bit addr left-shifted per QMK I2C convention) */
#define I2C_ADDR (0b0100000 << 1)
#define IODIRA   0x00
#define IODIRB   0x01
#define GPPUA    0x0C
#define GPPUB    0x0D
#define GPIOA    0x12
#define GPIOB    0x13
#define OLATA    0x14
#define OLATB    0x15

#define DACTYL_I2C_TIMEOUT 100

extern i2c_status_t mcp23018_status;
uint8_t             init_mcp23018(void);
