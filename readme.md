# Gector's Comments

## Compiling 
Run the command `qmk compile -kb dactylM_fouric -km default` to compile the keyboard normally. This places a hex file in the main directory, which you can upload to the teensy with "teensy loader". Teensy loader is availble from https://www.pjrc.com/teensy/loader.html

The "teensy_loader_cli" executable is included in this repository and typical use looks like:

`sudo ./teensy_loader_cli --mcu=atmega32u4 -w dactylM_fouric_default.hex   -v`


## Other
You should be able to install qmk's commands by running `python3 -m pip install --user qmk`.

Take a look at https://beta.docs.qmk.fm/tutorial/newbs_getting_started for information on qmk, the docs are extensive but can be difficult to navigate. I recomend looking at https://beta.docs.qmk.fm/developing-qmk/for-a-deeper-understanding/understanding_qmk and other parts of the "Developing QMK" section for an understanding of how QMK works.

## Changes Made
I created the keyboard called "dactylM_fouric", as in, "dactyl manuform - fouric", and based most of the code off of the ergodox_ez which is largely based off of modified internal programs of qmk itself. QMK expects to see a few different functions in the keyboard specific files, but you can add additional source files for your own functionality if you wish. In my case, I overrode the default `matrix.c` file located in the `quantum` directory, and made it work with I2C (built in to QMK for the most part) to communicate with an MCP23018.

The code initializes and then monitors both halves of the keyboard. By default, all columns are tied high with an internal pull-up resistor both in the Teensy 2.0 and the MCP23018 I/O expander. Rows are pulled low from Row 0 to Row 'n', and closed switches connect a column to a row. When a column is pulled low, it means that a switch on that column is both closed and that row is being pulled low by the firmware. The firmware keeps track of which row it is on and modify's an internal represenation of the key matrix created by a 2 dimensional array of 1's and 0's. 

## Editing The Keymap
Most edits for changing keys are made in `keymap.c` of `keyboards/dactylM_fouric/keymaps/`. See https://beta.docs.qmk.fm/using-qmk/simple-keycodes/keycodes for a list of availble keycodes. 


# Quantum Mechanical Keyboard Firmware

[![Current Version](https://img.shields.io/github/tag/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/tags)
[![Build Status](https://travis-ci.org/qmk/qmk_firmware.svg?branch=master)](https://travis-ci.org/qmk/qmk_firmware)
[![Discord](https://img.shields.io/discord/440868230475677696.svg)](https://discord.gg/Uq7gcHh)
[![Docs Status](https://img.shields.io/badge/docs-ready-orange.svg)](https://docs.qmk.fm)
[![GitHub contributors](https://img.shields.io/github/contributors/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/pulse/monthly)
[![GitHub forks](https://img.shields.io/github/forks/qmk/qmk_firmware.svg?style=social&label=Fork)](https://github.com/qmk/qmk_firmware/)

This is a keyboard firmware based on the [tmk\_keyboard firmware](https://github.com/tmk/tmk_keyboard) with some useful features for Atmel AVR and ARM controllers, and more specifically, the [OLKB product line](https://olkb.com), the [ErgoDox EZ](https://ergodox-ez.com) keyboard, and the [Clueboard product line](https://clueboard.co).

## Documentation

* [See the official documentation on docs.qmk.fm](https://docs.qmk.fm)

The docs are powered by [Docsify](https://docsify.js.org/) and hosted on [GitHub](/docs/). They are also viewable offline; see [Previewing the Documentation](https://docs.qmk.fm/#/contributing?id=previewing-the-documentation) for more details.

You can request changes by making a fork and opening a [pull request](https://github.com/qmk/qmk_firmware/pulls), or by clicking the "Edit this page" link at the bottom of any page.

## Supported Keyboards

* [Planck](/keyboards/planck/)
* [Preonic](/keyboards/preonic/)
* [ErgoDox EZ](/keyboards/ergodox_ez/)
* [Clueboard](/keyboards/clueboard/)
* [Cluepad](/keyboards/clueboard/17/)
* [Atreus](/keyboards/atreus/)

The project also includes community support for [lots of other keyboards](/keyboards/).

## Maintainers

QMK is developed and maintained by Jack Humbert of OLKB with contributions from the community, and of course, [Hasu](https://github.com/tmk). The OLKB product firmwares are maintained by [Jack Humbert](https://github.com/jackhumbert), the Ergodox EZ by [ZSA Technology Labs](https://github.com/zsa), the Clueboard by [Zach White](https://github.com/skullydazed), and the Atreus by [Phil Hagelberg](https://github.com/technomancy).

## Official Website

[qmk.fm](https://qmk.fm) is the official website of QMK, where you can find links to this page, the documentation, and the keyboards supported by QMK.
