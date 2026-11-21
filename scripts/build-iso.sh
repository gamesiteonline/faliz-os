#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD="$ROOT/.build/live"
DIST="$ROOT/dist"
mkdir -p "$DIST"
for tool in lb debootstrap xorriso; do command -v "$tool" >/dev/null || { echo "missing required tool: $tool" >&2; exit 1; }; done
(cd "$ROOT/src/faliz-shell" && make)
rm -rf "$BUILD"
mkdir -p "$BUILD/config/package-lists" "$BUILD/config/includes.chroot/usr/local/bin" "$BUILD/config/includes.chroot/usr/share/applications" "$BUILD/config/includes.chroot/usr/share/icons/hicolor/2048x2048/apps" "$BUILD/config/includes.chroot/etc/systemd/user" "$BUILD/config/includes.chroot/usr/share/faliz"
cp "$ROOT/src/faliz-shell/faliz-shell" "$BUILD/config/includes.chroot/usr/local/bin/faliz-shell"
cp "$ROOT/src/faliz-shell/faliz.css" "$BUILD/config/includes.chroot/usr/share/faliz/faliz.css"
cp "$ROOT/packaging/systemd/faliz-shell.service" "$BUILD/config/includes.chroot/etc/systemd/user/faliz-shell.service"
cp "$ROOT/packaging/live/faliz.list.chroot" "$BUILD/config/package-lists/faliz.list.chroot"
cp "$ROOT/packaging/live/com.faliz.OS.Shell.desktop" "$BUILD/config/includes.chroot/usr/share/applications/com.faliz.OS.Shell.desktop"
cp "$ROOT/branding/faliz-logo.svg" "$BUILD/config/includes.chroot/usr/share/faliz/faliz-logo.svg"
cp "$ROOT/branding/faliz-app-icon.png" "$BUILD/config/includes.chroot/usr/share/icons/hicolor/2048x2048/apps/com.faliz.OS.Shell.png"
(cd "$BUILD" && lb config --distribution bookworm --architectures amd64 --binary-images iso-hybrid --archive-areas "main contrib non-free non-free-firmware" --mirror-bootstrap "http://deb.debian.org/debian/" --mirror-chroot "http://deb.debian.org/debian/" --mirror-binary "http://deb.debian.org/debian/" --security false --linux-packages "linux-image-amd64" --apt-indices none)
(cd "$BUILD" && lb build)
cp "$BUILD/live-image-amd64.hybrid.iso" "$DIST/faliz-os-live-amd64.iso"
echo "Built $DIST/faliz-os-live-amd64.iso"
