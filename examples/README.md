# BoldMerge demo applications

The demos are independent consumers. Neither imports source code or runtime
implementation from the other framework.

## Web

Build `@boldmerge/web` first, then install and run the Vite app:

```sh
cd web && npm run build
cd ../examples/web && npm install && npm run dev
```

Use `npm run build` in `examples/web/` for a production bundle.

## Ubuntu Native

Install the Native library to a prefix, then build the separate pkg-config
consumer. Replace `_install` with a system prefix when appropriate.

```sh
meson setup native/build native --prefix="$PWD/_install"
meson compile -C native/build
meson install -C native/build

PKG_CONFIG_PATH="$PWD/_install/lib/pkgconfig:$PWD/_install/lib/x86_64-linux-gnu/pkgconfig" \
  meson setup examples/native/build examples/native
meson compile -C examples/native/build
examples/native/build/boldmerge-native-demo
```

The exact pkg-config directory is distribution-dependent. The Native demo
requires GTK 4, libadwaita, and a graphical session. It is a scrollable catalog
of the currently implemented Native scope: theme/density switching, five
accent presets plus deterministic custom accent, explicit nested overrides,
and Button, Input, Card, Checkbox, Radio Group/Radio, Switch, Select, Icon
Button, Tag, and Status variants and states, plus Page Layout, Section,
Breadcrumbs, Tabs, and Segmented Control. Components not yet present in
the Native library are intentionally not represented by demo-only substitutes.
The catalog includes all thirty-four currently public Native control families,
including Table, Avatar, Empty State, Skeleton, KPI, Segmented Progress, and
the multi-series Chart with its native Chart Tooltip and table alternative.
The catalog also includes Sidebar, Topbar, and App Shell as one composed
desktop workspace preview, plus native Tooltip and Dropdown Menu interactions.
Dialog and explicit Overlay/Push Drawer examples complete the current complex
interaction slice. A local Toast queue and cross-control Validation Summary
demonstrate native feedback behavior.
