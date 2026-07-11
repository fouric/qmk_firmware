// Collected/written/tweaked by Zachary Whitlock (2020)
// Ported to modern QMK custom matrix (lite) API.
//
// Based strongly on ergodox_ez code and existing code for dactyl_manuform.
// Teensy half: rows 0-4. MCP23018 half: rows 5-9.

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include "print.h"
#include "matrix.h"
#include "gpio.h"
#include "dactylm_fouric.h"

static matrix_row_t read_cols(uint8_t row);
static void         init_cols(void);
static void         unselect_rows(void);
static void         select_row(uint8_t row);

static uint8_t mcp23018_reset_loop;

void matrix_init_custom(void) {
    mcp23018_status = init_mcp23018();
    unselect_rows();
    init_cols();
}

void matrix_power_up(void) {
    mcp23018_status = init_mcp23018();
    unselect_rows();
    init_cols();
}

static inline bool store_raw_matrix_row(matrix_row_t current_matrix[], uint8_t index) {
    matrix_row_t temp = read_cols(index);
    if (current_matrix[index] != temp) {
        current_matrix[index] = temp;
        return true;
    }
    return false;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    if (mcp23018_status) {
        if (++mcp23018_reset_loop == 0) {
            print("trying to reset mcp23018\n");
            mcp23018_status = init_mcp23018();
            if (mcp23018_status) {
                print("left side not responding\n");
            } else {
                print("left side attached\n");
            }
        }
    }

    bool changed = false;
    for (uint8_t i = 0; i < MATRIX_ROWS_PER_SIDE; i++) {
        uint8_t right_index = i;
        uint8_t left_index  = i + MATRIX_ROWS_PER_SIDE;
        select_row(left_index);
        select_row(right_index);

        changed |= store_raw_matrix_row(current_matrix, left_index);
        changed |= store_raw_matrix_row(current_matrix, right_index);

        unselect_rows();
    }

    return changed;
}

/* Column pin configuration
 *
 * Teensy
 * col: 0   1   2   3   4   5
 * pin: B1  B2  B3  B7  D3  D2
 *
 * MCP23018
 * col: 0   1   2   3   4   5
 * pin: A* (bit-remapped in read_cols)
 */
static void init_cols(void) {
    gpio_set_pin_input_high(B1);
    gpio_set_pin_input_high(B2);
    gpio_set_pin_input_high(B3);
    gpio_set_pin_input_high(B7);
    gpio_set_pin_input_high(D2);
    gpio_set_pin_input_high(D3);

    // Debug LED on B0
    gpio_set_pin_output(B0);
    gpio_write_pin_high(B0);
}

static matrix_row_t read_cols(uint8_t row) {
    if (row > 4) {
        if (mcp23018_status) {
            return 0;
        }

        uint8_t data = 0;
        mcp23018_status = i2c_read_register(I2C_ADDR, GPIOA, &data, 1, DACTYL_I2C_TIMEOUT);
        if (mcp23018_status) {
            return 0;
        }

        // Remap MCP23018 port A bits to logical columns (preserved from original firmware)
        return (matrix_row_t)(
            (((~data) & 0x08) << 2) | // 1, 0b0000_1000 -> 0b0010_0000
            (((~data) & 0x10)) |      // 2, 0b0001_0000 -> 0b0001_0000
            (((~data) & 0x04) << 1) | // 3, 0b0000_0100 -> 0b0000_1000
            (((~data) & 0x20) >> 3) | // 4, 0b0010_0000 -> 0b0000_0100
            (((~data) & 0x01) << 1) | // 5, 0b0000_0001 -> 0b0000_0010
            (((~data) & 0x02) >> 1)   // 6, 0b0000_0010 -> 0b0000_0001
        );
    }

    /* Teensy
     * col: 0   1   2   3   4   5
     * pin: B1  B2  B3  B7  D3  D2
     */
    return ~((matrix_row_t)(
        ((PINB >> 1) & 0x07) | // 1-3, 0b0000_1110 -> 0b0000_0111
        ((PINB >> 4) & 0x08) | // 4,   0b1000_0000 -> 0b0000_1000
        ((PIND << 1) & 0x10) | // 5,   0b0000_1000 -> 0b0001_0000
        ((PIND << 3) & 0x20)   // 6,   0b0000_0100 -> 0b0010_0000
    ));
}

/* Row pin configuration
 *
 * Teensy
 * row: 0   1   2   3   4
 * pin: F0  F1  F4  F5  F6
 *
 * MCP23018
 * row: 5..9 on port B (bit-remapped in select_row)
 */
static void unselect_rows(void) {
    gpio_set_pin_input(F0);
    gpio_set_pin_input(F1);
    gpio_set_pin_input(F4);
    gpio_set_pin_input(F5);
    gpio_set_pin_input(F6);
}

static void select_row(uint8_t row) {
    if (row > 4) {
        if (mcp23018_status) {
            return;
        }

        uint8_t data = 0xFF;
        switch (row) {
            case 5:
                data = 0xFF & ~(0x04);
                break;
            case 6:
                data = 0xFF & ~(0x02);
                break;
            case 7:
                data = 0xFF & ~(0x01);
                break;
            case 8:
                data = 0xFF & ~(0x08);
                break;
            case 9:
                data = 0xFF & ~(0x10);
                break;
        }
        mcp23018_status = i2c_write_register(I2C_ADDR, GPIOB, &data, 1, DACTYL_I2C_TIMEOUT);
    } else {
        switch (row) {
            case 0:
                gpio_set_pin_output(F0);
                gpio_write_pin_low(F0);
                break;
            case 1:
                gpio_set_pin_output(F1);
                gpio_write_pin_low(F1);
                break;
            case 2:
                gpio_set_pin_output(F4);
                gpio_write_pin_low(F4);
                break;
            case 3:
                gpio_set_pin_output(F5);
                gpio_write_pin_low(F5);
                break;
            case 4:
                gpio_set_pin_output(F6);
                gpio_write_pin_low(F6);
                break;
        }
    }
}
