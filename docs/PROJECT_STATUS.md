

## 22. BUILD 04 graphical installer preview — 2026-11-25

BUILD 04 begins the highest-priority persistent-installation milestone with a native FALIZ Installer preview. The new GTK4/Libadwaita application is available from the FALIZ launcher, dock, and desktop application menu. It guides the user through target-disk discovery, an explicit erase acknowledgment, user and computer-name setup, installation-plan review, and completion reporting.

The live preview calls the guarded `faliz-installer --dry-run` backend and does not modify disks. Destructive partitioning, formatting, encryption, filesystem creation, bootloader installation, permanent user creation, and reboot handoff remain disabled until the complete backend is validated. This is an implemented graphical workflow foundation, not a claim of a completed permanent installer.

BUILD 04 native version checks pass for the shell, Settings, and Installer. The live-build pipeline now defaults to a BUILD 04 ISO name and packages the new graphical installer binary and desktop entry.
