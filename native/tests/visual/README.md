# Native visual baselines

`environment.lock` defines a fail-fast capture profile for Ubuntu 26.04 with
exact GTK/libadwaita, Pango, Cairo, HarfBuzz, fontconfig, Noto, and Xvfb package
versions. `capture.sh` additionally fixes the Cairo software renderer, X11
Xvfb at 96 DPI, a 24-bit 1280×1024 screen, Noto Sans, the C.UTF-8 locale,
disabled animations, and isolated configuration/cache directories. This
profile is a test fixture, not an Ubuntu support or minimum-version decision.

Run the representative Tier A matrix with:

```sh
meson compile -C native/build capture-visuals
```

The target writes four Light/Dark × Balanced/Compact PNG files plus
`SHA256SUMS` under `native/build/tests/visual-output/`. Two consecutive local
runs produced identical hashes for every image. The files are build output,
not approved baselines, and no pixel-comparison test runs by default.

The Tier A fixture currently covers representative Button, Input, Checkbox,
Radio Group/Radio, Switch, Select, Icon Button, Tag, Card, Status, Avatar/
Avatar Group, Empty State, Skeleton, KPI, Segmented Progress, and Chart states.
The complete matrix must expand to all 34 public component families and
relevant states, including overlays, focus/loading/disabled states,
shell/navigation, Table, an active Chart Tooltip, and richer durable-table
content. At least one RTL/pseudo-localized capture is required. Baselines must
be reviewed before they are committed and used as regression gates.
