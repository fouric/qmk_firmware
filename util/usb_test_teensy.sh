#!/usr/bin/env bash
# Watch for Teensy halfkay bootloader (16c0:0478) from host or Podman container.
# Usage:
#   ./util/usb_test_teensy.sh [seconds]
#   podman run --rm -v "$PWD:/qmk_firmware:z" --device /dev/bus/usb \
#     qmk-dactylm-2020 usb-test-teensy
#
# Press the Teensy RESET button (or RAISE+=) while this script is running.

set -euo pipefail

TIMEOUT="${1:-30}"
VENDOR="16c0"
PRODUCT="0478"

echo "=== Teensy USB diagnostic ==="
echo "Looking for bootloader ${VENDOR}:${PRODUCT} for up to ${TIMEOUT}s"
echo "Press RESET on the keyboard now."
echo

have_lsusb=0
command -v lsusb >/dev/null 2>&1 && have_lsusb=1

find_sysfs_teensy() {
	local d vendor product devmaj devmin node perms
	for d in /sys/bus/usb/devices/*; do
		[[ -f "$d/idVendor" && -f "$d/idProduct" ]] || continue
		vendor=$(cat "$d/idVendor")
		product=$(cat "$d/idProduct")
		[[ "$vendor" == "$VENDOR" && "$product" == "$PRODUCT" ]] || continue
		echo "SYSFS: $d (idVendor=$vendor idProduct=$product)"
		if [[ -f "$d/dev" ]]; then
			IFS=: read -r devmaj devmin <"$d/dev"
			printf -v devmin '%d' "0x${devmin}"
			for busdir in /dev/bus/usb/*; do
				[[ -d "$busdir" ]] || continue
				node="$busdir/$devmin"
				if [[ -e "$node" ]]; then
					perms=$(stat -c '%a %U:%G' "$node")
					echo "DEVNODE: $node ($perms)"
					if [[ -w "$node" ]]; then
						echo "WRITABLE: yes"
					else
						echo "WRITABLE: NO (flasher will fail — check udev rule)"
					fi
				fi
			done
		fi
		return 0
	done
	return 1
}

try_lsusb() {
	if [[ "$have_lsusb" -eq 0 ]]; then
		return 1
	fi
	if lsusb -d "${VENDOR}:${PRODUCT}" 2>/dev/null; then
		local line bus dev node perms
		line=$(lsusb -d "${VENDOR}:${PRODUCT}")
		bus=$(echo "$line" | awk '{print $2}')
		dev=$(echo "$line" | awk '{print $4}' | tr -d ':')
		node="/dev/bus/usb/${bus}/${dev}"
		if [[ -e "$node" ]]; then
			perms=$(stat -c '%a %U:%G' "$node")
			echo "DEVNODE: $node ($perms)"
			[[ -w "$node" ]] && echo "WRITABLE: yes" || echo "WRITABLE: NO"
		fi
		return 0
	fi
	return 1
}

probe_teensy_loader() {
	if ! command -v teensy_loader_cli >/dev/null 2>&1; then
		echo "teensy_loader_cli: not installed"
		return 0
	fi
	if [[ ! -f dactylM_fouric_default.hex ]]; then
		echo "teensy_loader_cli: skipping flash probe (no dactylM_fouric_default.hex in cwd)"
		return 0
	fi
	echo "teensy_loader_cli: quick probe (no -w, exits immediately if absent)..."
	if teensy_loader_cli --mcu=atmega32u4 -v dactylM_fouric_default.hex 2>&1 | head -5; then
		echo "teensy_loader_cli: saw bootloader"
	else
		echo "teensy_loader_cli: did not see bootloader"
	fi
}

end=$((SECONDS + TIMEOUT))
seen=0
while (( SECONDS < end )); do
	if try_lsusb || find_sysfs_teensy; then
		seen=1
		echo
		probe_teensy_loader
		echo
		echo "SUCCESS: bootloader visible to this environment."
		exit 0
	fi
	sleep 0.25
done

echo
if [[ "$seen" -eq 0 ]]; then
	echo "TIMEOUT: never saw ${VENDOR}:${PRODUCT}."
	echo
	echo "If the keyboard went dead but nothing appeared here:"
	echo "  - RESET may not be entering bootloader (stuck firmware / wrong key combo)"
	echo "  - Try the physical RESET button on the Teensy, not only RAISE+="
	echo "  - Try a different USB port/cable (must be a data port)"
	echo "  - On host run: lsusb | grep 16c0"
fi
exit 1
