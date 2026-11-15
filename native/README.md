# BoldMerge Native foundation

BoldMerge Native is the independent GTK implementation of the approved
BoldMerge Adaptive specification. It contains the shared-library foundation,
canonical GTK token loading, explicit theme scopes, and the Button, Input,
Card, Checkbox, Radio Group/Radio, Switch, Select, Icon Button, Tag, and Status
slices, plus Page Layout, Section, Breadcrumbs, Tabs, Segmented Control,
Sidebar, Topbar, App Shell, Tooltip, Dropdown Menu, Dialog, and Drawer. It
also includes Combobox, Command Palette, Table, Pagination, Avatar/Avatar
Group, Empty State, Skeleton, KPI, Segmented Progress, Toast/Toast Region, and
Validation Summary, plus multi-series Chart and Chart Tooltip. It contains no
Web runtime, runtime JSON loader, or JSON UI renderer.

## Requirements

- Meson 0.61 or newer;
- Ninja;
- a C11 compiler;
- GTK 4.0 or newer development files;
- libadwaita 1.0 or newer development files;
- GObject Introspection development tools when GIR/typelib output is wanted;
- a graphical session or virtual display for the GTK smoke test and demo.

These minimum GTK/libadwaita versions follow the APIs currently used
(`GtkCssProvider`, display-scoped providers, GTK CSS classes,
`AdwApplicationWindow`, and `AdwStyleManager`). They are API minimums, not a
claim that any particular Ubuntu LTS release is supported. A distribution
support matrix requires CI on that exact distribution and remains undecided.

## Build and test

From the repository root:

```sh
meson setup native/build native
meson compile -C native/build
meson test -C native/build --print-errorlogs
native/build/demo/boldmerge-native-demo
```

The distribution-boundary demo is the separate pkg-config consumer in
`../examples/native/`. See `../examples/README.md` for install-prefix and build
commands. Both executables are scrollable catalogs of every currently public
Native control and its principal variants/states; the in-tree demo remains a
library-development fixture.

Component contracts and examples are documented in
[`docs/COMPONENTS.md`](docs/COMPONENTS.md). Intentional platform differences
and the current rendered-review gap are recorded in
[`docs/VISUAL_PARITY.md`](docs/VISUAL_PARITY.md).
Automated AT-SPI evidence and the still-unexecuted manual Orca matrix are
recorded in
[`docs/ACCESSIBILITY_HARDENING.md`](docs/ACCESSIBILITY_HARDENING.md).

Use `-Dintrospection=disabled`, `-Ddemo=false`, or `-Dtests=false` for explicit
minimal builds. In the default `auto` mode, GIR and typelib files are generated
only when GObject Introspection is installed. `-Dintrospection=enabled` makes a
missing introspection toolchain a configuration error.

The canonical compiler writes `data/boldmerge-tokens.css` byte-for-byte from
the same exporter as `../dist/gtk/tokens.css`; drift checks cover both files.
Meson embeds this generated snapshot as a GResource and installs it under the
package data directory. Canonical JSON is never read by the Native build or at
runtime.

The compiler also writes `src/bm-accent-strategy.h`. Native custom-accent code
consumes its lightness, chroma, canvas, and semantic-role mapping constants;
the build fails explicitly if a changed role mapping needs new native logic.
This prevents a hand-maintained second token source.

## Foundation API

The introspection namespace is `Bm`, the C symbol prefix is `bm_`, public types
start with `Bm`, and installed headers live under `boldmerge-1/boldmerge`.

Call `bm_init_for_display()` once for each display. It is idempotent and loads
the generated token artifact plus the small GTK runtime bridge. Create a
`BmThemeScope`, set its independent theme, density, and accent dimensions, and
bind it to a subtree root with `bm_theme_scope_apply()`.

GTK does not reproduce browser custom-property inheritance. Nested overrides
are therefore explicit objects:

```c
BmThemeScope *application = bm_theme_scope_new (NULL);
bm_theme_scope_set_theme (application, BM_THEME_DARK);
bm_theme_scope_set_density (application, BM_DENSITY_BALANCED);
bm_theme_scope_apply (application, application_root);

BmThemeScope *panel = bm_theme_scope_new (application);
bm_theme_scope_set_density (panel, BM_DENSITY_COMPACT);
bm_theme_scope_apply (panel, panel_root);
```

An `INHERIT` dimension resolves through the explicit parent scope. Root theme
inheritance follows `AdwStyleManager`; root density defaults to Balanced and
root accent to Cyan. Setters update an already-bound widget at runtime. Dynamic
subtree roots must be bound explicitly; arbitrary GTK descendants are not
silently walked or mutated.

The root scope is the display's color-scheme authority: explicit Light/Dark
uses libadwaita's force-light/force-dark scheme so stock controls agree with the
BoldMerge canvas. Use one root authority per display. Nested scopes apply
BoldMerge context classes and inherited canonical colors locally; libadwaita
does not offer an equivalent per-subtree color-scheme switch for every stock
widget, so consumers must not assume a nested override restyles arbitrary Adw
widget internals. This platform limitation is why propagation is explicit.

`bm_theme_scope_set_custom_accent()` accepts an opaque sRGB `GdkRGBA`, runs the
canonical fixed OKLCH lightness/chroma/gamut-mapping strategy, validates the
same text, solid, subtle, and focus contrast pairs, and installs scope-qualified
literal GTK CSS. This validation covers only canonical token pairs. Actual
component states and application backgrounds still require platform testing.
GTK custom-accent output is sRGB-only; alpha seeds and wide-gamut color spaces
are rejected rather than silently converted.

## API stability

The library is pre-1.0 and its API is provisional. Documented API becomes
stable at 1.0. From that point, compatible additions use minor releases and
breaking ABI/API changes require a new major release. Public installed headers,
GObject properties, enum values, GIR names, pkg-config identity, and documented
CSS classes are covered. Internal source, private selectors, demo code, and test
fixtures are not public API. Deprecations will remain available for at least
one minor release where technically practical.

The package currently declares specification version
`unversioned-approved-baseline`; publishing is blocked until the approved
specification receives a version.
