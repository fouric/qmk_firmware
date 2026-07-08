/* Copyright 2020 Zachary Whitlock
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
/*const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

        KEYMAP(
                KC_A, KC_B, KC_C, KC_D,
                KC_E, KC_F, KC_G, KC_H,
                KC_I, KC_J, KC_K, KC_L,
 		KC_1, KC_2, KC_3, KC_4,
 		KC_5, KC_6, KC_7, KC_8,
 		KC_9, KC_X, KC_Y, KC_Z)
};*/

#define _BASE 0
#define _RAISE 1
#define _MEGAN 2
#define _NORMAL_MOD 3

// Fillers to make layering more clear

#define ____ KC_TRNS

#define SFT_ESC  SFT_T(KC_ESC)
#define CTL_BSPC CTL_T(KC_BSPC)
#define ALT_SPC  ALT_T(KC_SPC)
#define SFT_ENT  SFT_T(KC_ENT)

#define KC_ML KC_MS_LEFT
#define KC_MR KC_MS_RIGHT
#define KC_MU KC_MS_UP
#define KC_MD KC_MS_DOWN
#define KC_MB1 KC_MS_BTN1
#define KC_MB2 KC_MS_BTN1

#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
 /*
 Base (qwerty)
 * +-----------------------------------------+                             +-----------------------------------------+
 * | ESC  |   q  |   w  |   e  |   r  |   t  |                             |   y  |   u  |   i  |   o  |   p  |      |
 * |------+------+------+------+------+------|                             |------+------+------+------+------+------|
 * | TAB  |   a  |   s  |   d  |   f  |   g  |                             |   h  |   j  |   k  |   l  |   ;  |      |
 * |------+------+------+------+------+------|                             |------+------+------+------+------+------|
 * | SHFT |   z  |   x  |   c  |   v  |   b  |                             |   n  |   m  |   ,  |   .  |   /  |      |
 * +------+------+------+------+-------------+                             +-------------+------+------+------+------+
 *               |  [   |   ]  |                                                         |      |      |
 *               +-------------+-------------+                             +-------------+-------------+
 *                             |      |      |                             |      |      |
 *                             |------+------|                             |------+------|
 *                             |      |      |                             |      |      |
 *                             +-------------+                             +-------------+
 *                                           +-------------+ +-------------+
 *                                           |      |      | |      |      |
 *                                           |------+------| |------+------|
 *                                           |      |      | |      |      |
 *                                           +-------------+ +-------------+
 */

[_BASE] = LAYOUT( \
    KC_EQL,  KC_QUOT, KC_COMM,KC_DOT, KC_P,   KC_Y,       KC_F,   KC_G,   KC_C,   KC_R,   KC_L,   KC_SLSH,   \
    KC_TAB,  KC_A,    KC_O,   KC_E,   KC_U,   KC_I,       KC_D,   KC_H,   KC_T,   KC_N,   KC_S,   KC_MINS,   \
    LSFT(KC_1),KC_SCLN,KC_Q,  KC_J,   KC_K,   KC_X,       KC_B,   KC_M,   KC_W,   KC_V,   KC_Z,   KC_BSLASH, \
	                   KC_LBRC,KC_RBRC,                      KC_PLUS,KC_EQL, \
                                    RAISE,  KC_ENT,       KC_SPC, KC_BSPC,                                     \
	                                MT(MOD_LCTL,KC_ESC),KC_LSFT,KC_RSFT,MT(MOD_RCTL,KC_TAB),		\
	                                KC_LGUI, KC_LALT,      KC_LALT,KC_RGUI								\
),                                                    													     \

[_RAISE] = LAYOUT(
    RESET,  KC_LSFT,KC_LCTL,KC_LALT,KC_TAB, TG(_NORMAL_MOD),        	_______,KC_LEFT,KC_DOWN,KC_UP  ,KC_RGHT,KC_MUTE, \
    _______,KC_1   ,KC_2   ,KC_3   ,KC_4   ,KC_5   ,               	KC_6   ,KC_7   ,KC_8   ,KC_9   ,KC_0   ,_______,   				\
    KC_F1  ,KC_F2  ,KC_F3  ,KC_F4  ,KC_F5  ,KC_F6  ,               	KC_F7  ,KC_F8  ,KC_F9  ,KC_F10 ,KC_F11 ,KC_F12 , \
	                KC_PGDN,KC_PGUP,                                                KC_VOLD,KC_VOLU,				                \
	                                        _______,_______,        _______,KC_DEL,										\
	                                        _______,_______,       	_______,_______,						\
	                                        _______,_______,       	KC_ALGR,_______					\
),

[_MEGAN] = LAYOUT(
				  KC_EQL ,KC_Q   ,KC_W   ,KC_E   ,KC_R   ,KC_T   ,            KC_Y   ,KC_U   ,KC_I   ,KC_O   ,KC_P   ,  KC_LBRC,         \
				  KC_MINS,KC_A   ,KC_S   ,KC_D   ,KC_F   ,KC_G   ,            KC_H   ,KC_J   ,KC_K   ,KC_L   ,KC_SCLN,  KC_QUOT, \
				  KC_BSLS,KC_Z   ,KC_X   ,KC_C   ,KC_V   ,KC_B   ,            KC_N   ,KC_M   ,KC_COMM,KC_DOT ,KC_SLSH,  KC_RBRC, \
				                  _______,_______,                                            _______,_______,                                          \
                                  				   TG(_MEGAN), KC_ENT,       KC_SPC, KC_BSPC,			\
												   KC_LCTL,KC_LSFT,      KC_LALT,KC_LGUI,                                    \
				                                   KC_ESC, _______,      KC_TAB,_______				\
				  ),

[_NORMAL_MOD] = LAYOUT(												\
				 KC_EQL,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,       KC_Y,   KC_U,   KC_I,   KC_O,   KC_P,   KC_SLSH,   \
				 KC_GRV,  KC_A,    KC_S,   KC_D,   KC_F,   KC_G,       KC_H,   KC_J,   KC_K,   KC_L,   KC_SCLN,KC_MINS,   \
				 LSFT(KC_1),KC_Z,  KC_X,   KC_C,   KC_V,   KC_B,       KC_M,   KC_N,   KC_COMM,KC_DOT, KC_SLSH,KC_BSLASH, \
				                   KC_LBRC,KC_RBRC,                                    TG(_NORMAL_MOD),KC_EQL, \
								                   RAISE,  _______,    _______,_______,                                     \
								                   _______,_______,    _______,_______,		\
								                   _______,_______,    _______,_______								\
				  )                                                    													     \
};

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}
