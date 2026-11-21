#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
for tool in cc pkg-config; do command -v "$tool" >/dev/null || { echo "missing: $tool"; exit 1; }; done
(cd "$ROOT/src/faliz-shell" && make clean && make && ./faliz-shell --version)
test -f "$ROOT/branding/faliz-logo.svg"
test -f "$ROOT/packaging/systemd/faliz-shell.service"
if grep -RinE '(^|[^A-Za-z])manus(_|-|[A-Z])' "$ROOT" --exclude-dir=.git --exclude='*.log'; then
  echo 'forbidden deployment variable/name detected' >&2
  exit 1
fi
echo 'FALIZ validation passed'
