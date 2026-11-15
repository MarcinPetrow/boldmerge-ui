# Native foundation API reference

## `BmThemeScope`

`BmThemeScope` is a final GObject with conventional properties:

| Property | Type | Default | Meaning |
| --- | --- | --- | --- |
| `parent` | `BmThemeScope*` | `NULL` | Explicit parent for inherited dimensions |
| `theme` | `BmTheme` | `INHERIT` | Light, Dark, or explicit inheritance |
| `density` | `BmDensity` | `INHERIT` | Balanced, Compact, or explicit inheritance |
| `accent` | `BmAccent` | `INHERIT` | Five presets, Custom, or explicit inheritance |
| `custom-accent` | `GdkRGBA` | unset | Read-only validated custom sRGB seed; set through the fallible method |
| `effective-*` | corresponding enum | resolved | Read-only resolved dimensions |

Changing a local or parent property reapplies classes to the bound widget.
Parent cycles are rejected by the C precondition. A widget is bound with a weak
reference; the scope does not keep the widget alive. `bm_theme_scope_ref_widget()`
returns a new reference which the caller must release.

A root scope also sets the display's `AdwStyleManager` color scheme. There must
be only one root color-scheme authority per display. Nested scopes deliberately
do not change that global setting; they provide local BoldMerge context, not a
promise that all third-party or libadwaita widget internals can be rethemed per
subtree.

The currently documented foundation classes are `bm-scope`,
`bm-theme-light`, `bm-theme-dark`, `bm-density-balanced`,
`bm-density-compact`, and the five `bm-accent-*` preset classes. Application
authors should normally use `BmThemeScope`, rather than managing these classes
directly. `bm-canvas`, `bm-surface`, `bm-density-sample`, and
`bm-accent-sample` exist solely for the foundation demo and are not stable
component APIs.

## Accessibility and motion

The foundation introduces no custom controls or keyboard behavior. The demo
uses native GTK buttons. The initial CSS adds no animation, so it does not
override the platform's animation preference. Future animated widgets must
observe `GtkSettings:gtk-enable-animations` and their applicable canonical
motion values.

No WCAG or AT-SPI compliance claim is made by this foundation slice. Native
component accessibility and screen-reader testing belong to later component
and hardening phases.
