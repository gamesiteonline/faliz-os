#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD="$ROOT/.build/live"
DIST="$ROOT/dist"
BUILD_ID="${FALIZ_BUILD_ID:-04}"
ISO_NAME="faliz-os-build-${BUILD_ID}-live-amd64.iso"
mkdir -p "$DIST"
for tool in lb debootstrap xorriso cpio isohybrid; do command -v "$tool" >/dev/null || { echo "missing required tool: $tool" >&2; exit 1; }; done
(cd "$ROOT/src/faliz-shell" && make)
rm -rf "$BUILD"
mkdir -p "$BUILD/config/package-lists" "$BUILD/config/hooks" "$BUILD/config/includes.chroot/usr/bin" "$BUILD/config/includes.chroot/usr/local/bin" "$BUILD/config/includes.chroot/usr/share/applications" "$BUILD/config/includes.chroot/usr/share/icons/hicolor/2048x2048/apps" "$BUILD/config/includes.chroot/etc/systemd/user" "$BUILD/config/includes.chroot/etc/profile.d" "$BUILD/config/includes.chroot/etc/sway" "$BUILD/config/includes.chroot/etc/waybar" "$BUILD/config/includes.chroot/usr/share/wayland-sessions" "$BUILD/config/includes.chroot/usr/share/faliz"
cp "$ROOT/src/faliz-shell/faliz-shell" "$BUILD/config/includes.chroot/usr/local/bin/faliz-shell"
cp "$ROOT/src/faliz-shell/faliz-settings" "$BUILD/config/includes.chroot/usr/local/bin/faliz-settings"
cp "$ROOT/src/faliz-shell/faliz-installer" "$BUILD/config/includes.chroot/usr/local/bin/faliz-installer-ui"
cp "$ROOT/packaging/live/faliz-installer" "$BUILD/config/includes.chroot/usr/local/bin/faliz-installer"
cp "$ROOT/packaging/live/faliz-update" "$BUILD/config/includes.chroot/usr/local/bin/faliz-update"
cp "$ROOT/packaging/live/faliz-lock" "$BUILD/config/includes.chroot/usr/local/bin/faliz-lock"
cp "$ROOT/src/faliz-shell/faliz.css" "$BUILD/config/includes.chroot/usr/share/faliz/faliz.css"
cp "$ROOT/packaging/systemd/faliz-shell.service" "$BUILD/config/includes.chroot/etc/systemd/user/faliz-shell.service"
cp "$ROOT/packaging/systemd/faliz-session-environment.sh" "$BUILD/config/includes.chroot/etc/profile.d/faliz-session-environment.sh"
cp "$ROOT/packaging/live/faliz.list.chroot" "$BUILD/config/package-lists/faliz.list.chroot"
cp "$ROOT/packaging/live/9999-faliz-session-packages.chroot" "$BUILD/config/hooks/9999-faliz-session-packages.chroot"
chmod 0755 "$BUILD/config/hooks/9999-faliz-session-packages.chroot"
cp "$ROOT/packaging/live/com.faliz.OS.Shell.desktop" "$BUILD/config/includes.chroot/usr/share/applications/com.faliz.OS.Shell.desktop"
cp "$ROOT/packaging/live/com.faliz.OS.Settings.desktop" "$BUILD/config/includes.chroot/usr/share/applications/com.faliz.OS.Settings.desktop"
cp "$ROOT/packaging/live/com.faliz.OS.Installer.desktop" "$BUILD/config/includes.chroot/usr/share/applications/com.faliz.OS.Installer.desktop"
cp "$ROOT/packaging/wayland/faliz.desktop" "$BUILD/config/includes.chroot/usr/share/wayland-sessions/faliz.desktop"
cp "$ROOT/packaging/wayland/sway.conf" "$BUILD/config/includes.chroot/etc/sway/config"
cp "$ROOT/packaging/wayland/waybar.jsonc" "$BUILD/config/includes.chroot/etc/waybar/config"
cp "$ROOT/packaging/wayland/waybar.css" "$BUILD/config/includes.chroot/etc/waybar/style.css"
cp "$ROOT/packaging/wayland/faliz-session-autostart" "$BUILD/config/includes.chroot/usr/local/bin/faliz-session-autostart"
chmod 0755 "$BUILD/config/includes.chroot/etc/profile.d/faliz-session-environment.sh" "$BUILD/config/includes.chroot/usr/local/bin/faliz-session-autostart" "$BUILD/config/includes.chroot/usr/local/bin/faliz-installer-ui" "$BUILD/config/includes.chroot/usr/local/bin/faliz-installer" "$BUILD/config/includes.chroot/usr/local/bin/faliz-update" "$BUILD/config/includes.chroot/usr/local/bin/faliz-lock"
cp "$ROOT/branding/faliz-logo.svg" "$BUILD/config/includes.chroot/usr/share/faliz/faliz-logo.svg"
cp "$ROOT/branding/faliz-app-icon.png" "$BUILD/config/includes.chroot/usr/share/icons/hicolor/2048x2048/apps/com.faliz.OS.Shell.png"
# Use systemd for the live session: the native FALIZ shell already provides a
# systemd user service, and Debian's sysvinit live pass removes sway during
# its package transition.
(cd "$BUILD" && lb config --distribution bookworm --architectures amd64 --binary-images iso-hybrid --archive-areas "main contrib non-free non-free-firmware" --mirror-bootstrap "http://deb.debian.org/debian/" --mirror-chroot "http://deb.debian.org/debian/" --mirror-binary "http://deb.debian.org/debian/" --security false --firmware-chroot false --linux-packages "linux-image" --linux-flavours "amd64" --initsystem systemd --apt-indices none)
mkdir -p "$BUILD/config/bootloaders/isolinux"
cp -a /usr/share/live/build/bootloaders/isolinux/. "$BUILD/config/bootloaders/isolinux/"
rm -f "$BUILD/config/bootloaders/isolinux/isolinux.bin" "$BUILD/config/bootloaders/isolinux/vesamenu.c32"
cp /usr/lib/ISOLINUX/isolinux.bin "$BUILD/config/bootloaders/isolinux/isolinux.bin"
cp /usr/lib/syslinux/modules/bios/vesamenu.c32 "$BUILD/config/bootloaders/isolinux/vesamenu.c32"
python3 - "$ROOT/branding/faliz-app-icon.png" "$BUILD/config/bootloaders/isolinux/splash.png" <<'PY'
from pathlib import Path
import sys
from PIL import Image

source, destination = map(Path, sys.argv[1:])
image = Image.open(source).convert("RGB")
scale = max(640 / image.width, 480 / image.height)
image = image.resize((round(image.width * scale), round(image.height * scale)), Image.Resampling.LANCZOS)
left = (image.width - 640) // 2
top = (image.height - 480) // 2
image.crop((left, top, left + 640, top + 480)).save(destination, format="PNG")
PY
(cd "$BUILD/config/bootloaders/isolinux" && printf '%s\n' splash.png | cpio --quiet -o -H newc > bootlogo)
cat > "$BUILD/config/includes.chroot/usr/bin/rsvg" <<'EOF'
#!/bin/sh
if [ "${1:-}" = "--format" ]; then
  shift 2
  height=480
  width=640
  while [ "$#" -gt 2 ]; do
    case "$1" in
      --height) height="$2"; shift 2 ;;
      --width) width="$2"; shift 2 ;;
      *) shift ;;
    esac
  done
  input="$1"
  output="$2"
  exec /usr/bin/rsvg-convert --format png --width "$width" --height "$height" --output "$output" "$input"
fi
exec /usr/bin/rsvg-convert "$@"
EOF
chmod 0755 "$BUILD/config/includes.chroot/usr/bin/rsvg"
(cd "$BUILD" && lb build)
cp "$BUILD/binary.hybrid.iso" "$DIST/$ISO_NAME"
echo "Built $DIST/$ISO_NAME"
