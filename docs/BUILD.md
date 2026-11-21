# Build and test

## Native shell

```bash
cd src/faliz-shell
make
./faliz-shell --version
```

The graphical shell needs an active GTK/Wayland display. On a developer workstation it can be launched from a Wayland session with `./faliz-shell`.

## Live ISO

The image pipeline uses live-build and produces an x86_64 ISO-hybrid image. Run it from a machine with root privileges, network access, and at least 12 GB free space:

```bash
./scripts/build-iso.sh
```

## QEMU smoke test

```bash
qemu-system-x86_64 -enable-kvm -m 4096 -cdrom dist/faliz-os-live-amd64.iso -boot d
```

If KVM is unavailable, remove `-enable-kvm`; the boot will be slower. Hardware compatibility is not considered verified by a QEMU boot.

## Current acceptance checks

- Shell compiles with the system GTK4 and Libadwaita headers.
- `--version` returns a deterministic build identity.
- ISO build script checks required tools and keeps all output in the repository.
- No deployment configuration includes environment variables with the `manus` prefix or name.
