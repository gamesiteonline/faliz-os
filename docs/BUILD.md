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
