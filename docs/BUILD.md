# Build and test

## Native shell

```bash
cd src/faliz-shell
make
./faliz-shell --version
```

The graphical shell needs an active GTK/Wayland display. On a developer workstation it can be launched from a Wayland session with `./faliz-shell`.

## Live ISO

The image pipeline targets Debian Bookworm and uses live-build to produce an x86_64 ISO-hybrid image. Run it from a machine with root privileges, network access, and at least 12 GB free space:

```bash
./scripts/build-iso.sh
```

The pipeline contains compatibility fixes for the installed live-build release, including the legacy firmware-index lookup, bootloader template paths, SVG conversion arguments, and Syslinux bootlogo archive. It also installs `syslinux-utils` in the image chroot because the final ISO-hybrid stage runs there.

The verified output is `dist/faliz-os-live-amd64.iso`. The build completed on 2026-11-21 as an ISO-hybrid image of 1,103,101,952 bytes. The SHA-256 digest for that build was `e56063b0851b4a093ef4f19ea151bba21a65dd074c3a2ae31ad79d271188789a`.

## QEMU smoke test

```bash
qemu-system-x86_64 -enable-kvm -m 4096 -cdrom dist/faliz-os-live-amd64.iso -boot d
```

If KVM is unavailable, remove `-enable-kvm`; the boot will be slower. A non-KVM smoke test kept the generated image running in QEMU for 25 seconds without a boot or device error. Hardware compatibility is not considered verified by a QEMU boot.

## Current acceptance checks

- Shell compiles with the system GTK4 and Libadwaita headers.
- `--version` returns a deterministic build identity.
- ISO build script checks required tools and keeps all output in the repository.
- ISO is bootable according to `file` and `xorriso`: it has an El Torito BIOS entry at `/isolinux/isolinux.bin` and an isohybrid MBR.
- No deployment configuration includes environment variables with the `manus` prefix or name.


## BUILD 02 final verification — 2026-11-22

BUILD 02 is produced with:

```bash
cd /home/ubuntu/faliz-os
sudo ./scripts/build-iso.sh
```

The stable output is:

```text
dist/faliz-os-build-02-live-amd64.iso
```

Final size: `1,145,044,992 bytes`

Final SHA-256: `0399074963601e3e09959235b0ccb2baa6582fb617e49bf3963a8dfb9849da85`

Verify the checksum with:

```bash
sha256sum -c dist/faliz-os-build-02-live-amd64.iso.sha256
```

The final ISO was identified by `file` as a bootable ISO 9660/DOS-MBR image. `xorriso` reported an El Torito BIOS boot entry using `/isolinux/isolinux.bin`, boot-info-table support, and an isohybrid-suitable image.

The compressed live filesystem was extracted and verified to contain `/usr/bin/sway`, `/usr/bin/swaymsg`, `/usr/bin/swaynag`, `/etc/sway/config`, `/usr/share/wayland-sessions/faliz.desktop`, the native FALIZ shell and settings binaries, desktop entries, and branding assets.

Native validation completed with:

```text
FALIZ OS shell BUILD 02 0.2.0
FALIZ Settings BUILD 02 0.2.0
FALIZ validation passed
```

A 30-second software-emulated QEMU smoke test kept the final ISO running until the expected timeout (`qemu_exit=124`). KVM was unavailable in the sandbox (`/dev/kvm` was not present), so this is recorded as a TCG/software boot smoke test rather than a KVM or hardware result.

BUILD 02 screenshots are:

```text
screenshots/faliz-build-02-shell.png
screenshots/faliz-build-02-settings.png
```

The BUILD 02 compositor is Debian Bookworm's `sway`, selected with the live-build `--initsystem systemd` option. This is intentional: the default sysvinit live transition removed sway after package installation, while systemd preserves the compositor and matches FALIZ's systemd user-session service.
