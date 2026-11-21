
## 2026-11-21 — ISO pipeline validation

- Root privileges were confirmed necessary for live-build bootstrap.
- Ubuntu Noble was rejected by the installed live-build version because it assumes Debian-style security suites and package names.
- Debian Bookworm bootstrap succeeds, but this live-build version still fails at the kernel stage while requesting a removed `dists/bookworm/Contents-amd64.gz` index.
- The image builder remains in the repository with explicit Debian Bookworm settings so it can be retried with a newer live-build package or a modern image builder such as live-build from upstream, live-bootstrap, or a dedicated Debian build host.
- No physical disk was touched, and no ISO is claimed as produced in this milestone.
