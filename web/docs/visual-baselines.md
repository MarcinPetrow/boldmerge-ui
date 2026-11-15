# Visual baseline policy

Visual snapshots are regression evidence for reviewed rendering contracts. They
do not replace behavior, accessibility, contrast, localization, or manual
platform testing.

## Coverage tiers

### Tier A — theme and density matrix

Representative fixtures for Avatar, Chart, Combobox, Command Palette,
Progress/KPI, Empty State, feedback/validation, Pagination, Skeleton, Table,
and the Release Operations consumer run in the complete Cartesian matrix:

- Light / Balanced
- Light / Compact
- Dark / Balanced
- Dark / Compact

These fixtures cover every shipped component family either directly or through
the integrated catalog/workflow composition. A new public visual component must
join an appropriate Tier A fixture before its batch is complete.

### Tier B — focused state baselines

Keyboard focus, disabled/loading/invalid selection states, overlays, dialogs,
drawers, shell composition, and complete core-control state boards use targeted
baselines. They do not repeat the complete Cartesian matrix when the same theme
and density mappings are already exercised by Tier A.

### Tier C — environmental cases

Mobile shell/drawer, RTL and pseudo-localized content, long text, and other
breakpoint or environment-specific compositions use explicit one-purpose
baselines. Behavioral assertions accompany them where pixels alone cannot
prove the contract.

## Deterministic environment

Playwright controls the following values in `playwright.config.ts`:

- pinned Chromium through the lockfile;
- Linux snapshot suffix;
- 1280 × 900 CSS px viewport unless a test explicitly documents another size;
- device scale factor 1;
- `en-US` locale and UTC timezone;
- headless rendering and no preferred reduced motion;
- explicit BoldMerge theme, density, and accent attributes;
- disabled animations during screenshot comparison.

Screenshot tests are tagged `@visual`. The separate
`playwright.cross-browser.config.ts` excludes that tag so Firefox/WebKit
behavior runs cannot accidentally create or compare Chromium/Linux images.

Fixtures use deterministic local content and must not depend on network data,
wall-clock time, random values, external images, or asynchronous fonts. The
current baselines use system fallback fonts; controlled font binaries and
cross-platform font/rendering evidence remain release gaps.

## Review and update rules

1. Run the failing visual test without snapshot updates and inspect expected,
   actual, and diff images.
2. Decide whether the change is an intended contract change, a product defect,
   or an unstable fixture/rendering condition.
3. Fix defects and instability before changing any baseline.
4. Update only the reviewed spec or test with `--update-snapshots`; never update
   the whole suite merely to make it pass.
5. Inspect every newly written image in both themes and affected densities.
6. Run the updated test without `--update-snapshots`, then run `npm run check`.
7. Record the affected component/state matrix and executed commands in review
   notes. Platform-specific images must never be copied or renamed to claim an
   unexecuted browser or operating-system result.

Baseline filenames include the component or composition, theme, density, and
Playwright platform suffix whenever those dimensions vary. Intentional visual
changes must include implementation, relevant behavior assertions, reviewed
images, and documentation in the same change.
