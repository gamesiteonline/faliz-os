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
