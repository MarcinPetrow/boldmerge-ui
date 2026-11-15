# BoldMerge Adaptive

**Status:** Approved design and architecture baseline (pre-implementation)  
**Target:** Application-ready v1.0  
**Scope:** Shared design specification, two independent UI frameworks: Web and Ubuntu Native.

> This document records architectural and design decisions. Numeric values not explicitly specified are implementation proposals requiring validation, not retroactive design decisions.

## 1. Product principles

- Product-agnostic UI system for multiple BoldMerge Studio applications.
- Visual direction: technical, precise, content-first; inspired by high-performance monitoring interfaces, without copying any reference design.
- Equal-quality Light and Dark themes, designed separately rather than mechanically inverted.
- Balanced and Compact density, independent of viewport width.
- Restraint: hierarchy through placement and spacing first, then typography, contrast, surface treatment, and finally accent.
- Two independent frameworks with native platform APIs and independent release cycles. No shared UI runtime, JSON renderer, or mandatory common implementation.
- Visual parity means consistent intent, dimensions, behavior, and tokens, not pixel identity across browser and GTK rendering.

## 2. Repositories and architecture

### `boldmerge-design-system`

Canonical, versioned design specification:

```text
boldmerge-design-system/
  tokens/
    primitives/
    semantic/
    components/
  specifications/
    foundations/
    components/
    patterns/
  references/
  docs/
```

### `boldmerge-frameworks`

Independent implementations in one monorepo:

```text
boldmerge-frameworks/
  web/
    src/components/
    src/foundations/
    src/utilities/
    tests/
    package.json
  native/
    src/
    include/
    tests/
    meson.build
  examples/
    web/
    native/
  .github/workflows/
```

| Dimension | BoldMerge Web | BoldMerge Native |
| --- | --- | --- |
| Platform | Modern browsers | Ubuntu / supported GTK 4 Linux |
| Implementation | TypeScript, Lit, Custom Elements | C, GObject, GTK 4, libadwaita |
| Encapsulation | Open Shadow DOM | Native GTK widgets |
| Styling | Scoped CSS, CSS custom properties, `::part()` | GTK CSS, style classes |
| Composition | Named slots and documented public API | GTK widget composition and documented extension points |
| Distribution | npm `@boldmerge/web` | Shared library, headers, pkg-config, GIR/typelib |
| Build | Vite for development, Rollup for library | Meson + Ninja |
| Test | Vitest, Playwright | GLib tests, GTK integration tests |
| Version | Independent SemVer | Independent SemVer |

The specification is versioned independently. Each implementation declares the specification version it implements. No framework imports or depends on the other.

## 3. Public API and extensibility

- **Semantic Contract + Native API**: share component intent, variants, states, interactions, and accessibility requirements; do not force identical method names or signatures.
- **Composable Components**: documented slots/parts/properties on Web; documented widget composition points on GTK. Internal implementation is not public API.
- Web package supports both `import '@boldmerge/web'` (register all) and selective side-effect imports such as `import '@boldmerge/web/button'` (register only the requested element and necessary dependencies).
- Publish ESM, type declarations, and explicit package exports. Ensure selective imports do not eagerly register unrelated elements. Validate bundler behavior; tree-shaking is not guaranteed merely by having exports.
- Web Components work without requiring a consumer to use Lit, React, Angular, or Vue. Framework adapters may be added later if necessary.
- Native library uses conventional GObject properties/signals and GObject Introspection, where technically applicable.
- Stable documented API beginning with each framework's v1.0; breaking changes follow SemVer.

## 4. Foundations

### Geometry, typography, density

- Precision Geometry: radius mostly **8px**, generally **6–10px**.
- Typography: **Inter** for UI, **JetBrains Mono** for data and aligned numerical values.
- Lucide outline icons: **1.5px stroke**, rounded joins, **16/20/24px**, default **20px**.
- Density measurements:

| Element | Balanced | Compact |
| --- | ---: | ---: |
| Button height | 40px | 32px |
| Table row height | 48px | 36px |
| Card padding | 20px | 12px |
| Section gap | 32px | 24px |
| General section/layout gap | 24px | 16px |
| Sidebar item height | 40px | 32px |
| Floating-label input height | 48px | 40px |
| Tab height | 44px | 36px |
| Menu item height | 40px | 32px |

- Minimum touch target **44px** in touch-oriented contexts; Compact visual dimensions must not silently violate accessible touch targets.
- Motion: hover/focus **120ms**, press **80ms**, dropdown/tooltip **120ms**, dialog/drawer **160ms**; easing `cubic-bezier(0.2, 0, 0, 1)`. No click scaling. Respect reduced motion.
- Skeleton: subtle left-to-right shimmer **1.5s**, static under reduced motion.
- Thin neutral scrollbar ~**6px** when supported; retain native scrolling and accessible behavior.

### Token system and themes

- Three token layers: **Primitive → Semantic → Component**.
- Canonical token source: JSON in design-system repository; build-time exports to CSS custom properties and GTK CSS. The JSON is a design artifact, **not** a runtime UI schema.
- Cascading theme: Light/Dark and Balanced/Compact inherited from app, locally overridable by a container. Accent can also be locally overridden.
- Web: attributes and inherited CSS variables; GTK: style classes and explicit propagation as needed (GTK CSS does not reproduce browser cascade semantics automatically).
- Theme changes at runtime; optional system-preference integration.
- Accent: five starter presets **Cyan, Blue, Violet, Orange, Green**, plus arbitrary custom color; generate tonal scales in **OKLCH**, map semantics independently for Light and Dark, and validate contrast for each actual usage.
- Reference accent seed examples (not guaranteed accessible as text): Cyan `#06B6D4`, Blue `#3B82F6`, Violet `#8B5CF6`, Orange `#F97316`, Green `#22C55E`.
- Accent should remain restrained; semantic success/warning/error colors are distinct from arbitrary accent.

### Accent Surfaces (opt-in)

- Default surfaces have **no decorative texture**.
- Optional full-surface accent gradient anchored at a configurable corner, with configurable spread.
- Patterns: diagonal thick stripes, **8px stripe / 8px gap**; or sharp circular dots, **12px diameter**, **36px center spacing**, grid rotated **45°**. Dots remain circles, not diamonds. **No blur**.
- Pattern fades away from anchor corner by default; fading can be disabled.
- Decorative background should be highly transparent; user requested approximately **50% greater transparency** than the earlier reference. Final opacity must be calibrated against reference visuals and contrast checks.

## 5. Component specifications

### Actions and forms

- Button variants: **Primary, Secondary, Outline, Ghost**; **Destructive** is a semantic treatment. No press scaling.
- Inputs: hybrid filled surface with subtle contrast and **1px border**.
- Floating label begins inside input and floats smaller within border when focused/filled.
- Integrated validation: error text inside expanded input area, below a divider; error icon and border treatment. Error is also programmatically exposed.
- Select: classic for short option lists; separate searchable **Combobox** for filtering.
- Checkbox: **4px** radius; Radio: circular **20px**; Switch: **44×24px**, **8px** radius.

### Navigation and layout

- Sidebar: expanded **240px**, collapsed **64px**, mobile drawer. Active row uses **3px vertical accent indicator**, accent icon/text, **no filled active background**; collapsed items have tooltips.
- Topbar: slim global actions (search, notifications, account). Page title, breadcrumbs, and contextual actions belong to content header.
- Page layouts: `contained`, `fluid` (**default**), `split`.
- Cards: flat surface, **8px radius**, **no default border or shadow**, padding **20/12px**; open structure primarily through spacing and typography.
- Sections: spacing-first, **32/24px**; optional separators.
- Tabs: **2px** active underline in accent, **44/36px** heights; segmented controls for mode switching.
- Breadcrumb separator: `/`, subdued ancestors, prominent current item, positioned above page title.
- Hybrid search: topbar trigger opens centralized Command Palette, **Ctrl/⌘+K**, grouped results and keyboard navigation.

### Data, feedback, overlays

- Tables: horizontal **1px row dividers** only; no vertical rules or zebra by default; **48/36px** row height. Row opens detail; independent controls and `⋯` actions must not trigger row navigation.
- Pagination: compact Previous/Next, visible range, total pages, page size **25** by default.
- Status: **text + 7px semantic dot**; never color-only.
- Tags: subtle accent background, accent text, **6px radius**, **12px Inter**, **4×8px padding**.
- Avatar: people circular; teams/organizations rounded square **8px**; sizes **24/32/40/48px**; deterministic fallback, optional availability dot and avatar group.
- Toast: neutral surface, semantic icon, title/message/close, **bottom-right default**, no tinted background or colored strip.
- Dialog: compact, **8px radius**, overlay/shadow, no default section dividers, actions bottom-right, correct keyboard and focus management.
- Drawer: **Overlay default**, Push optional for master-detail; mobile uses Overlay.
- Empty state: neutral icon, title, description, optional action; no default illustration.
- Tooltip: pointer with **6px** triangle and **6px radius**, **400ms hover delay**, keyboard focus and auto-placement.
- Dropdown menu: grouped items, subtle separators before destructive groups, **40/32px** item height.
- Skeleton: shimmer per motion spec above.

### Visualization

- Charts: high-contrast precision, **1.5px** data lines, visible **5px** points, subtle **1px** grid, accent-aware; no default area fill.
- Chart tooltip: category/date header, series labels, aligned monospaced values, colored series dots, multiple series.
- KPI cards: metric + **1.5px** sparkline; sparkline points hidden by default; comparison and change semantics determined by metric (e.g., fewer errors is good).
- Progress: segmented by default, **20** segments configurable, **7px** height, **3px** gap, **2px** radius. Exact numeric value independent of visual segment approximation.

## 6. Accessibility and internationalization

- Web target: **WCAG 2.2 AA** for component behavior and documented usage; native GTK uses analogous accessible behavior via GTK/AT-SPI, without claiming that WCAG directly certifies a GTK library.
- Keyboard navigation, visible focus, correct focus order and restoration, appropriate ARIA/AT-SPI semantics, labels, error association, reduced motion, and screen-reader testing.
- Contrast targets: **4.5:1** ordinary text, **3:1** large text and relevant non-text UI indicators, subject to applicable WCAG exceptions.
- Localizable built-in strings; RTL support for applicable components; never assume text length or Latin-only content.
- Components alone cannot guarantee compliance of an entire consuming application.

## 7. Documentation and testing

- **Documentation First**: static technical documentation, component variants/states, API, HTML/TypeScript and C/GTK examples, theme/density references, accessibility notes. No interactive playground required for v1.0.
- Automated visual regression **separately** for Web and GTK, each against platform-specific approved baselines; controlled fonts, DPI/scale, window size and renderer/environment.
- Web: Vitest for unit tests, Playwright for integration, keyboard, and screenshot tests.
- Native: GLib test framework and GTK integration tests; deterministic screenshot capture where feasible.
- Cover Light/Dark, Balanced/Compact, and relevant states (default, hover, focus, active, disabled, error).
- CI runs tests on pull requests; updating baselines requires review.
- Visual parity is reviewed against the shared design spec, not a fragile cross-platform pixel diff.

## 8. v1.0 scope and release gates

**Application-Ready Release** includes:

- Foundations: tokens, themes, typography, density, icons.
- Actions: buttons and icon buttons.
- Forms: inputs, Select, Combobox, Checkbox, Radio, Switch, integrated validation.
- Navigation: Sidebar, Topbar, Tabs, Breadcrumbs, Command Palette.
- Layout: page layouts, cards, sections.
- Data: tables, pagination, tags, statuses, avatars.
- Feedback: toasts, dialogs, drawers, empty states, skeletons.
- Overlays: tooltips and dropdown menus.
- Visualization: charts, KPI cards, segmented progress.
- Documentation and accessibility testing.

Release gates:
1. All v1.0 components implemented and documented for the framework being released.
2. Both themes and densities work; custom accent behaves and passes applicable contrast checks.
3. Functional, accessibility, and visual regression tests pass.
4. Public API and examples are documented, with SemVer policy.
5. A realistic business app can be assembled using that framework without reimplementing its core UI components.
6. No known critical defects within the release scope.

Web and Native **do not need to release v1.0 simultaneously**.

## 9. Implementation sequence

1. **Foundation:** token pipeline, themes, density, typography, icons, layout foundations.
2. **Core:** buttons, input, checkbox, radio, switch, select, card, tag, status.
3. **Application Shell:** sidebar, topbar, breadcrumbs, tabs, page layout, sections, responsiveness.
4. **Complex Interactions:** dialog, drawer, dropdown, tooltip, combobox, toast, command palette, integrated validation.
5. **Data & Visualization:** tables, pagination, KPI, charts, segmented progress, empty states, skeletons.
6. **Hardening:** accessibility, visual regression, documentation, examples, integration, stable API.

Build the canonical specification/tokens first, then **Web as reference implementation**, then Native against the same spec. Do not port Web implementation into GTK or introduce shared runtime dependencies.

## 10. Implementation guardrails

- Do not silently change approved design decisions. Record unresolved details as proposals/ADRs.
- No runtime JSON UI renderer; no React dependency in the Web library; no cross-framework implementation coupling.
- No automatic mass component scaffolding before the foundations are verified.
- Use native browser/GTK behavior and semantics whenever possible; customize only where required by the design contract.
- Respect third-party font/icon licenses; reference dependencies appropriately rather than embedding unlicensed assets.
- Make build, lint, test, and example commands reproducible; avoid claiming tests passed without running them.
- Do not publish packages, create remote repositories, or push commits without explicit authorization.
