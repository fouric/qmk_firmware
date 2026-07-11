# Podman for dactylM_fouric (2020-era QMK)

This checkout is from ~2020 and targets a custom `dactylM_fouric` keyboard (ATmega32U4 + `BOOTLOADER = halfkay`).

A self-contained `Containerfile` is provided that builds a rootless-Podman-friendly image with the exact toolchain needed to compile and flash this old version of QMK.

## Image

`localhost/qmk-dactylm-2020:latest` (~550 MB)

### What's in it

- AVR toolchain (gcc-avr 5.4 era + binutils-avr + avr-libc — matching the 2020-ish timeframe)
- `teensy_loader_cli` (for your `BOOTLOADER = halfkay` / atmega32u4)
- Common flasher tools (dfu-programmer, dfu-util, etc.)
- Python 3 + venv with the exact packages from `requirements.txt` (so `bin/qmk` works)
- `make`, etc.

## Build the image (anytime)

```bash
podman build -t qmk-dactylm-2020 -f Containerfile .
```

## Build firmware

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  qmk-dactylm-2020 make dactylM_fouric:default
```

## USB diagnostic (before flashing)

If flash hangs on "Waiting for Teensy device...", run this **first**, then press RESET while it watches:

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm-2020 usb-test-teensy
```

Or from the repo without rebuilding the image:

```bash
./util/usb_test_teensy.sh 30
```

It reports whether `16c0:0478` appears and whether `/dev/bus/usb/...` is writable. **Start the watcher, then reset** — same timing as flash.

## Flash (rootless Podman + USB passthrough)

**Recommended for `qmk-dactylm-2020`** (avoids `teensy_loader_cli -w` wait-mode issues):

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm-2020 flash-teensy
```

Press `RAISE`+`=` when it says it's waiting. Same command works on `qmk-dactylm-legacy`.

Or flash manually once the bootloader is already visible (`usb-test-teensy` first):

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm-2020 teensy_loader_cli --mcu=atmega32u4 -v dactylM_fouric_default.hex
```

Build + flash via make (reset **after** you see `Waiting for Teensy device...`):

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  --device /dev/bus/usb \
  qmk-dactylm-2020 make dactylM_fouric:default:flash
```

## Notes for flashing under rootless Podman

- Your user needs access to the device (Teensy halfkay VID:PID is `16c0:0478`).
- Typical fixes on the host:
  - Add yourself to the `plugdev` group (then log out and back in).
  - Drop a udev rule (see example below).
  - Temporarily: `sudo chmod 666 /dev/bus/usb/<bus>/<dev>`.
- Try `--userns=keep-id` if you want your uid visible inside the container.

### Example udev rule for Teensy (halfkay bootloader)

As root on the host. Use **`99-teensy.rules`** (not `50-`) so it runs *after* the system default that forces USB nodes to `664`:

```bash
echo 'SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="16c0", ATTR{idProduct}=="0478", MODE="0666", GROUP="plugdev"' \
  | sudo tee /etc/udev/rules.d/99-teensy.rules
sudo rm -f /etc/udev/rules.d/50-teensy.rules   # remove old rule if present
sudo udevadm control --reload-rules
sudo udevadm trigger
```

After `RAISE`+`=` (or reset), `./util/usb_test_teensy.sh` should show `WRITABLE: yes`.

## Using the qmk helper

```bash
podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  qmk-dactylm-2020 bin/qmk compile -kb dactylM_fouric -km default
```

## Legacy image (unit tests + older compiler)

`localhost/qmk-dactylm-legacy:latest` — a **separate** image defined in `Containerfile.legacy`.

Use this when you need the 2020-era native compiler behavior (especially `make test`). It is based on **Ubuntu 18.04**:

- Native **gcc/g++ 7.5** — all 6 gtest suites pass, including `serial_link_transport` (newer gcc 9+ rejects code in `transport.h`)
- **avr-gcc 5.4.0** — same AVR toolchain generation as above; firmware builds work the same

```bash
podman build -t qmk-dactylm-legacy -f Containerfile.legacy .

podman run --rm -it -v "$PWD:/qmk_firmware:z" \
  qmk-dactylm-legacy make test
```

The main `qmk-dactylm-2020` image (Ubuntu 22.04) is still the better default for day-to-day firmware work on a modern host. Its `teensy_loader_cli` is built on 18.04 (copied in at image build time) so Podman flashing matches the legacy image. Reach for `qmk-dactylm-legacy` when you specifically need tests or compiler parity with when this tree was written.

## Files

- `Containerfile` — main firmware image (`qmk-dactylm-2020`, Ubuntu 22.04).
- `Containerfile.legacy` — legacy toolchain image (`qmk-dactylm-legacy`, Ubuntu 18.04).
- The old `Dockerfile` (which does `FROM qmkfm/base_container`) is left untouched. That upstream image no longer contains the required AVR toolchain for this era of QMK.

## Notes

- The `:z` on the volume mount is for SELinux (harmless on non-SELinux systems).
- You normally do **not** need `--user $(id -u):$(id -g)` with this image because the build writes into the mounted volume.
- Submodules (LUFA, vusb, etc.) must be present on the host; the container uses what you mount.