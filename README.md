<div align="center">

<img src="branding/faliz-app-icon.png" alt="FALIZ OS icon" width="140">

# FALIZ OS

### A professional Debian-based Wayland desktop experience

[![BUILD 04](https://img.shields.io/badge/BUILD%2004-guarded%20installer-7c5cff?style=for-the-badge)](https://github.com/gamesiteonline/faliz-os/releases/tag/v0.4.1)
[![Debian Bookworm](https://img.shields.io/badge/Debian-Bookworm-a81d33?style=for-the-badge&logo=debian&logoColor=white)](https://www.debian.org/releases/bookworm/)
[![Architecture](https://img.shields.io/badge/Architecture-amd64%20%2F%20x86__64-1f6feb?style=for-the-badge)](https://github.com/gamesiteonline/faliz-os)
[![License](https://img.shields.io/badge/License-MIT-2ea44f?style=for-the-badge)](LICENSE)

**FALIZ OS** is a Linux desktop operating-system project created by **Fahad Mohamed**. It combines a stable Debian foundation with Wayland, Sway, GTK4, Libadwaita, systemd, PipeWire, NetworkManager, and a native FALIZ desktop shell.

[Download BUILD 03](https://github.com/gamesiteonline/faliz-os/releases/tag/v0.3.0) · [View the source code](https://github.com/gamesiteonline/faliz-os) · [See the screenshots](screenshots/faliz-build-02-15-screen-gallery.png)

</div>

## About the creator

FALIZ OS is created by **Fahad Mohamed from Tanzania** 🇹🇿. Fahad is an **Advanced-level student** who spent approximately **two years** developing this operating-system project, learning through design, programming, desktop integration, testing, and continuous improvement.

FALIZ OS is an independent project built with determination and a focus on creating a polished, welcoming Linux desktop experience. The project is still evolving, and each release documents what has been implemented and verified rather than presenting unfinished ideas as completed features.

## BUILD 04 highlights

BUILD 04 builds on the polished Debian Bookworm live desktop for amd64/x86_64 computers. It includes a real Sway Wayland session, a native GTK4/Libadwaita FALIZ shell and Settings application, a professional Waybar panel, floating dock, launcher, workspaces, notifications, dark and light themes, branding, and application shortcuts.

The new native **FALIZ Installer** guides users through storage-device discovery, an explicit erase acknowledgment, user setup, installation-plan review, and a safe dry run. After a second destructive confirmation and administrator authorization, the guarded backend can prepare GPT/EFI/ext4 storage, copy the live system, create a user, generate fstab and initramfs, and configure GRUB for UEFI or legacy BIOS. The image also includes swaylock integration, NetworkManager tools, audio controls through Pavucontrol, UDisks2 support, GNOME Disk Utility, Firefox ESR, LibreOffice Writer, Thunderbird, and a broader desktop application launcher.

## Download

Download the verified BUILD 04.1 image from the [FALIZ OS BUILD 04.1 release page](https://github.com/gamesiteonline/faliz-os/releases/tag/v0.4.1).

- [Download the FALIZ OS BUILD 04.1 ISO](https://github.com/gamesiteonline/faliz-os/releases/download/v0.4.1/faliz-os-build-04-live-amd64.iso) — approximately 1.48 GiB.
- [Download the BUILD 04.1 SHA-256 checksum](https://github.com/gamesiteonline/faliz-os/releases/download/v0.4.1/faliz-os-build-04-live-amd64.iso.sha256).

After downloading both files, verify the ISO before writing it to a USB drive:

```bash
sha256sum -c faliz-os-build-04-live-amd64.iso.sha256
```

The expected SHA-256 digest for BUILD 04.1 is:

```text
f6e18b27f2b4f9c60dd2ca90c74447931f216e38fcc313c0425ec87ea347530d
```

## In-place updates after installation

FALIZ includes an **FALIZ Update Manager** for installed systems. Future FALIZ releases can publish a checksum-verified component bundle containing the shell, Settings, native applications, desktop entries, styles, and updater. The installed system downloads that bundle and updates those components in place; it does **not** overwrite the whole root filesystem and does **not** require reinstalling the ISO.

Check for a release without changing anything:

```bash
faliz-update --check
```

Apply a published FALIZ component update only after reviewing the version and release notes:

```bash
sudo faliz-update --apply --confirm
```

The updater verifies the bundle SHA-256 checksum, rejects archive paths outside the approved FALIZ component list, backs up replaced files under `/var/lib/faliz/backups/`, and supports rollback with `faliz-update --rollback VERSION`. Debian package updates remain a separate, explicit operation using `faliz-update --packages --check` and `faliz-update --packages --apply --confirm`. The ISO itself remains a fresh-install artifact; it is no longer the only way to receive FALIZ application and desktop changes.

Release maintainers can create a bundle with `scripts/create-update-bundle.sh VERSION`, upload the generated archive to a GitHub Release, and publish its checksum and asset URL in `updates/stable/manifest.json`.

## Install and boot FALIZ OS from a USB drive

BUILD 03 is a bootable live ISO. The steps below create a USB device that boots FALIZ OS so you can test the desktop on a PC without immediately changing the computer's internal disk.

### What you need

- A 64-bit PC or laptop with at least 4 GB RAM recommended.
- A USB drive with at least 8 GB capacity. Everything on this drive will be erased.
- The FALIZ OS BUILD 03 ISO.
- Either **Rufus** on Windows or **Ventoy** on Windows/Linux.
- A backup of important files. Never select a drive containing files you need to keep.

### Method A — Rufus on Windows

1. Download and install Rufus from [rufus.ie](https://rufus.ie/).
2. Insert the USB drive into the PC.
3. Open Rufus. Under **Device**, carefully select the correct USB drive.
4. Click **SELECT** and choose `faliz-os-build-03-live-amd64.iso`.
5. For most modern computers, use **GPT** and **UEFI**. For older BIOS-only computers, use the option appropriate for that computer.
6. Keep the recommended file-system settings, then click **START**.
7. If Rufus asks whether to write in ISO Image mode or DD Image mode, choose the recommended default first. If the USB does not boot, recreate it using the other mode.
8. Confirm the warning that the USB will be erased and wait until Rufus reports **READY**.
9. Safely eject the USB, insert it into the target PC, and restart the computer.
10. Open the PC's boot menu during startup. Common keys are **F12**, **F9**, **F10**, **Esc**, or **Del**, depending on the manufacturer.
11. Select the USB device and choose the FALIZ OS live entry. FALIZ OS should start without installing to the internal disk.

### Method B — Ventoy on Windows or Linux

1. Download Ventoy from [ventoy.net](https://www.ventoy.net/).
2. Run Ventoy2Disk and select the correct USB drive. Ventoy will erase the selected drive during installation.
3. Click **Install** and confirm the warnings.
4. After Ventoy finishes, open the new Ventoy USB drive in the file manager.
5. Copy `faliz-os-build-03-live-amd64.iso` directly onto the USB drive. Do not extract the ISO.
6. Safely eject the USB and connect it to the target PC.
7. Restart the PC and open its boot menu using the manufacturer's boot key.
8. Select the Ventoy USB, choose the FALIZ OS ISO from the Ventoy menu, and boot it.

### First boot notes

Choose the live desktop option shown by the boot menu. The live environment is designed for evaluation and testing. Check the display, keyboard, touchpad, network, audio, applications, and workspace controls before deciding whether the hardware is suitable.

BUILD 04.1 provides the graphical workflow and a guarded backend for partitioning, filesystem setup, user creation, initramfs generation, and bootloader configuration. However, it has not been executed against a real disk in this build environment. Test it first on disposable hardware or a virtual machine with backups; UEFI NVRAM behavior, legacy BIOS installation, interrupted-install recovery, encryption, and post-install first boot still require acceptance testing. Never select a disk containing data you need to keep.

## Verification and project status

The BUILD 03 ISO has been checked as a bootable ISO 9660 image with an El Torito BIOS entry and isohybrid MBR. Native validation passed for the FALIZ shell, FALIZ Settings, packaging, and release tooling. A QEMU software-emulated smoke test completed; KVM, VirtualBox, and physical hardware validation were not available in the build environment.

The project remains an active engineering foundation. A dedicated graphical login/display manager, full graphical installer, persistent Settings backend, complete native replacements for every integrated Debian application, and broader hardware validation remain future work.

## Social media and developer links

<div align="center">

[![Threads](https://img.shields.io/badge/Threads-000000?style=for-the-badge&logo=threads&logoColor=white)](https://www.threads.com/@ard.sing)
[![Reddit](https://img.shields.io/badge/Reddit-FF4500?style=for-the-badge&logo=reddit&logoColor=white)](https://www.reddit.com/u/gamesiteonline)
[![Instagram](https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white)](https://www.instagram.com/ard.sing)
[![WhatsApp Channel](https://img.shields.io/badge/WhatsApp_Channel-25D366?style=for-the-badge&logo=whatsapp&logoColor=white)](https://whatsapp.com/channel/0029VbChyDUI1rcht5jajL3q)
[![YouTube](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://youtube.com/@gamesiteonline?si=2qKZs5YUptHS6cTd)
[![Google Play Games](https://img.shields.io/badge/Google_Play_Games-01875F?style=for-the-badge&logo=googleplay&logoColor=white)](https://my.play/gamesiteonline)

</div>

## License

The project source is released under the MIT License. Linux, GTK, Libadwaita, systemd, and other upstream components retain their own licenses.
