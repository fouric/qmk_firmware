# Rootless Podman image for modern QMK + dactylm_fouric
# Target: dactylm_fouric (ATmega32U4 + halfkay / Teensy 2.0)
#
# Build:
#   podman build -t qmk-dactylm -f Containerfile .
#
# Firmware:
#   podman run --rm -it -v "$PWD:/qmk_firmware:z" \
#     qmk-dactylm make dactylm_fouric:default
#
# Flash (preferred — avoids teensy_loader_cli -w quirks):
#   podman run --rm -it -v "$PWD:/qmk_firmware:z" \
#     --device /dev/bus/usb \
#     qmk-dactylm flash-teensy
#
# See podman.md for udev rules and USB diagnostics.

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

RUN apt-get update && apt-get install -y --no-install-recommends \
        avr-libc \
        binutils-avr \
        build-essential \
        ca-certificates \
        clang-format \
        dfu-programmer \
        dfu-util \
        gcc-avr \
        git \
        libhidapi-hidraw0 \
        libusb-0.1-4 \
        libusb-1.0-0 \
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

RUN python3 -m venv /opt/qmk-venv
ENV PATH="/opt/qmk-venv/bin:${PATH}"

COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt \
    && pip install --no-cache-dir qmk

WORKDIR /qmk_firmware
VOLUME ["/qmk_firmware"]

CMD ["sh", "-c", "echo 'Mount your qmk tree: -v \"$PWD:/qmk_firmware:z\"' && echo 'Example: podman run --rm -it -v \"$PWD:/qmk_firmware:z\" qmk-dactylm make dactylm_fouric:default' && exec bash"]
