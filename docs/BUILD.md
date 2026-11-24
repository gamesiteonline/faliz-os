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


## BUILD 03 professional OS foundation — in progress

BUILD 03 extends the BUILD 02 Wayland session with the first professional operating-system functions:

- Waybar top panel with workspaces, clock, network, PipeWire volume, battery, system tray, lock, and power controls.
- Real `swaylock` integration through `/usr/local/bin/faliz-lock` and the `Mod+Escape` shortcut.
- Network connection editor shortcut through NetworkManager.
- PipeWire/PulseAudio control through `pavucontrol`.
- UDisks2 support for future disk and removable-media management.
- `faliz-installer` safety foundation with disk listing, dry-run mode, exact-target confirmation checks, and refusal of unimplemented destructive installation.
- `faliz-update` foundation with read-only update checking and explicit confirmation for package changes.
- FALIZ Settings visibility for lock screen, update manager, installer safety, and power controls.

The native applications now report BUILD 03 version `0.3.0`, and repository validation passes. The installer has not been claimed as complete: no partition, filesystem, bootloader, or disk operation is implemented in this milestone.


## BUILD 03 core application suite — in progress

The second requested phase has started after the professional foundation. The FALIZ launcher is now scrollable and exposes maintained Debian applications for Calculator, Calendar, Notes, System Monitor, Screenshot, Software Store, Media Player, Archive Manager, Document Viewer, Image Viewer, Browser, Files, Terminal, and Settings. BUILD 03 also adds Firefox ESR, LibreOffice Writer, Thunderbird, disk utility, and the supporting desktop applications to the live package manifest.

These applications are integrated launch targets rather than being falsely described as separately written FALIZ-native replacements. The next application milestone is to add FALIZ-specific wrappers, consistent settings, file associations, and first-party utility surfaces where the system applications do not provide the desired experience.


## BUILD 03 final artifact verification — 2026-11-23

The final BUILD 03 ISO is:

```text
/home/ubuntu/faliz-os/dist/faliz-os-build-03-live-amd64.iso
```

It is `1,564,475,392` bytes with SHA-256:

```text
655a34444c26e98732988cb89a98f1871a068809d6d92237288e0cfd58c53ecc
```

`file` and `xorriso` confirm a bootable ISO 9660 image with an El Torito BIOS entry and isohybrid MBR. SquashFS inspection confirmed sway, swaylock, Waybar, FALIZ lock/installer/update commands, and the core application payload. Firefox ESR is available through `/usr/bin/firefox-esr` and the disk utility through `/usr/bin/gnome-disks`.

Native validation passed with BUILD 03 version output, checksum verification returned `OK`, and a software-emulated QEMU boot remained running for 30 seconds with the expected timeout (`qemu_exit=124`). KVM was unavailable in the sandbox.


## BUILD 03 professional polish and developer credits — 2026-11-23

The FALIZ desktop received a visual polish pass with stronger top-bar hierarchy, deeper desktop/content panels, clearer focus-visible states, improved workspace buttons, refined dock shadows, better launcher scrolling, and professional keyboard accessibility styling.

Settings now includes an About FALIZ page with clickable developer-credit rows for Threads, Reddit, Instagram, the WhatsApp channel, YouTube, and Google Play Games. The URLs are embedded in the final `/usr/local/bin/faliz-settings` binary and were verified from the final SquashFS.

The polished BUILD 03 ISO checksum is:

```text
e90dd1947cd2e6dd5948de17906e8849052892ba79fd4710cf615e6db9d59ead
```

The final image remains bootable, checksum-valid, and passed the 30-second software-emulated QEMU smoke test. Native validation passed with BUILD 03 version output.
