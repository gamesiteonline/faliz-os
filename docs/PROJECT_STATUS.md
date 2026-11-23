# FALIZ OS Project Status

**Project:** FALIZ OS  
**Target architecture:** x86_64  
**Base system:** Debian Bookworm  
**Current milestone:** Bootable Debian-based live ISO with the native FALIZ shell  
**Status date:** 2026-11-22

## 1. Project goal

FALIZ OS is being developed as a real Linux-based operating system rather than a static HTML mockup or simulated desktop. The project targets x86_64 computers and laptops and is built on a stable Debian foundation.

The long-term goal is a complete operating-system experience with:

- A native graphical FALIZ desktop shell.
- Wayland-oriented desktop behavior.
- systemd-based session integration.
- GPU-accelerated GTK rendering where supported by the host hardware.
- PipeWire audio and WirePlumber session management.
- NetworkManager networking.
- Polkit authorization support.
- Flatpak support where appropriate.
- A premium futuristic visual identity with dark graphite surfaces, purple, electric blue, pink accents, glass-like panels, animations, and dark/light theme support.
- A safe installation experience that never overwrites an existing disk without explicit confirmation.

The project is being completed incrementally. The current result is a genuine bootable live system, not yet a complete installed operating system with a dedicated installer.

## 2. Repository

The project repository is located at:

```text
/home/ubuntu/faliz-os
```

The repository contains the native shell, branding, packaging, live-build configuration, documentation, and validation scripts.

A source archive is also maintained at:

```text
/home/ubuntu/faliz-os-source.tar.gz
```

Generated build output is kept outside Git tracking in:

```text
/home/ubuntu/faliz-os/.build/
/home/ubuntu/faliz-os/dist/
```

A `.gitignore` file now excludes these generated directories and the locally compiled shell binary.

## 3. Native FALIZ shell

The primary desktop shell is implemented in C using GTK4 and Libadwaita. It is compiled as a native Linux executable rather than rendered through a browser or web wrapper.

### Implemented shell features

- Branded FALIZ desktop surface.
- Dark futuristic visual design.
- Top status bar.
- Live clock.
- Dock-style application launcher area.
- Real launch actions for:
  - Files through Nautilus.
  - Terminal through KGX.
  - Settings through the available desktop settings command.
- GTK4 and Libadwaita integration.
- Native CSS-based theme styling.
- Deterministic `--version` output.
- Warning-clean project-owned C code under the available compiler configuration.

The main implementation files are:

```text
src/faliz-shell/main.c
src/faliz-shell/faliz.css
src/faliz-shell/Makefile
```

The shell is compiled with system GTK4 and Libadwaita development headers using `pkg-config`.

### Shell installation path in the live image

The live image installs the compiled shell at:

```text
/usr/local/bin/faliz-shell
```

The visual stylesheet is installed at:

```text
/usr/share/faliz/faliz.css
```

## 4. Session integration

Initial systemd user-session integration has been added.

The service file is:

```text
packaging/systemd/faliz-shell.service
```

It is installed into the image at:

```text
/etc/systemd/user/faliz-shell.service
```

This establishes the first real session-level integration point for starting the FALIZ shell in a user graphical session.

A dedicated display manager, login screen, lock screen, compositor, and full session policy are future milestones and are not yet claimed as complete.

## 5. Branding and visual identity

Original FALIZ branding has been added to the project.

### Branding assets

```text
branding/faliz-logo.svg
branding/faliz-app-icon.png
```

The official app icon is a generated 2048×2048 PNG based on the supplied FALIZ artwork and is integrated into the image as:

```text
/usr/share/icons/hicolor/2048x2048/apps/com.faliz.OS.Shell.png
```

The official SVG logo is included in the live filesystem at:

```text
/usr/share/faliz/faliz-logo.svg
```

### Desktop metadata

The FALIZ shell desktop entry is:

```text
packaging/live/com.faliz.OS.Shell.desktop
```

It is installed into the image at:

```text
/usr/share/applications/com.faliz.OS.Shell.desktop
```

The desktop application identity is:

```text
com.faliz.OS.Shell
```

## 6. Debian live image

A Debian Bookworm live-build pipeline has been implemented for x86_64.

The main build script is:

```text
scripts/build-iso.sh
```

The build uses:

- Debian Bookworm.
- x86_64 / amd64 architecture.
- ISO-hybrid output.
- Linux kernel packages.
- systemd and systemd-sysv.
- NetworkManager.
- PipeWire.
- WirePlumber.
- seatd.
- GTK4 runtime.
- Libadwaita runtime.
- Xwayland.
- Wayland protocol packages.
- KGX terminal.
- Nautilus file manager.
- Adwaita icon theme.
- Noto fonts.
- Syslinux utilities required by the final ISO-hybrid stage.

The package manifest is:

```text
packaging/live/faliz.list.chroot
```

## 7. Live-build compatibility work

The installed live-build version required several compatibility fixes before the ISO could be produced.

### Firmware-index compatibility

The builder was configured to avoid the obsolete firmware index lookup that attempted to request a removed `Contents-amd64.gz` file.

### Kernel package compatibility

The kernel configuration was changed from a combined package/flavour setting to separate values:

- `--linux-packages linux-image`
- `--linux-flavours amd64`

This allowed the Debian Bookworm kernel package to be installed correctly.

### Bootloader compatibility

The installed live-build bootloader template referenced stale paths. The pipeline now copies and corrects the local ISOLINUX template and explicitly supplies:

```text
/usr/lib/ISOLINUX/isolinux.bin
/usr/lib/syslinux/modules/bios/vesamenu.c32
```

### SVG conversion compatibility

A chroot-local `rsvg` compatibility wrapper was added. It translates the legacy live-build arguments into the `rsvg-convert` command format available in the image-building environment.

### Bootlogo compatibility

The final Syslinux stage expected a file named `binary/isolinux/bootlogo` in cpio archive format. The pipeline now:

1. Reads the official FALIZ app icon.
2. Uses Pillow to create a 640×480 splash PNG.
3. Places the splash image in the local ISOLINUX template.
4. Packages it into the expected `bootlogo` cpio archive.

### ISO-hybrid finalization compatibility

The final ISO stage runs inside the Debian chroot, so installing `isohybrid` only on the Ubuntu host was insufficient. The package manifest now includes `syslinux-utils`, ensuring the command is available inside the image-building environment.

### Output filename compatibility

The installed live-build release produces:

```text
.build/live/binary.hybrid.iso
```

The wrapper now copies that file to the stable project output name:

```text
dist/faliz-os-live-amd64.iso
```

## 8. Generated ISO

The verified ISO is:

```text
/home/ubuntu/faliz-os/dist/faliz-os-live-amd64.iso
```

Build size:

```text
1,103,101,952 bytes
```

SHA-256 digest:

```text
e56063b0851b4a093ef4f19ea151bba21a65dd074c3a2ae31ad79d271188789a
```

The checksum is stored in:

```text
/home/ubuntu/faliz-os/dist/faliz-os-live-amd64.iso.sha256
```

The ISO is an ISO 9660 filesystem with:

- DOS/MBR boot-sector data.
- El Torito BIOS boot support.
- `/isolinux/boot.cat` catalog.
- `/isolinux/isolinux.bin` boot image.
- ISO-hybrid MBR support.

## 9. ISO verification

The generated image was checked with `file`, `xorriso`, QEMU, and SquashFS inspection tools.

### File-type verification

`file` reported the image as a bootable ISO 9660 CD-ROM filesystem with a DOS/MBR boot sector.

### El Torito and MBR verification

`xorriso` reported:

- El Torito boot catalog at LBA 38.
- BIOS boot image at `/isolinux/isolinux.bin`.
- Boot-info-table support.
- ISO-hybrid MBR.
- 1,052 MiB image data.

### QEMU smoke test

The image was launched using QEMU without KVM and with no physical disk attached:

```bash
timeout 25s qemu-system-x86_64 \
  -cdrom dist/faliz-os-live-amd64.iso \
  -m 1024 \
  -display none \
  -serial none \
  -no-reboot
```

QEMU remained running for the entire 25-second test window and was then stopped by the timeout. This is a successful bounded boot smoke test, but it does not prove compatibility with every physical computer, GPU, firmware implementation, display manager, or laptop model.

No physical disk was modified.

### Native payload verification

The compressed live filesystem was extracted and inspected. It contains:

```text
/etc/systemd/user/faliz-shell.service
/usr/local/bin/faliz-shell
/usr/share/applications/com.faliz.OS.Shell.desktop
/usr/share/faliz/faliz-logo.svg
/usr/share/icons/hicolor/2048x2048/apps/com.faliz.OS.Shell.png
```

This confirms that the ISO contains the actual native FALIZ shell and branding payload, not merely a static screenshot or placeholder interface.

## 10. Validation suite

The repository validation script is:

```text
scripts/validate.sh
```

It currently checks that:

- Required compiler and `pkg-config` tools exist.
- The native shell can be rebuilt.
- The shell returns its deterministic version string.
- The FALIZ logo exists.
- The systemd service exists.
- Forbidden deployment variable/name patterns are not present in tracked project files.

The validation result was:

```text
FALIZ validation passed
```

A compiler warning originates from the system GTK4 header under the requested pedantic flags; it is outside the FALIZ project source.

## 11. Documentation added or updated

The project includes:

```text
README.md
docs/ARCHITECTURE.md
docs/BUILD.md
docs/DEVELOPMENT_LOG.md
docs/PROJECT_STATUS.md
```

The documentation covers the project overview, architecture boundaries, native build process, ISO creation, QEMU testing, compatibility fixes, milestone history, and current limitations.

## 12. Git history

The most recent milestone commits are:

```text
36570f1 Ignore generated ISO build artifacts
fffd0a9 Produce verified bootable FALIZ live ISO
a8ca077 Integrate official FALIZ app icon branding
```

Earlier commits cover the initial FALIZ shell/live-image foundation, warning cleanup, ISO compatibility documentation, and official icon integration.

## 13. What is complete

The following items are complete for the current milestone:

- Native GTK4/Libadwaita FALIZ shell.
- FALIZ dark futuristic visual theme.
- Live clock and desktop status interface.
- Dock and real application launch actions.
- Native Files, Terminal, and Settings launch integration.
- systemd user-session service integration.
- Official FALIZ SVG logo.
- Official FALIZ app icon.
- Desktop application metadata.
- Debian Bookworm live-build configuration.
- x86_64 ISO-hybrid build pipeline.
- Bootloader compatibility fixes.
- Branded boot splash archive.
- Real bootable live ISO.
- ISO-hybrid and El Torito verification.
- QEMU boot smoke test.
- Native payload inspection inside the compressed live filesystem.
- Repository validation.
- Build and development documentation.

## 14. What is not yet complete

The following items remain future work and are intentionally not claimed as finished:

- A dedicated FALIZ Wayland compositor or compositor configuration.
- A complete graphical installer.
- Disk partitioning and safe installation workflow.
- A dedicated FALIZ login manager experience.
- Lock-screen implementation.
- Full settings application with persistent system controls.
- Hardware-specific GPU validation.
- Broad laptop, desktop, Wi-Fi, Bluetooth, touchpad, suspend, and power-management testing.
- Full accessibility audit.
- Full dark/light theme switching across all applications.
- Additional native FALIZ applications.
- Signed release images and reproducible release build infrastructure.
- Automated hardware and virtual-machine test matrix.

The current ISO should therefore be described as a **verified Debian-based FALIZ OS live image with a native FALIZ shell**, not yet as a finished general-purpose installed operating system.

## 15. Recommended next milestones

### Milestone 1: Dedicated session foundation

- Define the FALIZ Wayland session file.
- Select and integrate a suitable compositor strategy.
- Start the shell through a complete graphical session.
- Add session environment defaults.

### Milestone 2: Login and lock experience

- Add a display-manager strategy.
- Implement or integrate a FALIZ-branded login screen.
- Add screen locking and unlock behavior.
- Test session recovery and logout paths.

### Milestone 3: Settings integration

- Build a native FALIZ Settings application.
- Add network, display, audio, appearance, user, and power controls.
- Connect controls to system services through safe authorized APIs.

### Milestone 4: Installer

- Add a guided installer.
- Detect disks and boot mode safely.
- Require explicit confirmation before destructive operations.
- Support installation to a selected target disk.
- Add bootloader installation and recovery documentation.

### Milestone 5: Hardware and release quality

- Test BIOS and UEFI boot paths.
- Test Intel, AMD, and supported NVIDIA configurations where available.
- Test networking, PipeWire audio, suspend, external displays, and common laptop input devices.
- Add release signing, reproducible build notes, and versioned release artifacts.

## 16. Build commands

### Build the native shell

```bash
cd /home/ubuntu/faliz-os/src/faliz-shell
make
./faliz-shell --version
```

### Run repository validation

```bash
cd /home/ubuntu/faliz-os
./scripts/validate.sh
```

### Build the live ISO

```bash
cd /home/ubuntu/faliz-os
sudo ./scripts/build-iso.sh
```

The build requires root privileges, network access, sufficient free disk space, and the Debian live-build dependencies described by the project documentation.

### Test with QEMU

```bash
cd /home/ubuntu/faliz-os
qemu-system-x86_64 \
  -enable-kvm \
  -m 4096 \
  -cdrom dist/faliz-os-live-amd64.iso \
  -boot d
```

If KVM is unavailable, remove `-enable-kvm`. Hardware compatibility must still be tested separately on physical systems.


## 17. BUILD 02 final status — 2026-11-22

BUILD 02 is a distinct, bootable Debian Bookworm x86_64 ISO that advances FALIZ from the BUILD 01 shell foundation to a real Wayland desktop-session foundation.

Final ISO:

```text
/home/ubuntu/faliz-os/dist/faliz-os-build-02-live-amd64.iso
```

Size: `1,145,044,992 bytes`

Final SHA-256:

```text
0399074963601e3e09959235b0ccb2baa6582fb617e49bf3963a8dfb9849da85
```

Checksum file:

```text
/home/ubuntu/faliz-os/dist/faliz-os-build-02-live-amd64.iso.sha256
```

The ISO is ISO 9660 bootable media with an El Torito BIOS entry at `/isolinux/isolinux.bin`, an isohybrid MBR, and a SquashFS live filesystem. The final SquashFS inspection confirmed these required payloads:

```text
/usr/bin/sway
/usr/bin/swaymsg
/usr/bin/swaynag
/etc/sway/config
/usr/share/wayland-sessions/faliz.desktop
/usr/local/bin/faliz-shell
/usr/local/bin/faliz-settings
/usr/local/bin/faliz-session-autostart
/usr/share/applications/com.faliz.OS.Shell.desktop
/usr/share/applications/com.faliz.OS.Settings.desktop
/usr/share/faliz/faliz-logo.svg
/usr/share/icons/hicolor/2048x2048/apps/com.faliz.OS.Shell.png
```

The build uses Debian's packaged `sway` compositor. The live-build pipeline selects systemd because the sysvinit live transition removed sway during package setup; this also matches the existing FALIZ systemd user-session integration.

### BUILD 02 completed and verified

- Real Debian Bookworm `sway` Wayland compositor payload.
- FALIZ Wayland session desktop entry and sway configuration.
- Four real virtual workspaces with switching and window-movement shortcuts.
- Tiling, floating, fullscreen, reload, and exit window-management bindings.
- Native GTK4/Libadwaita FALIZ shell.
- Functional launcher popover with Files, Terminal, Browser, and Settings entries.
- Animated floating dock styling with application icons and tooltips.
- Toast notifications for shell actions.
- Dark/light theme switch.
- Live clock, status bar, workspace controls, and Wayland status indicator.
- Native FALIZ Settings application with Appearance and System pages.
- Settings controls/status for theme, animation, wallpaper, sway, workspaces, NetworkManager, PipeWire/WirePlumber, Polkit, and Flatpak.
- Mako notification daemon, swaybg wallpaper startup, and FALIZ session autostart helper.
- Custom FALIZ branding, application desktop entries, icon, and boot splash asset.
- Final SquashFS payload inspection.
- Native validation: `FALIZ validation passed`.
- SHA-256 verification: `OK`.
- QEMU software-emulation smoke test: process remained running for 30 seconds and exited with the expected timeout (`qemu_exit=124`). KVM was unavailable in the sandbox, so no KVM claim is made.
- BUILD 01 remains preserved at `dist/build-01/faliz-os-live-amd64.iso`.

Screenshots:

- `screenshots/faliz-build-02-shell.png`
- `screenshots/faliz-build-02-settings.png`

### BUILD 02 incomplete and not claimed

- Dedicated FALIZ graphical login/display manager.
- Lock screen.
- First-time setup wizard.
- Graphical installer, disk partitioning, user creation, or bootloader installation workflow.
- Full native Files, Browser, Store, Gallery, Music, Video, Notes, Calculator, Calendar, and System Monitor application suite. BUILD 02 launches existing Files/Terminal/Browser integrations but does not claim all of these as native FALIZ applications.
- Persistent settings backend for every displayed system control.
- Hardware validation on physical BIOS/UEFI systems, VirtualBox, GPUs, Wi-Fi, Bluetooth, touchpads, suspend, power management, or external displays.
- Complete networking, audio, shutdown, and installation acceptance testing on physical hardware.
- Signed or reproducible release infrastructure.


## 18. BUILD 03 professional OS foundation — 2026-11-23

The first BUILD 03 phase implements professional operating-system foundations while preserving BUILD 02. The session now packages Waybar for a top panel with workspaces, clock, network, audio, battery, tray, lock, and power indicators. It adds swaylock integration, NetworkManager connection editing, PipeWire volume control, and UDisks2 support.

The image also includes two guarded command foundations. `faliz-installer --list` inventories disks, and `faliz-installer --dry-run --target DEVICE` explains a future installation without modifying storage. Installation refuses to run unless the exact target is repeated with `--confirm`, and even then the current foundation exits without changing disks because the graphical installer is not yet implemented. `faliz-update --check` is read-only; package changes require `faliz-update --apply --confirm`.

FALIZ Settings now exposes lock screen, update manager, installer safety, and power-control status. Native shell and Settings version output is BUILD 03 `0.3.0`. Repository validation passes, and safety tests confirmed that mismatched installer confirmations are refused.

This phase does not claim a complete login manager, graphical installer, partitioning, encryption, bootloader installation, full power dialog, or hardware-tested system tray. Those remain the next professional OS implementation steps before expanding the core application suite requested in the second phase.


## 19. BUILD 03 core applications — started after the professional foundation

The FALIZ launcher is now scrollable and includes a practical core suite: Calculator, Calendar, Notes, System Monitor, Screenshot, Software Store, Media Player, Archive Manager, Document Viewer, Image Viewer, Browser, Files, Terminal, and Settings. The BUILD 03 live manifest adds Firefox ESR, GNOME Calculator, Calendar, Text Editor, System Monitor, Screenshot, Software, File Roller, Evince, Eye of GNOME, VLC, LibreOffice Writer, Thunderbird, and GNOME Disk Utility.

This is an honest integration milestone. The applications are maintained Debian programs launched through FALIZ, not yet separate first-party FALIZ-native replacements. Consistent FALIZ wrappers, file associations, settings integration, and first-party utilities remain the next application work.


## 20. BUILD 03 final status — 2026-11-23

BUILD 03 is a distinct bootable ISO that contains both the professional operating-system foundation and the first core application suite.

```text
ISO: /home/ubuntu/faliz-os/dist/faliz-os-build-03-live-amd64.iso
Size: 1,564,475,392 bytes
SHA-256: 655a34444c26e98732988cb89a98f1871a068809d6d92237288e0cfd58c53ecc
```

The final image contains a real Waybar panel, swaylock, NetworkManager connection editor, Pavucontrol, UDisks2, guarded installer/update foundations, Calculator, Calendar, Notes, System Monitor, Screenshot, Software Store, Media Player, Archive Manager, Document Viewer, Image Viewer, Firefox ESR, LibreOffice Writer, Thunderbird, and GNOME Disk Utility. Native validation, checksum verification, SquashFS inspection, and a 30-second software QEMU smoke test passed.

The installer remains deliberately non-destructive and does not yet partition, format, encrypt, or install to a disk. A dedicated graphical login, full power dialog, persistent settings backend, first-party FALIZ-native replacements for every application, VirtualBox test, and physical hardware test are not claimed complete.
