# BoldMerge UI

BoldMerge UI is a monorepo for the BoldMerge Adaptive design system. It contains
the canonical build-time token source, a Lit/Web Components package, a native
C/GObject GTK 4 library, and standalone consumer examples. Web and Native are
independent runtimes built from one reviewed design contract; neither renders
runtime JSON nor imports the other framework.

The approved architecture and measurements are in
[`BOLDMERGE_ADAPTIVE_SPEC.md`](BOLDMERGE_ADAPTIVE_SPEC.md). Values that the
baseline did not specify are marked `proposal` in JSON and recorded in
[`docs/DECISIONS.md`](docs/DECISIONS.md).

## Requirements and commands

Node.js 22 or newer is required for tokens and Web. Native requires Meson,
Ninja, GTK 4, libadwaita, and GObject Introspection.

```sh
npm ci
npm run tokens:validate
npm run tokens:build
npm run tokens:check-generated
npm test
npm run check
```

`tokens:validate` checks structure, names, types, units, layer direction,
references, cycles, all theme/density/accent combinations, and required contrast
for the semantic accent mappings. `tokens:build` writes deterministic artifacts
to `dist/` and example snapshots to `examples/output/`.
`tokens:check-generated` compares the committed files with an in-memory build
without rewriting them.

## Canonical source

The source has three enforced layers:

```text
tokens/primitives/  raw values, accent seeds, and OKLCH strategy
tokens/semantic/    theme, density, typography, interaction, and color roles
tokens/components/  component measurements and semantic aliases
```

References use the exact form `{token.name}`. Primitive tokens cannot reference
later layers; semantic tokens can reference primitives or semantics; component
tokens can reference any earlier layer. Context is declared independently with
`theme`, `density`, or `accent`. The validator resolves the complete Cartesian
product to catch missing or ambiguous values.

JSON is a design and build artifact only. Applications consume generated CSS,
not these files.

## Browser usage and local cascade

Load `dist/css/tokens.css`, then select dimensions independently:

```html
<html data-bm-theme="dark" data-bm-density="compact" data-bm-accent="violet">
  <body>
    <section data-bm-theme="light" data-bm-density="balanced">
      This subtree inherits a local Light/Balanced override.
    </section>
  </body>
</html>
```

Defaults are Light, Balanced, and Cyan. If no root `data-bm-theme` is present,
`prefers-color-scheme: dark` selects Dark. Explicit attributes win and may be
placed on any container because CSS custom properties inherit. Density never
changes automatically with viewport width. `prefers-reduced-motion` reduces
motion durations unless the root explicitly opts into `data-bm-motion="full"`.

Consumer styles reference names such as:

```css
.example-card {
  color: var(--bm-color-text-primary);
  background: var(--bm-color-surface);
  border-radius: var(--bm-card-radius);
  padding: var(--bm-card-padding);
}
```

## Accent generation and contrast

The five preset seeds are Cyan, Blue, Violet, Orange, and Green. For presets and
custom colors, the deterministic algorithm:

1. converts the sRGB seed to OKLCH;
2. preserves its hue and caps chroma with restrained light/mid/dark limits;
3. applies fixed lightness targets for tones 50–950;
4. reduces chroma with a fixed 24-iteration binary search until the color is in
   the sRGB gamut; and
5. rounds once to 8-bit lowercase hexadecimal sRGB.

Generate a reviewed custom accent stylesheet without changing canonical JSON:

```sh
npm run --silent tokens:custom -- '#c026d3' customer > customer-accent.css
npm run --silent tokens:custom -- '#c026d3' customer --platform gtk > customer-accent-gtk.css
```

The command fails if the fixed semantic mapping does not reach 4.5:1 for accent
text/content or 3:1 for the focus indicator against its mapped canvas. These
checks cover only supplied token pairs. Consumers must validate every actual
combination, including states, text size, images, transparency, and locally
overridden backgrounds. A seed is never assumed safe as text merely because it
was accepted as an accent.

## GTK artifact and limitations

GTK CSS does not implement browser custom properties or browser-like local
cascade semantics. `dist/gtk/tokens.css` therefore exports context-qualified
`@define-color` values. Non-color values are emitted as valid CSS comments so
the artifact remains parseable and auditable, but native widget code must apply
those dimensions through its own style classes/properties.

The Native framework must explicitly propagate the active theme, density, and
accent to widget subtrees and select the corresponding qualified colors. It must
also handle runtime custom accents by generating/installing a GTK CSS provider
and updating affected widgets. Loading this file alone does not switch GTK
themes or reproduce Web inheritance.

Exact GTK behavior, font metrics, and rendered color can vary with GTK,
libadwaita, renderer, display profile, and installed fonts. Platform-specific
contrast and screenshot tests remain required. Native applications provision
the pinned Inter v20 and JetBrains Mono v24 families in their runtime image;
fallback behavior is documented in [`docs/FONTS_AND_ICONS.md`](docs/FONTS_AND_ICONS.md).

## Generated files

- `dist/css/tokens.css`: browser artifact;
- `dist/gtk/tokens.css`: GTK-compatible color artifact and numeric-token audit;
- `examples/output/*.css`: committed example/golden snapshots used by tests.
- `native/data/boldmerge-tokens.css`: deterministic Native build snapshot of
  the GTK artifact.
- `native/src/bm-accent-strategy.h`: generated Native compile-time constants
  for the canonical custom-accent strategy and semantic mapping.
- `native/data/component-metrics.css`: generated GTK selectors for resolved
  Button, Input, and Card measurements in both densities.

Generated files are review artifacts and are not currently published.

## Framework packages

The repository contains independently buildable framework packages consuming
the canonical artifacts at build time:

- `web/`: the independent Lit/Web Components implementation;
- `native/`: the independent C/GObject GTK 4/libadwaita foundation.
- `examples/web/` and `examples/native/`: separate consumer applications for
  the Web package and installed Native library.

Neither framework loads token JSON at runtime or imports the other. See
[`native/README.md`](native/README.md) for the Native build requirements,
theme-scope model, demo, and tests; see [`web/README.md`](web/README.md) for Web
imports and component documentation. Third-party font and icon licenses are in
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).

The project itself has not yet selected a distribution license. Do not publish
packages or release binaries until that decision is recorded.
