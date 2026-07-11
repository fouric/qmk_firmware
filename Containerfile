# Self-contained Containerfile for this ~2020-era QMK checkout.
# Target: dactylM_fouric (ATmega32U4 + halfkay bootloader / Teensy 2.0 style)
#
# Features:
# - AVR toolchain (avr-gcc 5.x era from Ubuntu 22.04's gcc-avr package, avr-libc, etc.)
# - teensy_loader_cli for flashing via halfkay
# - Python 3 + the requirements for bin/qmk and build helpers
# - Works great with rootless Podman
#
# Build the image:
#   podman build -t qmk-dactylm-2020 -f Containerfile .
#
# Typical usage (rootless Podman):
#   podman run --rm -it -v "$PWD:/qmk_firmware:z" \
#     qmk-dactylm-2020 make dactylM_fouric:default
#
# Flash (keyboard must be in bootloader mode; pass through USB):
#   podman run --rm -it -v "$PWD:/qmk_firmware:z" \
#     --device /dev/bus/usb \
#     qmk-dactylm-2020 make dactylM_fouric:default:flash
#
# Using the qmk helper:
#   podman run --rm -it -v "$PWD:/qmk_firmware:z" \
#     qmk-dactylm-2020 bin/qmk compile -kb dactylM_fouric -km default
#
# Rootless Podman + USB device access tips:
# - Use 99-teensy.rules (see podman.md) so /dev/bus/usb nodes are writable.
#
# The legacy Dockerfile (which pulled qmkfm/base_container) is left untouched.

# teensy_loader_cli built on 22.04 fails to spot HalfKay in rootless Podman (libusb quirk).
# Build the flasher on 18.04 and copy the binary into the 22.04 image.
FROM ubuntu:18.04 AS teensy-cli
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        git \
        libusb-dev \
    && git clone --depth 1 https://github.com/PaulStoffregen/teensy_loader_cli.git /tmp/teensy_loader_cli \
    && make -C /tmp/teensy_loader_cli \
    && cp /tmp/teensy_loader_cli/teensy_loader_cli /teensy_loader_cli \
    && cp /lib/x86_64-linux-gnu/libusb-0.1.so.4 /libusb-0.1.so.4 \
    && rm -rf /var/lib/apt/lists/* /tmp/teensy_loader_cli

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive \
    LC_ALL=C.UTF-8 \
    LANG=C.UTF-8

# Base toolchain and runtime deps for this era of QMK (AVR focus + flashing tools)
RUN apt-get update && apt-get install -y --no-install-recommends \
        avr-libc \
        binutils-avr \
        build-essential \
        ca-certificates \
        dfu-programmer \
        dfu-util \
        gcc-avr \
        git \
        libusb-0.1-4 \
        make \
        python3 \
        python3-pip \
        python3-setuptools \
        python3-venv \
        unzip \
        usbutils \
        wget \
        zip \
    && rm -rf /var/lib/apt/lists/*

COPY util/usb_test_teensy.sh /usr/local/bin/usb-test-teensy
COPY util/flash_teensy.sh /usr/local/bin/flash-teensy
RUN chmod 0755 /usr/local/bin/usb-test-teensy /usr/local/bin/flash-teensy

# Bundle 18.04 teensy_loader_cli + libusb (22.04 libusb breaks -w wait mode in Podman).
RUN mkdir -p /usr/local/lib/teensy
COPY --from=teensy-cli /teensy_loader_cli /usr/local/lib/teensy/teensy_loader_cli
COPY --from=teensy-cli /libusb-0.1.so.4 /usr/local/lib/teensy/libusb-0.1.so.4
RUN printf '%s\n' \
    '#!/bin/sh' \
    'export LD_LIBRARY_PATH=/usr/local/lib/teensy${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}' \
    'exec /usr/local/lib/teensy/teensy_loader_cli "$@"' \
    > /usr/local/bin/teensy_loader_cli \
    && chmod 0755 /usr/local/bin/teensy_loader_cli /usr/local/lib/teensy/teensy_loader_cli

# Create an isolated venv for QMK's Python requirements so we don't fight
# Ubuntu's externally-managed-environment rules, and to keep the image clean.
RUN python3 -m venv /opt/qmk-venv
ENV PATH="/opt/qmk-venv/bin:${PATH}"

# Install the pinned-ish Python deps from this checkout's requirements.txt
COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt

# Workdir and volume expected by QMK conventions.
# Always mount your source checkout at runtime:
#   -v "$PWD:/qmk_firmware:z"
WORKDIR /qmk_firmware
VOLUME ["/qmk_firmware"]

# Helpful default: show usage if someone runs the container without a command
# and without a mount. Override with your make/qmk command as needed.
CMD ["sh", "-c", "echo 'Mount your qmk tree: -v \"$PWD:/qmk_firmware:z\"' && echo 'Example: podman run --rm -it -v \"$PWD:/qmk_firmware:z\" qmk-dactylm-2020 make dactylM_fouric:default' && exec bash"]
