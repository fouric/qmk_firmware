# Podman for dactylm_fouric (modern QMK)

This branch ports `dactylm_fouric` onto current QMK. The keyboard is a single Teensy 2.0 (`atmega32u4` + halfkay) with an MCP23018 expander for the other half.

## Image

`localhost/qmk-dactylm:latest`

### What's in it

- AVR toolchain (`gcc-avr`, `binutils-avr`, `avr-libc`)
- `teensy_loader_cli` (HalfKay flasher; binary built on Ubuntu 18.04 libusb for reliable Podman USB)
- `dfu-programmer` / `dfu-util`
- Python 3 venv with this tree's `requirements.txt` plus the `qmk` CLI
- Helpers: `flash-teensy`, `usb-test-teensy`

## Build the image

```bash
podman build -t qmk-dactylm -f Containerfile .
```

## Build firmware

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  qmk-dactylm make -j"$(nproc)" dactylm_fouric:default
```

## USB diagnostic (before flashing)

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm usb-test-teensy
```

Or on the host:

```bash
./util/usb_test_teensy.sh 30
```

Start the watcher, then press **RAISE+=** or the physical Teensy reset. HalfKay VID:PID is `16c0:0478`.

## Build and flash

Build in parallel, wait for the HalfKay bootloader without using `teensy_loader_cli -w`, and flash the resulting firmware:

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm bash -lc 'make -j"$(nproc)" dactylm_fouric:default && flash-teensy dactylm_fouric_default.hex'
```

Press **RAISE+=** or the physical Teensy reset button after the helper starts waiting for the bootloader.

## Flash an existing build

**Recommended** (polls for bootloader, then flashes without `teensy_loader_cli -w`):

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm flash-teensy
```

Or, once the bootloader is already visible:

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm teensy_loader_cli --mcu=atmega32u4 -v dactylm_fouric_default.hex
```

QMK's standard build-and-flash target invokes `teensy_loader_cli -w`, which may wait forever under rootless Podman even after resetting the keyboard:

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm make dactylm_fouric:default:flash
```

Use the build-and-flash command above instead.

### udev rule (host)

```bash
echo 'SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="16c0", ATTR{idProduct}=="0478", MODE="0666", GROUP="plugdev"' \
  | sudo tee /etc/udev/rules.d/99-teensy.rules
sudo rm -f /etc/udev/rules.d/50-teensy.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Optional legacy image

`Containerfile.legacy` builds `qmk-dactylm-legacy` on Ubuntu 18.04 (gcc 7.5 / avr-gcc 5.4). Prefer `qmk-dactylm` for normal work; use legacy only if you need that older host toolchain.

```bash
podman build -t qmk-dactylm-legacy -f Containerfile.legacy .
```

## Files

- `Containerfile` — main image (`qmk-dactylm`)
- `Containerfile.legacy` — optional older toolchain
- `util/flash_teensy.sh` / `util/usb_test_teensy.sh` — flash helpers

## Notes

- Mount with `:z` for SELinux (harmless elsewhere).
- Submodules must be present on the host; the container uses the mounted tree.
- The old `dactylM_fouric` branch remains the known-good ~2020 baseline until this port is confirmed on hardware.
