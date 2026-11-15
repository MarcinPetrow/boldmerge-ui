# @boldmerge/web

BoldMerge Web foundation package: Lit, TypeScript, open-shadow-root Web
Components, ESM, Vite development, and Rollup library output. It has no React,
Angular, Vue, runtime JSON, or shared cross-framework runtime.

The completed Core slices currently provide `<bm-button>`, `<bm-input>`,
`<bm-card>`, `<bm-checkbox>`, `<bm-radio-group>`/`<bm-radio>`, `<bm-switch>`,
`<bm-select>`/`<bm-option>`, `<bm-icon-button>`, `<bm-tag>`, `<bm-status>`,
`<bm-page-layout>`, `<bm-section>`, Breadcrumbs, Tabs, Segmented Control,
Sidebar, Topbar, App Shell, Tooltip, Dropdown, Dialog, Drawer, Toast,
Validation Summary, Combobox, Command Palette, Pagination, Table, Avatar, and Empty State on the foundation
`<bm-theme>`. No unimplemented v1 components are scaffolded.

## Commands

```sh
npm ci
npx playwright install chromium
npm run dev
npm run tokens:check
npm run typecheck
npm run build
npm run test:unit
npm run test:e2e
npm test
npm run check
```

The browser suite uses the Chromium revision pinned by the exact Playwright
dependency. Screenshot baselines are Linux-specific and load the pinned,
self-hosted Inter v20 and JetBrains Mono v24 variable WOFF2 assets.

The demo is served at `http://127.0.0.1:4173/`. It demonstrates a nested
Dark/Compact/Violet scope inside a Light/Balanced root. Resize the viewport and
density remains unchanged until selected explicitly.

## Imports

```ts
import '@boldmerge/web';            // register every currently shipped element
import '@boldmerge/web/theme';      // selectively register only <bm-theme>
import '@boldmerge/web/icon';       // selectively register only <bm-icon>
import '@boldmerge/web/button';     // selectively register only <bm-button>
import '@boldmerge/web/input';      // selectively register only <bm-input>
import '@boldmerge/web/card';       // selectively register only <bm-card>
import '@boldmerge/web/checkbox';   // selectively register only <bm-checkbox>
import '@boldmerge/web/radio';      // register <bm-radio-group> and <bm-radio>
import '@boldmerge/web/switch';     // selectively register only <bm-switch>
import '@boldmerge/web/select';     // register <bm-select> and <bm-option>
import '@boldmerge/web/icon-button';// selectively register <bm-icon-button>
import '@boldmerge/web/tag';        // selectively register <bm-tag>
import '@boldmerge/web/status';     // selectively register <bm-status>
import '@boldmerge/web/page-layout';// selectively register <bm-page-layout>
import '@boldmerge/web/section';    // selectively register <bm-section>
import '@boldmerge/web/breadcrumbs';// register breadcrumbs and item
import '@boldmerge/web/tabs';       // register tabs, tab, and tab-panel
import '@boldmerge/web/segmented-control'; // register group and segment
import '@boldmerge/web/sidebar';    // register sidebar and sidebar item
import '@boldmerge/web/topbar';     // selectively register <bm-topbar>
import '@boldmerge/web/app-shell';  // selectively register <bm-app-shell>
import '@boldmerge/web/tooltip';    // selectively register <bm-tooltip>
import '@boldmerge/web/dropdown';   // register dropdown, group, and item
import '@boldmerge/web/dialog';     // selectively register <bm-dialog>
import '@boldmerge/web/drawer';     // selectively register <bm-drawer>
import '@boldmerge/web/toast';      // register toast and local toast region
import '@boldmerge/web/validation-summary'; // integrated form error summary
import '@boldmerge/web/combobox';   // register searchable combobox and option
import '@boldmerge/web/command-palette'; // register palette, groups, commands
import '@boldmerge/web/pagination'; // selectively register <bm-pagination>
import '@boldmerge/web/table';      // native table behavior; pair with styles.css
import '@boldmerge/web/avatar';     // register avatar and avatar group
import '@boldmerge/web/empty-state'; // selectively register <bm-empty-state>

import {
  BoldMergeTheme,
  createAccentPalette,
} from '@boldmerge/web/foundation'; // utilities/classes, no registration
```

Load either the canonical variables alone or variables plus document defaults:

```css
@import '@boldmerge/web/tokens.css';
/* or */
@import '@boldmerge/web/styles.css';
```

`styles.css` imports the self-hosted fonts. Consumers that provide their own
document foundations can load `@boldmerge/web/fonts.css` separately.

Package exports are explicit; no unlisted internal path is public API.

Component APIs: [Button](docs/button.md), [Input](docs/input.md),
[Card](docs/card.md), [Checkbox](docs/checkbox.md), [Radio](docs/radio.md), and
[Switch](docs/switch.md), [Select](docs/select.md),
[Icon Button](docs/icon-button.md), [Tag](docs/tag.md), and
[Status](docs/status.md), [Page Layout](docs/page-layout.md),
[Section](docs/section.md), [Breadcrumbs](docs/breadcrumbs.md), [Tabs](docs/tabs.md),
[Segmented Control](docs/segmented-control.md), [Sidebar](docs/sidebar.md),
[Topbar](docs/topbar.md), [App Shell](docs/app-shell.md),
[Tooltip](docs/tooltip.md), [Dropdown](docs/dropdown.md),
[Dialog](docs/dialog.md), [Drawer](docs/drawer.md), and
[Pagination](docs/pagination.md), [Table](docs/table.md), and
[Avatar](docs/avatar.md), and [Empty State](docs/empty-state.md). Vite demos are available
at `/demo/button.html`, `/demo/input.html`, `/demo/card.html`, and
`/demo/checkbox.html`, `/demo/radio.html`, `/demo/switch.html`, and
`/demo/select.html`, `/demo/icon-button.html`, `/demo/tag.html`, and
`/demo/status.html`, `/demo/layout.html`, `/demo/tabs.html`, and
`/demo/shell.html`, `/demo/overlays.html`, `/demo/modals.html`, and
`/demo/pagination.html`, `/demo/table.html`, `/demo/avatar.html`, and
`/demo/empty-state.html`.

A separate package-consumer application is available in `../examples/web/`.
Build this package first, then run `npm install && npm run dev` there.

## Theme scopes

```html
<html data-bm-theme="light" data-bm-density="balanced" data-bm-accent="cyan">
  <bm-theme theme="dark" density="compact" accent="violet">
    Local Dark/Compact/Violet subtree
  </bm-theme>
</html>
```

`theme` accepts `light`, `dark`, `system`, or `inherit`. `density` accepts
`balanced`, `compact`, or `inherit`; it never follows viewport size. `accent`
accepts a preset, `inherit`, or an opaque `#RRGGBB` custom seed. `motion` accepts
`full`, `reduced`, or `inherit`. All may change at runtime.

`<bm-theme>` uses an open shadow root and exposes its inner surface as
`::part(surface)`. Canonical custom properties inherit across its shadow
boundary and remain locally overridable. Invalid custom accents dispatch a
composed, bubbling `bm-theme-error` event.

Custom scales use the canonical deterministic OKLCH strategy and validate the
canonical semantic pairs. Consumers still must validate contrast in every real
component state and background.

## Token provenance

`src/styles/tokens.css` is synchronized byte-for-byte from
`../dist/css/tokens.css`. `src/generated/token-metadata.ts` records its SHA-256
and the current design-token package version. `npm run tokens:check` prevents a
stale snapshot from building unnoticed.

The design-token version is currently the private placeholder `0.0.0`; the
approved specification still lacks a stable SemVer identifier. The hash pins
this implementation to an exact artifact, but publishing remains blocked until
the version and cross-repository distribution policy are approved.

Inter v20 and JetBrains Mono v24 are self-hosted from pinned Fontsource 5.3.0
packages with system fallbacks. Lucide 1.73.0 supplies the reviewed icon
catalog. See `../docs/FONTS_AND_ICONS.md` and `../THIRD_PARTY_NOTICES.md`.
