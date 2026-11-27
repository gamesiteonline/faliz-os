

## 23. BUILD 04 guarded installation backend — follow-up

The BUILD 04 installer now has a real backend behind layered safety gates. It validates a whole-disk block device, rejects mounted and virtual targets, requires exact target confirmation and an explicit erase acknowledgment, and requires a final `ERASE:/dev/DEVICE` environment confirmation. The planned install path creates a GPT, EFI and ext4 partitions, copies the live FALIZ filesystem, writes fstab and hostname, creates the requested user, regenerates initramfs, and configures GRUB for UEFI or legacy BIOS.

The graphical installer launches this backend only after a separate destructive confirmation dialog and administrator authorization. No destructive operation has been executed in this sandbox. The backend therefore remains an implemented and guarded path that still requires real-disk, UEFI/BIOS, interruption-recovery, and post-install hardware acceptance testing before it can be called production-ready.

## BUILD 04.1 native application suite

BUILD 04.1 now includes a first-party GTK4/Libadwaita native application suite: FALIZ Files, Settings, Software Store, Notes, Calculator, Screenshot, System Monitor, and Media Player. Files, Notes, Calculator, Screenshot, and System Monitor perform local operations directly; Store and Media Player use controlled hand-offs to GNOME Software/PolicyKit and VLC for mature package and codec support. Source compilation and `scripts/validate.sh` pass. The BUILD 04.1 ISO still needs to be rebuilt and smoke-tested before these changes should be described as present in a downloadable image.
