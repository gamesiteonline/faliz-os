# FALIZ OS architecture

## Foundation

The first target is an Ubuntu Noble live image on x86_64. Mature components remain in place: Linux kernel, systemd, NetworkManager, PipeWire/WirePlumber, seatd, Wayland, Polkit, and Flatpak. FALIZ owns the shell, visual identity, default session configuration, and first-party applications.

## Shell boundary

The current `faliz-shell` is a native GTK4/Libadwaita desktop surface that runs in a Wayland session and provides the first interaction contract: branded desktop surface, status bar, clock, dock, and real launcher actions. It is deliberately not described as a compositor or a complete window manager yet.

The next shell milestone is to move the surface into a dedicated Wayland compositor/session using Smithay or wlroots, then add window lifecycle, workspaces, focus, snapping, keyboard shortcuts, and accessibility settings.

## Application contract

First-party apps will share a small FALIZ design system and communicate with system services through documented D-Bus or portal interfaces. Interfaces must report real state rather than render simulated system controls.

## Safety

Builds write only inside `.build/` and `dist/`. No script in this repository partitions or overwrites disks. Installer work will be isolated behind explicit confirmation and tested in QEMU first.
