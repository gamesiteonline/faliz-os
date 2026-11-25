

## BUILD 04 guarded installation backend — follow-up

The installer backend now contains the real installation path behind multiple safety gates. In install mode it validates a whole-disk target, refuses mounted or virtual devices, requires root, requires an exact target confirmation plus `--yes-really-erase`, requires the environment confirmation `FALIZ_NONINTERACTIVE_CONFIRM=ERASE:/dev/DEVICE`, creates a GPT with a 512 MiB EFI partition and ext4 root partition, copies the live FALIZ SquashFS, writes fstab and hostname, creates the requested user, regenerates initramfs, and installs a GRUB bootloader for either UEFI or legacy BIOS.

The GUI exposes this backend only after the user reviews the plan and confirms a destructive erase dialog. The backend has not been run against a real disk in this sandbox. Physical hardware, UEFI NVRAM behavior, BIOS boot, encryption, recovery, interrupted-install recovery, and post-install first boot remain required acceptance tests before calling permanent installation production-ready.
