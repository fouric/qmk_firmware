# dactylm_fouric

Handwired Dactyl Manuform variant with a single Teensy 2.0 (`atmega32u4`, halfkay) and an MCP23018 I/O expander for the other half (mux/demux style — not a dual-MCU split).

## Build / flash

See [podman.md](../../podman.md) for rootless Podman images and Teensy flash helpers.

```bash
make dactylm_fouric:default
# or
qmk compile -kb dactylm_fouric -km default
```

Enter bootloader with **RAISE + =** (`QK_BOOT`) or the physical Teensy reset button, then flash the `.hex`.

## Notes (from Gector)

- Most keymap edits live in `keymaps/default/keymap.c`.
- Matrix scanning overrides QMK's default matrix: Teensy GPIO for one half, I2C to MCP23018 for the other.
- Columns are pulled high; rows are driven low while scanning. Closed switches pull a column low.
- I2C runs at 100 kHz (`F_SCL` in `config.h`) for reliability over the interconnect cable.
