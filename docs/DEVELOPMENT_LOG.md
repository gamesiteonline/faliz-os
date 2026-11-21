
## 2026-11-21 — ISO pipeline validation

- Root privileges were confirmed necessary for live-build bootstrap.
- Ubuntu Noble was rejected by the installed live-build version because it assumes Debian-style security suites and package names.
- Debian Bookworm bootstrap succeeds after disabling the obsolete firmware-index lookup and separating the kernel image/flavour settings.
- The pipeline now supplies a corrected ISOLINUX template, a Pillow-generated FALIZ bootlogo cpio archive, and an in-chroot `syslinux-utils` dependency for the final ISO-hybrid stage.
- A real `dist/faliz-os-live-amd64.iso` was produced successfully. `file` identifies it as a bootable ISO 9660 image; `xorriso` reports an El Torito BIOS entry and an isohybrid MBR.
- QEMU boot smoke testing ran the ISO for 25 seconds without a boot failure and did not touch a physical disk. This does not establish compatibility with physical hardware.
