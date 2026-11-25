#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
for tool in cc pkg-config; do command -v "$tool" >/dev/null || { echo "missing: $tool"; exit 1; }; done
(cd "$ROOT/src/faliz-shell" && make clean && make && ./faliz-shell --version && ./faliz-settings --version && ./faliz-installer --version)
test -f "$ROOT/branding/faliz-logo.svg"
test -f "$ROOT/packaging/systemd/faliz-shell.service"
test -f "$ROOT/packaging/wayland/faliz.desktop"
test -f "$ROOT/packaging/wayland/sway.conf"
test -f "$ROOT/packaging/live/com.faliz.OS.Settings.desktop"
test -f "$ROOT/packaging/live/com.faliz.OS.Installer.desktop"
test -x "$ROOT/packaging/live/faliz-installer"
"$ROOT/packaging/live/faliz-installer" --dry-run --target /dev/faliz-test-target | grep -q 'No partitions, filesystems, bootloaders, or disks will be changed.'
if "$ROOT/packaging/live/faliz-installer" --install --target /dev/faliz-test-target --confirm /dev/faliz-test-target; then
  echo 'guarded installer unexpectedly performed an install' >&2
  exit 1
fi
if git -C "$ROOT" grep -inE '(^|[^A-Za-z])manus(_|-|[A-Z])' -- ':!*.log' ':!.build/**' ':!dist/**'; then
  echo 'forbidden deployment variable/name detected' >&2
  exit 1
fi
echo 'FALIZ validation passed'
