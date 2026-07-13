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

/*
 * Restore 2020-era mod-tap chord feel for MT(MOD_LCTL, KC_ESC) / MT(MOD_RCTL, KC_TAB).
 *
 * Since May 2023, QMK defaults to "ignore interrupt": holding an MT and pressing
 * another key waits for the full TAPPING_TERM (200 ms) before the mod applies.
 * HOLD_ON_OTHER_KEY_PRESS settles the MT as a hold as soon as another key goes
 * down (closest match to the old default).
 *
 * Unlike SPECULATIVE_HOLD (which sends Ctrl/Shift immediately on keydown without
 * changing the tap/hold decision, and can briefly flash a lone mod to the OS),
 * this only changes when the decision is made — key events stay buffered until
 * then. Prefer this over SPECULATIVE_HOLD for keyboard chords.
 */
#define HOLD_ON_OTHER_KEY_PRESS
