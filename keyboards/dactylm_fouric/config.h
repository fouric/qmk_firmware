/* Copyright 2020 Zachary Whitlock
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

/* key matrix size — custom matrix (Teensy half + MCP23018 half) */
#define MATRIX_ROWS 10
#define MATRIX_ROWS_PER_SIDE (MATRIX_ROWS / 2)
#define MATRIX_COLS 6

#define DIODE_DIRECTION COL2ROW

/* MCP23018 over cable is more reliable at 100 kHz than the AVR default 400 kHz */
#define F_SCL 100000UL
