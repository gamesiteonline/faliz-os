
## 2026-11-21 — ISO pipeline validation

- Root privileges were confirmed necessary for live-build bootstrap.
- Ubuntu Noble was rejected by the installed live-build version because it assumes Debian-style security suites and package names.
- Debian Bookworm bootstrap succeeds after disabling the obsolete firmware-index lookup and separating the kernel image/flavour settings.
- The pipeline now supplies a corrected ISOLINUX template, a Pillow-generated FALIZ bootlogo cpio archive, and an in-chroot `syslinux-utils` dependency for the final ISO-hybrid stage.
- A real `dist/faliz-os-live-amd64.iso` was produced successfully. `file` identifies it as a bootable ISO 9660 image; `xorriso` reports an El Torito BIOS entry and an isohybrid MBR.
- QEMU boot smoke testing ran the ISO for 25 seconds without a boot failure and did not touch a physical disk. This does not establish compatibility with physical hardware.


## 2026-11-22 — BUILD 02 completed

- Continued from BUILD 01 without removing the preserved milestone ISO.
- Added the native GTK4/Libadwaita launcher, dock, workspace controls, toast notifications, theme switch, Wayland status, and application launch entries.
- Added the native FALIZ Settings application with Appearance and System pages.
- Added the Debian Bookworm sway Wayland session, four workspace configuration, window-management shortcuts, mako notifications, swaybg wallpaper startup, and FALIZ session autostart.
- The first package-retention hook was intentionally investigated rather than treated as proof: live-build installed sway, then the default sysvinit live transition removed it.
- Fixed the root cause by selecting `--initsystem systemd` in `scripts/build-iso.sh`, matching the existing FALIZ systemd user-session integration.
- Rebuilt the image under root privileges as required by live-build bootstrap.
- Final ISO: `dist/faliz-os-build-02-live-amd64.iso`, 1,145,044,992 bytes.
- Final SHA-256: `0399074963601e3e09959235b0ccb2baa6582fb617e49bf3963a8dfb9849da85`.
- SquashFS inspection confirmed `/usr/bin/sway`, `/usr/bin/swaymsg`, `/usr/bin/swaynag`, the FALIZ Wayland session entry, native shell/settings binaries, desktop entries, and branding.
- `./scripts/validate.sh` passed and `sha256sum -c` returned `OK`.
- QEMU software-emulation smoke test remained running for 30 seconds and ended with the expected timeout. KVM and physical/VirtualBox testing remain unavailable or unverified in this environment.
- Explicitly left graphical login, lock screen, installer, full native application suite, and hardware acceptance testing incomplete.

## 2026-11-27 — BUILD 04.1 native application suite

- Added a first-party `faliz-app` GTK4/Libadwaita application binary with focused modes for FALIZ Files, Notes, Calculator, Screenshot, System Monitor, Software Store, and Media Player.
- FALIZ Files now browses the home folder, opens selected items with the default application, changes folders, and creates a new folder.
- FALIZ Notes saves and reloads a local note at `Documents/FALIZ Notes/notes.txt`; FALIZ Calculator supports keyboard expressions using `+`, `-`, `*`, and `/`.
- FALIZ Screenshot provides area/window capture and full-screen capture into `Pictures/FALIZ Screenshots`; FALIZ System Monitor reads live `/proc` load and memory data plus home filesystem usage.
- FALIZ Settings now documents the native suite and reports BUILD 04.1. Store and Media Player provide branded, controlled hand-offs to GNOME Software/PolicyKit and VLC rather than silently reimplementing privileged package or codec operations.
- Added desktop entries for all seven native-app modes, updated the launcher, and added build/validation coverage. Compilation and `scripts/validate.sh` passed; a new ISO has not yet been rebuilt in this milestone.
