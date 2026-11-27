#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
VERSION=${1:-}
OUTPUT_DIR=${2:-"$ROOT/dist/updates"}
BUNDLE_NAME="faliz-os-update-${VERSION}.tar.gz"
STAGE=$(mktemp -d)
trap 'rm -rf "$STAGE"' EXIT

if [[ ! "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  echo "Usage: $0 VERSION [OUTPUT_DIR]" >&2
  echo "Example: $0 0.4.2 dist/updates" >&2
  exit 2
fi
mkdir -p "$OUTPUT_DIR"
(cd "$ROOT/src/faliz-shell" && make clean && make)
mkdir -p "$STAGE/usr/local/bin" "$STAGE/usr/share/faliz" "$STAGE/usr/share/applications" "$STAGE/etc/faliz"
install -m 0755 "$ROOT/src/faliz-shell/faliz-shell" "$STAGE/usr/local/bin/faliz-shell"
install -m 0755 "$ROOT/src/faliz-shell/faliz-settings" "$STAGE/usr/local/bin/faliz-settings"
install -m 0755 "$ROOT/src/faliz-shell/faliz-app" "$STAGE/usr/local/bin/faliz-app"
install -m 0755 "$ROOT/packaging/live/faliz-update" "$STAGE/usr/local/bin/faliz-update"
install -m 0644 "$ROOT/src/faliz-shell/faliz.css" "$STAGE/usr/share/faliz/faliz.css"
for desktop in "$ROOT"/packaging/live/com.faliz.OS.*.desktop; do
  install -m 0644 "$desktop" "$STAGE/usr/share/applications/$(basename "$desktop")"
done
printf '%s\n' "$VERSION" > "$STAGE/etc/faliz/build-id"
BUNDLE="$OUTPUT_DIR/$BUNDLE_NAME"
tar -czf "$BUNDLE" -C "$STAGE" usr etc
sha256sum "$BUNDLE" | tee "$BUNDLE.sha256"
cat > "$OUTPUT_DIR/manifest-${VERSION}.json" <<EOF
{
  "version": "$VERSION",
  "bundle_url": "REPLACE_WITH_GITHUB_RELEASE_ASSET_URL",
  "sha256": "$(sha256sum "$BUNDLE" | awk '{print $1}')",
  "notes": "FALIZ component update $VERSION"
}
EOF
printf 'Created %s\n' "$BUNDLE"
printf 'Upload the bundle to a GitHub Release, replace bundle_url in manifest-%s.json, then publish it as updates/stable/manifest.json.\n' "$VERSION"
