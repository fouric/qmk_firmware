#!/usr/bin/env bash
# Wait for HalfKay bootloader, then flash (no teensy_loader_cli -w).
# Works around libusb wait-mode issues in the Ubuntu 22.04 image.
#
# Usage:
#   flash-teensy [hex-file] [timeout-seconds]
#   podman run ... qmk-dactylm-2020 flash-teensy

set -euo pipefail

HEX="${1:-dactylM_fouric_default.hex}"
TIMEOUT="${2:-60}"

if [[ ! -f "$HEX" ]]; then
	echo "error: hex not found: $HEX" >&2
	exit 1
fi

echo "Waiting for Teensy bootloader (16c0:0478) for up to ${TIMEOUT}s..."
echo "Press RAISE+= or the physical reset button now."

have_lsusb=0
command -v lsusb >/dev/null 2>&1 && have_lsusb=1

bootloader_visible() {
	if [[ "$have_lsusb" -eq 1 ]] && lsusb -d '16c0:0478' >/dev/null 2>&1; then
		return 0
	fi
	local d vendor product
	for d in /sys/bus/usb/devices/*; do
		[[ -f "$d/idVendor" && -f "$d/idProduct" ]] || continue
		vendor=$(cat "$d/idVendor")
		product=$(cat "$d/idProduct")
		[[ "$vendor" == "16c0" && "$product" == "0478" ]] && return 0
	done
	return 1
}

end=$((SECONDS + TIMEOUT))
while (( SECONDS < end )); do
	if bootloader_visible; then
		echo "Bootloader detected — flashing $HEX"
		exec teensy_loader_cli --mcu=atmega32u4 -v "$HEX"
	fi
	sleep 0.25
done

echo "Timed out waiting for Teensy bootloader." >&2
exit 1
