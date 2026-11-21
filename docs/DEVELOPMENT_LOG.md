# Development log

## 2026-11-21 — Stage 1/2 foundation

- Selected Ubuntu Noble x86_64 as the initial base to maximize driver and package maturity.
- Installed the initial native and image-build toolchain: Rust/Cargo, GTK4/Libadwaita headers, Wayland headers, debootstrap, live-build, xorriso, and QEMU.
- Created the repository layout and Git history.
- Implemented the first native FALIZ shell surface with branded background, top status bar, clock, dock, and real file/terminal launch actions.
- Added a live-build image pipeline and systemd user service.
- Added original SVG branding and project architecture/build documentation.

## Explicit limitations

- This milestone does not yet replace the compositor or implement a complete window manager.
- A live ISO build and QEMU boot must be run before calling the image bootable; physical hardware compatibility remains unverified.
- The broader application ecosystem, installer, settings integration, security policy UI, and performance benchmarks are subsequent stages.
