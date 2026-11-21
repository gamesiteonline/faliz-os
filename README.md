# FALIZ OS

FALIZ OS is a Linux-based desktop operating system project by Fahad Mohamed. The project is designed around a stable Debian foundation, Wayland, systemd, PipeWire, NetworkManager, Polkit, and Flatpak, with a native FALIZ desktop shell layered on top.

## Current milestone: Stage 2 / Stage 3 foundation

This repository currently contains:

- A native GTK4/Libadwaita FALIZ shell surface with a branded desktop, launcher, dock, clock, and real application launching.
- A reproducible live-build configuration for a Debian-based live ISO.
- Systemd user-session integration for starting the shell.
- Official FALIZ app-icon branding, desktop metadata, and architecture/build documentation.
- Validation scripts for compiling the shell and checking the image toolchain.

This is an active engineering foundation, not a claim that every item in the master specification is complete. The custom shell compositor, installer UX, hardware matrix, and full native application suite remain subsequent stages.

## Quick start

```bash
sudo apt-get install build-essential pkg-config libgtk-4-dev libadwaita-1-dev live-build debootstrap xorriso qemu-system-x86
cd src/faliz-shell
make
./faliz-shell
```

To build the live image after the host has the required packages:

```bash
./scripts/build-iso.sh
```

The current live image target is Debian Bookworm on x86_64, selected because the installed live-build toolchain has first-class Debian support. The shell remains distribution-agnostic. The image builder never writes to a physical disk. It produces an ISO under `dist/`; installation to a disk is intentionally left to a future installer workflow with explicit user confirmation.

## Architecture

- `src/faliz-shell`: native GTK4 shell surface and interaction foundation.
- `packaging/live`: live-build package list, branding, and system defaults.
- `packaging/systemd`: user service for starting the shell session.
- `branding`: logo and visual identity assets.
- `docs`: architecture, build, testing, and development log.
- `scripts`: reproducible build and validation helpers.

## License

The project source is released under the MIT License. Linux, GTK, Libadwaita, systemd, and other upstream components retain their own licenses.
