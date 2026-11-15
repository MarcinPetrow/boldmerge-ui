# BoldMerge Adaptive v1.0 release readiness

Audit date: 2026-11-11  
Approved scope: [`../BOLDMERGE_ADAPTIVE_SPEC.md`](../BOLDMERGE_ADAPTIVE_SPEC.md)  
Verdict: **Neither BoldMerge Web nor BoldMerge Native is Application-Ready v1.0.**

> Post-audit update: the Native toolchain was installed after the original
> audit. The library, demo fixture, GIR/typelib, and separate pkg-config demo
> now build; all four Meson test suites pass, including a representative
> end-to-end AT-SPI tree inspection. Missing v1 scope, visual baselines, manual
> Orca evidence, API/ABI hardening, and release policy still keep
> Native far from v1.0 readiness. The detailed tables below preserve the
> original audit snapshot; current command evidence is recorded at the end.
> Web Core Batch 2C was subsequently completed: Radio Group/Radio and Switch
> now have selective exports, docs, examples, unit/browser/keyboard tests, and
> representative visual baselines. This does not change the v1.0 verdict.
> Web Core Batch 2D subsequently added the classic Select/Option slice with
> native keyboard/form behavior, integrated validation, selective exports,
> documentation, demos, and visual coverage. The v1.0 verdict remains unchanged.
> Web Core Batch 2E subsequently added Icon Button, Tag, and Status with
> selective exports, API docs/examples, keyboard/semantic tests, and visual
> baselines. The icon asset policy remains open and the v1.0 verdict is unchanged.
> Web Application Shell Batch 3A subsequently added Page Layout, Section, and
> Breadcrumbs with selective exports, native link/current-page semantics,
> density-aware composition, docs/examples, and visual coverage. The responsive
> shell and most v1 scope remain incomplete, so the verdict is unchanged.
> Web Application Shell Batch 3B subsequently added Tabs and Segmented Control
> with explicit relationships, form integration, roving keyboard navigation,
> docs/examples, selective exports, and visual baselines. The v1 verdict remains
> unchanged.
> Web Application Shell Batch 3C subsequently added Sidebar, Topbar, and App
> Shell with selective exports, responsive desktop/mobile behavior, focus
> restoration, API docs, focused/catalog demos, and three shell baselines. The
> remaining complex interaction, data, visualization, localization, AT, and
> release-engineering gates keep the v1 verdict unchanged.
> Web Complex Interactions Batch 4A subsequently added Tooltip and Dropdown
> with selective exports, viewport placement, menu keyboard/typeahead behavior,
> API docs, focused/catalog demos, and two overlay baselines. Dialog, Drawer,
> remaining complex/data scope, and hardening gates keep the verdict unchanged.
> Web Complex Interactions Batch 4B subsequently added Dialog and Drawer with
> native modal focus behavior, Overlay/Push modes, nested scroll locking,
> responsive switching, selective exports, docs/demos, and two modal baselines.
> Remaining complex/data scope and hardening gates keep the verdict unchanged.
> Foundation Hardening subsequently pinned and self-hosted Inter v20 and
> JetBrains Mono v24 for Web and added a shared Lucide 1.73.0 icon catalog with
> selective Web and Native APIs, generated GTK resources, licenses, docs, and
> tests. Native controlled environments still must provision the pinned fonts.

This audit evaluates the repository as it exists locally. A passing test for an
implemented slice is not evidence for unimplemented v1.0 scope. Native source
that has not compiled is not classified as implemented or passing.

## Status definitions

| Status | Meaning |
| --- | --- |
| Verified | Implemented and its available build/tests passed in this audit |
| Partial | Some contract or evidence exists, but release requirements remain |
| Source-only | Source/tests/docs exist but could not be compiled or run |
| Not started | No framework implementation exists |
| Blocked | Verification could not start because a required tool/environment is absent |
| Not run | Command was deliberately not attempted after an earlier prerequisite blocked it |

## Release-gate verdicts

| Application-Ready gate | Web | Native | Evidence/gap |
| --- | --- | --- | --- |
| 1. All v1.0 components implemented and documented | **Fail** | **Fail** | Web Core and Application Shell batches through 3C are implemented. Native has Button, Input, and Card. Complex interaction, data, visualization, and substantial Native scope remain absent. |
| 2. Both themes/densities and custom accent pass applicable contrast | **Partial** | **Blocked** | Canonical mappings pass token-pair checks. Web has representative component coverage, not a complete Cartesian/state matrix. Native has no rendered or compiled evidence. |
| 3. Functional, accessibility, and visual regression tests pass | **Fail** | **Blocked** | Current Web tests pass, but v1 scope, screen-reader, localization/RTL, and full visual coverage are absent. Native tests cannot build. |
| 4. Public API/examples documented with SemVer policy | **Partial** | **Partial/source-only** | Current slices are documented. Both packages remain private/provisional `0.0.0`; the specification has no SemVer release. |
| 5. Realistic business application can be assembled without reimplementing core UI | **Fail** | **Fail** | Web now composes a responsive shell and core forms, but overlays, richer data, feedback, and visualization components remain absent. Native lacks the corresponding shell and most controls. |
| 6. No known critical defects in release scope | **Not assessable** | **Not assessable** | Most release scope is absent; Native code has not compiled. |

## Web component status

| v1 area/component | Status | API/docs/example | Test/visual evidence | Principal gap |
| --- | --- | --- | --- | --- |
| Tokens/theme/density/accent/motion | Verified foundation, partial release evidence | README and foundation demo | Unit/browser tests passed | No full browser/platform, preset/state contrast, RTL, or reduced-motion matrix |
| Typography | Implemented, pre-stable | Pinned self-hosted variable fonts, fallback and license policy | Build copies versioned subsets; browser coverage uses package fonts | Cross-platform metric review and unsupported-script fallbacks remain |
| Icons | Implemented, pre-stable | Selective Icon API, reviewed catalog and provenance policy | Unit/API/build coverage | Manual AT, forced-colors and complete visual review remain |
| Button | Verified slice | API doc and demo | Unit, keyboard, integration, screenshots passed | Screen-reader and complete theme×density×state matrix absent |
| Icon Button | Verified slice | API doc and demo | Native keyboard activation, states, selective import, and screenshots passed | Consumer-provided icons only; icon asset policy and manual AT remain open |
| Input | Verified slice | API doc and demo | Form, focus, validation, screenshot tests passed | Cross-browser/AT, localization, and broader input-purpose coverage absent |
| Select | Verified slice | API doc and demo | Native keyboard/form/options/validation, selective-import, and screenshot tests passed | Manual AT, mobile popup, cross-browser, and complete visual matrix absent |
| Combobox | Verified slice | API doc, focused demo, and catalog example | ARIA linkage, filtering, form validity, IME deferral, keyboard navigation, selective import, and paired screenshots passed | Remote loading/ranking, virtualization, mobile keyboard, manual AT, RTL, and multi-browser matrix remain |
| Checkbox | Verified slice | API doc and demo | Native Space/form/mixed/invalid and screenshot tests passed | Group-level error pattern and manual AT matrix absent |
| Radio | Verified slice | API doc and demo | Exclusive form value, roving arrows/Home/End, disabled, selective-import, and screenshot tests passed | Manual AT, RTL, and complete visual matrix absent |
| Switch | Verified slice | API doc and demo | Native Space/form/disabled/switch-role, selective-import, and screenshot tests passed | Manual AT and complete visual matrix absent |
| Integrated validation pattern | Verified slice | Input/Select and Validation Summary docs; focused/catalog demos | Native validity aggregation, actionable focus, dynamic updates, selective import, and paired screenshots passed | Manual AT, localized/RTL errors, server-error integration, and full control matrix remain |
| Sidebar | Verified slice | API doc, focused demo, and catalog example | Desktop geometry, active semantics, mobile dismissal/focus, selective import, and screenshots passed | Provisional breakpoint, routing integration, manual AT/RTL, and cross-browser matrix remain |
| Topbar | Verified slice | API doc, focused demo, and catalog example | Composition, selective import, and shell screenshots passed | Search/account behavior is consumer-owned; responsive overflow and manual AT remain |
| App Shell | Verified slice | API doc, focused demo, and catalog example | State propagation, desktop/mobile behavior, focus restoration, selective import, and screenshots passed | Mobile focus is not modal-trapped; localization/RTL/forced-colors and breakpoint approval remain |
| Tabs | Verified slice | API doc and demo | Tab/panel relationships, roving arrows/Home/End, disabled state, selective import, and screenshots passed | Vertical tabs, routing, manual AT/RTL, and cross-browser matrix absent |
| Breadcrumbs | Verified slice | API doc and layout demo | Native links/focus, current-page semantics, selective import, and screenshots passed | Routing integration, manual AT, RTL, and localization matrix absent |
| Command Palette | Verified slice | API doc, focused demo, and catalog example | Grouped filtering, opt-in shortcut, modal focus, disabled skipping, selection/restoration, selective import, and paired screenshots passed | Remote ranking/history, authorization, shortcut conflicts, manual AT, RTL, and multi-browser matrix remain |
| Page layouts | Verified slice | Fluid/contained/split API doc and demo | Slot composition, canonical widths/gaps, selective import, and screenshots passed | Proposal widths require design review; complex application patterns remain |
| Card | Verified slice | API doc and demo | Composition/accessibility/screenshots passed | Complete visual matrix and integration-app evidence absent |
| Sections | Verified slice | API doc and layout demo | Named region, slots, separator, density spacing, and screenshots passed | Heading policy remains application-owned; manual AT matrix absent |
| Table | Implemented, pre-stable | Native semantics, sorting, selection, row activation, independent controls, density and paired visual coverage | API doc, focused demo, catalog example | Responsive policy, virtualization, manual AT/RTL, forced colors and multi-browser evidence remain |
| Pagination | Implemented, pre-stable | Unit, keyboard, reduced-motion, selective-import and paired visual coverage | API doc, focused demo, catalog example | RTL/manual AT, forced colors, localization and multi-browser evidence remain |
| Tag | Verified slice | API doc and demo | Visible text, selective import, theme/density screenshots passed | Interactive/dismissible behavior intentionally excluded; manual AT matrix absent |
| Status | Verified slice | API doc and demo | Text-plus-7px-dot tones, decorative-dot assertion, selective import, screenshots passed | Dynamic announcement usage and manual AT matrix remain application/release work |
| Avatar | Implemented, pre-stable | Initials, labels, availability, exact geometry, grouping, density independence, selective import and paired visual coverage | API doc, focused demo, catalog example | Image/CDN failures, non-Latin/bidi names, manual AT, forced colors and multi-browser evidence remain |
| Toast | Verified slice | API doc, focused demo, and catalog example | Local bounded FIFO queue, semantic roles, dismissal, reduced motion, selective import, and paired screenshots passed | Manual announcement timing/AT, rapid queue stress, RTL, forced colors, and multi-browser matrix remain |
| Dialog | Verified slice | API doc, focused demo, and catalog example | Native modal focus, Escape/action/backdrop policy, cancellable close, nested scroll lock, restoration, reduced motion, selective import, and screenshot passed | Manual AT, forced colors, browser history policy, and multi-browser matrix remain |
| Drawer | Verified slice | API doc, Overlay/Push demo, and catalog example | Overlay modal behavior, desktop Push, mobile mode switch, nested restoration/scroll lock, reduced motion, selective import, and screenshot passed | RTL visuals, mobile virtual keyboard, complex master-detail integration, and manual AT remain |
| Empty State | Implemented, pre-stable | Named region, heading levels, optional composition, keyboard action, selective import and paired visual coverage | API doc, focused demo, catalog example | Localization/RTL, announcement integration, manual AT, forced colors and multi-browser evidence remain |
| Skeleton | Implemented, pre-stable | Shapes, explicit loading status, 1.5s shimmer, static reduced motion, selective import and paired visual coverage | API doc, focused demo, catalog example | Manual AT, forced colors, RTL, streamed transitions and multi-browser evidence remain |
| Tooltip | Verified slice | API doc, focused demo, and catalog example | Hover delay, focus/Escape, auto-placement, accessible description, selective import, and screenshots passed | Touch long-press, clipping ancestors, manual AT/forced-colors, and cross-browser matrix remain |
| Dropdown Menu | Verified slice | API doc, focused demo, and catalog example | Trigger semantics, arrows/Home/End, typeahead, disabled items, dismissal/focus restoration, selective import, and screenshots passed | Nested menus, localization/RTL browser coverage, destructive confirmation, and manual AT remain |
| Charts/chart tooltip | Implemented, pre-stable | Multi-series lines, keyboard categories, aligned tooltip, summary/table alternative, selective import and paired visual coverage | API doc, focused demo, catalog example | Manual AT, forced colors, RTL, localization, streaming, sampling and multi-browser evidence remain |
| KPI | Implemented, pre-stable | Explicit metric/change meaning, decorative 1.5px sparkline, selective import and paired visual coverage | API doc, focused demo, catalog example | Locale formatting, live updates, manual AT, forced colors and multi-browser evidence remain |
| Segmented Progress | Implemented, pre-stable | Exact progress semantics, rounded configurable segments, reduced motion, selective import and paired visual coverage | API doc, focused demo, catalog example | Indeterminate/loading policy, locale formatting, manual AT, RTL and multi-browser evidence remain |

## Native component status

The Native toolchain is now available locally. Build, install, GIR/typelib,
component tests, and both catalogs execute; rendered baselines and manual
AT-SPI/Orca evidence remain absent.

| v1 area/component | Status | API/docs/example | Test/visual evidence | Principal gap |
| --- | --- | --- | --- | --- |
| Library/pkg-config/GIR skeleton | Verified build slice | README/API policy | Build, local install, pkg-config consumer, GIR and typelib passed | Stable ABI review and pinned CI remain |
| Tokens/theme/density/accent | Implemented, pre-stable | Foundation docs and both catalogs | GLib/GTK tests pass; catalogs launch | Controlled rendered baselines and manual platform review absent |
| Typography | Implemented policy, pre-stable evidence | Pinned families, versions, acquisition and fallback policy | Controlled environment contract documented | Current host and release images must install and verify exact families |
| Icons | Implemented, pre-stable | GObject constructor API, shared catalog and installed provenance | Generated embedded resources and component test | Rendered theme/forced-colors review remains |
| Button | Implemented, pre-stable | GObject API, docs, both catalogs | Component tests pass | Manual AT-SPI, GIR ownership cleanup and screenshots absent |
| Input | Implemented, pre-stable | GObject API, docs, both catalogs | Validation/accessibility tests pass | Manual IME/AT-SPI and screenshots absent |
| Card | Implemented, pre-stable | GObject API, docs, both catalogs | Composition tests pass | GIR ownership cleanup and screenshots absent |
| Icon Button | Implemented, pre-stable | GObject API, docs, both catalogs | Variants, treatment, loading, content and accessible-label tests pass | Icon provenance, manual AT-SPI and screenshots absent |
| Select | Implemented, pre-stable | GObject API, docs, both catalogs | Native dropdown value, required/error validation and theme tests pass | Per-option disabled state, manual AT-SPI and screenshots absent |
| Combobox | Implemented, pre-stable | GObject API, docs, both catalogs | Searchable native dropdown, value, validation, accessibility role and theme tests pass | Editable closed-field parity, per-option disabled state, manual AT-SPI and screenshots absent |
| Checkbox | Implemented, pre-stable | GObject API, docs, both catalogs | Native semantics, required validation, mixed state and theme tests pass | Manual AT-SPI and screenshots absent |
| Radio | Implemented, pre-stable | Radio Group/Radio GObject API, docs, both catalogs | Exclusive value selection, required validation, role and theme tests pass | Manual arrow/RTL/AT-SPI evidence and screenshots absent |
| Switch | Implemented, pre-stable | GObject API, docs, both catalogs | Native switch role/state and signal tests pass | Manual AT-SPI and screenshots absent |
| Integrated validation pattern | Implemented, pre-stable | Input/Select/Combobox/Checkbox/Radio Group plus Validation Summary API, docs, both catalogs | Cross-control validate, error aggregation, alert role, visibility and repair tests pass | Dynamic message-only updates, localization, manual AT-SPI and screenshots remain |
| Sidebar | Implemented, pre-stable | GObject item/navigation API, docs, both catalogs | Navigation role, active/collapsed state, URI signal and composition tests pass | Routing, RTL, manual AT-SPI and screenshots absent |
| Topbar | Implemented, pre-stable | Open composition API, docs, both catalogs | Region composition contract and build tests pass | Responsive overflow, manual AT-SPI and screenshots absent |
| App Shell | Implemented, pre-stable | AdwFlap composition API, docs, both catalogs | Desktop/mobile, collapse/open propagation and focus-trigger contract tests pass | Host breakpoint policy, narrow-window/RTL/manual AT-SPI and screenshots absent |
| Tabs | Implemented, pre-stable | GtkStackSwitcher/Stack GObject API, docs, both catalogs | Selection/value and panel tests pass | Disabled/vertical tabs, manual AT-SPI and screenshots absent |
| Breadcrumbs | Implemented, pre-stable | Native link/current-item API, docs, both catalogs | Navigation role, link/current and composition tests pass | Routing, localization/RTL, manual AT-SPI and screenshots absent |
| Command Palette | Implemented, pre-stable | GObject command/palette API, docs, both catalogs | Group/keyword filtering, disabled skipping, selection, dialog role and theme tests pass | Global accelerator/routing remain app-owned; manual keyboard/AT-SPI and screenshots absent |
| Page layouts | Implemented, pre-stable | Fluid/contained/split API, docs, both catalogs | Composition, proposal widths and mode tests pass | Responsive policy and screenshots absent |
| Sections | Implemented, pre-stable | Header/actions/content/footer API, docs, both catalogs | Region label, separator and composition tests pass | Heading policy, manual AT-SPI and screenshots absent |
| Table | Implemented, pre-stable | GtkColumnView/model/factory API, docs, both catalogs | Sort requests, native multi-selection, row activation, loading/empty state and theme tests pass | Complex cell example, responsive policy, manual AT-SPI/RTL and screenshots absent |
| Pagination | Implemented, pre-stable | GObject API, docs, both catalogs | Range derivation, boundaries, disabled state, page context, navigation role and theme tests pass | Localization, RTL/manual AT-SPI and screenshots absent |
| Tag | Implemented, pre-stable | GObject API, docs, both catalogs | Text/prefix/suffix composition and theme tests pass | Manual visual baselines absent |
| Status | Implemented, pre-stable | GObject API, docs, both catalogs | Five tone classes and text contract tests pass | Live announcement remains application-owned; screenshots absent |
| Avatar | Implemented, pre-stable | Avatar/Avatar Group GObject API, docs, both catalogs | Initials, kind, four fixed sizes, availability, image failure, grouping and overflow tests pass | Remote image policy, non-Latin/bidi review, manual AT-SPI and screenshots absent |
| Toast | Implemented, pre-stable | BmToast/AdwToastOverlay region API, docs, both catalogs | Local queue count, tone, explicit close reason, clear and region-role tests pass | Native queue shows one item; swipe/timeout distinction, manual AT-SPI and screenshots remain |
| Dialog | Implemented, pre-stable | Modal GtkWindow composition/action API, docs, both catalogs | Modal role, action return value, close reason, open state and composition tests pass | Backdrop dismissal differs by platform; Escape/manual focus/AT-SPI and screenshots remain |
| Drawer | Implemented, pre-stable | AdwFlap Overlay/Push and Start/End API, docs, both catalogs | Mode/position/mobile, open-close, return value and composition tests pass | Deprecated compatibility primitive, swipe/RTL/manual AT-SPI and screenshots remain |
| Empty State | Implemented, pre-stable | Region composition API, docs, both catalogs | Heading clamp, icon/action composition, region role and theme tests pass | Dynamic announcements, localization/RTL, manual AT-SPI and screenshots absent |
| Skeleton | Implemented, pre-stable | GObject API, docs, both catalogs | Three shapes, explicit status and system reduced-motion tests pass | Rendered baselines and manual AT-SPI absent |
| Tooltip | Implemented, pre-stable | Native child/content API, docs, both catalogs | Tooltip text, managed child and update contract tests pass | System timing differs from Web token; focus behavior, manual AT-SPI and screenshots absent |
| Dropdown Menu | Implemented, pre-stable | Item/group/popover GObject API, docs, both catalogs | Selection, disabled, destructive, menu-item role and trigger state tests pass | Nested menus/links, RTL, typeahead evidence, manual AT-SPI and screenshots absent |
| Charts/chart tooltip | Implemented, pre-stable | GObject API, docs, both catalogs | Multi-series Cairo plot, pointer/keyboard categories including tested RTL movement, mixed-script formatted values, aligned native tooltip, summary/table alternative and contract tests pass | Rendered baselines, manual AT-SPI, broader locale coverage, streaming and sampling remain |
| KPI | Implemented, pre-stable | GObject API, docs, both catalogs | Text meaning, explicit tone and decorative sparkline tests pass | Localization, manual AT-SPI and screenshots absent |
| Segmented Progress | Implemented, pre-stable | GObject API, docs, both catalogs | Exact clamping, 1–100 segments, rounded fill and progress role tests pass | Localization, RTL/manual AT-SPI and screenshots absent |

## Cross-cutting coverage

### Web

| Dimension | Current evidence | Readiness |
| --- | --- | --- |
| Light/Dark | Runtime switching tested; Tier A representative fixtures cover the complete Light/Dark × Balanced/Compact matrix | Partial: focused states and platform rendering remain targeted rather than Cartesian |
| Balanced/Compact | Independent variables and the complete Tier A Cartesian matrix are tested | Partial: not every transient/focused state is repeated in both densities |
| Accent | Five canonical palettes and custom generator token pairs tested; custom scope switching tested | Partial: actual component states across all accents not contrast-tested |
| Reduced motion | Canonical CSS override plus explicit Skeleton, Progress, overlay, modal, toast, and Pagination assertions | Partial: complete component/state audit and manual vestibular review remain |
| Keyboard/focus | Current controls, Tabs/Segmented Control, mobile shell, Tooltip, Dropdown, dialogs/drawers, and the integration workflow pass in Chromium, Firefox, and WebKit | Partial: manual platform/AT evidence remains |
| Accessibility | Native control semantics, labels/errors, composition assertions, keyboard suites, and a recorded manual AT matrix exist | Partial: manual AT matrix is not executed; no rules-engine audit or compliance evidence |
| Visual regression | Sixty-one Linux Chromium snapshots pass; Tier A fixtures cover all four theme/density combinations, with targeted state, mobile, RTL/pseudo-localized, and integration baselines | Partial: system fallback fonts and one browser/OS; cross-engine images are intentionally not reused |
| Localization/RTL | Arabic RTL navigation/chart geometry, Polish built-in labels (including localized page connector), pseudo-localized long text, bidi-isolated values, and 320px reflow behavior pass in Chromium, Firefox, and WebKit | Partial: real translation bundles, full locale matrix, and manual platform evidence remain |
| API docs/examples | Current Core, Application Shell, and Complex batches through 4C have focused docs/demos | Partial: remaining complex/data/visualization APIs are absent; current package contents were not re-audited after these batches |
| Integration app | The standalone consumer contains both the complete component catalog and a Release Operations workflow composing shell navigation, KPI/Progress/Chart/Table data, approval Dialog, status changes, Pagination, and Toast feedback | Partial: workflow uses synthetic data; behavior passes in all three Playwright engines while visual evidence remains Chromium/Linux only |

### Native

| Dimension | Current evidence | Readiness |
| --- | --- | --- |
| Light/Dark | Runtime switching and compiled class tests pass; a locked Cairo/Xvfb profile generates reproducible representative Light/Dark captures | Partial: candidate images are not reviewed baselines and full component/state coverage remains absent |
| Balanced/Compact | Generated metrics and the independent four-combination class matrix pass; the locked fixture renders both densities reproducibly | Partial: reviewed measurement baselines and full component coverage remain absent |
| Accent | Five presets/custom strategy compile; canonical contrast generation and runtime scope switching pass | Partial: rendered component-state contrast sampling remains absent |
| Reduced motion | GTK animation-setting test and Skeleton static fallback execute successfully | Partial: manual vestibular review and complete animated-state audit remain |
| Keyboard/focus | Native controls retain GTK behavior; component contracts cover focus restoration and Chart LTR/RTL arrows, Home/End/Escape | Partial: broader integration keyboard pass and manual AT-SPI remain |
| Accessibility | Compiled role/property/relation contracts, GIR/typelib generation, and an end-to-end AT-SPI catalog-tree test covering 733 nodes and representative validation states pass | Partial: manual Orca behavior, announcement timing, navigation order, and comprehension remain unverified |
| Visual regression | Exact package/environment lock plus a Cairo/Xvfb generator produce four representative Tier A PNGs covering core, selection, identity, content-state, metric, and chart families with identical hashes across consecutive runs | Partial: outputs are unreviewed build artifacts; no committed baselines, comparison gate, full inventory, RTL, or transient-state matrix yet |
| Localization/RTL | Direction-neutral APIs plus Arabic Chart RTL keyboard movement and mixed Arabic/Polish long-value tests pass | Partial: translation catalogs, extraction, full component/locale matrix and rendered evidence remain |
| API docs/examples | Foundation/component docs, GIR/typelib, in-tree fixture and independent pkg-config catalog build | Partial: transfer annotations and ABI/API gates remain |
| Integration app | Both complete component catalogs build and launch | Partial: catalog data is synthetic; a realistic workflow and integration assertions remain |

## Commands and actual results

The terms in this table describe command execution, not overall release status.

| Command | Result | Actual evidence |
| --- | --- | --- |
| `npm run tokens:build` at repository root | **Passed** | Generated seven deterministic artifacts, including Native metrics/accent outputs |
| `npm run check` at repository root | **Passed** | 291 source tokens and 55 generated tones validated; 7 artifacts verified; 8/8 tests passed |
| `npm run check` in `web/` | **Passed** | Token hash matched; TypeScript and Rollup build passed; the 37-entry API baseline matched; 38/38 Vitest and 144/144 Playwright tests passed, including the complete Tier A theme/density matrix, RTL/localization/forced-colors/reflow, and the consumer workflow |
| `npm run test:firefox` in `web/` | **Passed** | 95/95 portable behavior tests passed after closing a repeated-input Combobox active-descendant defect |
| `npm run test:webkit` in `web/` | **Passed with one explicit skip** | 94 portable behavior tests passed; the forced-colors assertion was skipped because Playwright WebKit does not emulate that media feature |
| `npm run test:browser-matrix` in `web/` | **Passed with one explicit skip** | The combined Firefox/WebKit run passed 189/190 tests; only the documented WebKit forced-colors assertion was skipped |
| `npm run api:check` in `web/` | **Passed** | 37 declaration entries matched the reviewed baseline; package exports, selective source entries, Rollup inputs, side-effect metadata, built outputs, full registration, and component API docs were consistent |
| `npm run build` in `examples/web/` | **Passed** | TypeScript and Vite built the standalone catalog plus Release Operations workflow after the library build; 54 modules transformed |
| `npm pack --dry-run --json` in `web/` | **Blocked first attempt** | npm cache under `/home/marcin/.npm` was read-only (`EROFS`, exit 226) |
| `npm_config_cache=/tmp/boldmerge-npm-cache npm pack --dry-run --json` | **Passed** | Produced a dry-run manifest with 267 files after the Charts build; no package was published |
| `meson setup native/build native` | **Blocked** | `meson` command missing, exit 127 |
| `meson compile -C native/build` | **Not run** | Configuration prerequisite blocked |
| `meson test -C native/build --print-errorlogs` | **Not run** | Configuration prerequisite blocked |
| Native demo/GIR/typelib/install tests | **Not run** | Meson, Ninja, GTK 4, libadwaita, `g-ir-scanner`, and display stack unavailable |
| Environment probe | **Passed as a diagnostic** | `cc` and GLib 2.88.0 present; Meson, Ninja, GTK 4, libadwaita, and GObject Introspection missing |

An initial Batch 3C Playwright run failed one immediate-width assertion because
state propagation completed on Lit's next update. The assertion was corrected
to wait for the rendered CSS state; the targeted rerun passed 3/3 and the final
full run passed 38/38. Blocked and not-run work must not be described as passing.
During Batch 4A, the first targeted Playwright run exposed an invalid
auto-placement fixture assumption and typeahead including decorative prefix
text; a second run exposed an assertion made after the menu was closed. These
were corrected, the targeted suite passed 3/3. A reduced-motion/Tab hardening
test was then added; the final full Web run passed 44/44. These intermediate
failures are not reported as passing runs.

## Public API, SemVer, package, and ABI review

### Shared/specification

- The approved specification has no assigned SemVer version, while each
  framework is required to declare the specification version it implements.
- Canonical tokens and both framework manifests use private `0.0.0`
  placeholders. No compatibility policy can be enforced against an unversioned
  specification contract.
- Repository identity and artifact distribution remain unresolved. Local copied
  snapshots are deterministic, but there is no released cross-repository
  dependency contract.

### Web package

- `@boldmerge/web` is `private: true`, version `0.0.0`, with explicit full and
  selective exports. Selective registration tests passed.
- Lit remains an external runtime dependency and is correctly declared as a
  package dependency; no consumer React/Angular/Vue dependency exists.
- The dry-run package contains built ESM, maps, type declarations, CSS, API docs,
  README, and package metadata. Demo sources are excluded.
- The manifest has no `license`, `repository`, `homepage`, `bugs`, `author`, or
  funding metadata. There is no project LICENSE/NOTICE file to include.
- A deterministic pre-release API baseline now detects declaration and package
  export drift and checks selective-entry consistency. Browser support,
  custom-element collision policy, deprecation windows, SemVer compatibility,
  and package provenance/signing are not established.
- Chromium visual/behavior plus Firefox and WebKit behavior configurations
  pass locally. WebKit skips one unsupported forced-colors emulation assertion;
  supported minimum browser versions and pinned CI remain undecided.

### Native package/ABI

- The intended package is `boldmerge-1`, version `0.0.0`, SONAME 0, GIR
  namespace/version `Bm-1.0`, with installed headers and pkg-config metadata.
  None has been generated or installed successfully in this environment.
- Using GIR namespace `1.0` while the C library is pre-1.0 needs an explicit
  stability decision; it can be interpreted as a stable language-binding API.
- Public transfer/ownership/nullability annotations have not been validated by
  `g-ir-scanner` or language-binding smoke tests.
- No ABI baseline, symbol allowlist/version script, visibility policy,
  `abi-compliance-checker`/`libabigail` job, or upgrade/install test exists.
  Internal `bm_` symbols may become accidental exports until symbol visibility
  is constrained and audited.
- Enum growth, GObject property/signal stability, CSS-class stability, SONAME
  policy, and deprecation windows need a reviewed 1.0 ABI policy.
- Minimum GTK 4/libadwaita APIs are documented, but no supported distribution
  matrix or Ubuntu LTS claim is evidenced.

## Third-party license readiness

- No project-level LICENSE, COPYING, or NOTICE file exists. This blocks a public
  release regardless of dependency status.
- Direct Web dependencies report: Lit BSD-3-Clause; Playwright and TypeScript
  Apache-2.0; Rollup, Vite, Vitest, happy-dom, and `@types/node` MIT. The lockfile
  also contains transitive BSD, ISC, Apache-2.0, MIT, and MPL-2.0 packages.
- A reviewed production dependency inventory and generated third-party notices
  do not exist. Development-only dependencies must be separated from shipped
  runtime obligations in that review.
- GTK 4 and libadwaita are dynamically linked native dependencies and require a
  reviewed LGPL compliance/distribution plan. Meson/Ninja tool licenses and all
  transitive native dependencies must be recorded from the actual build image.
- Inter, JetBrains Mono, and Lucide binaries/assets are not embedded. Before
  controlled visual testing or icon delivery, their exact versions, acquisition
  method, licenses, attribution, and redistribution policy must be decided.
- This section is an engineering inventory, not legal approval. Legal/license
  review remains a release requirement.

## Prioritized blockers

### P0 — prevents meaningful v1 qualification

1. Implement the large missing v1 component scope independently for each
   framework; current demos cannot assemble the required business application.
2. Provision a pinned Native build image and compile the existing Native slice.
   Fix all C, CSS, Meson, GIR, ownership, install, and runtime failures before
   calling any Native component implemented.
3. Assign a specification version and framework pre-release versions; decide
   repository ownership and version-pinned token distribution.
4. Choose and add the project license, package metadata, dependency inventory,
   third-party notices, and font/icon policies.

### P1 — release-gate evidence

5. Establish automated accessibility plus manual browser screen-reader and
   Native AT-SPI/Orca matrices, including errors, focus restoration, high
   contrast/forced colors, reduced motion, and keyboard behavior.
6. Establish full platform-specific visual matrices for all components across
   Light/Dark × Balanced/Compact and relevant states, with controlled fonts,
   renderer, DPI, locale, and baseline-review rules.
7. Add RTL, pseudo-localization, long-text, locale-sensitive content, and
   translated built-in-string tests.
8. Add Web multi-browser CI and a tested Native distribution/support matrix.
9. Add realistic Web and Native integration applications and close all defects
   discovered through them.

### P2 — stable release engineering

10. Add API/ABI diff gates, deprecation tooling, package/install consumer tests,
    SBOM/provenance, changelog/migration policy, and reproducible release jobs.

## Proposed next smallest batch

**Web Hardening 6A — accessibility/AT, RTL/localization, and reduced motion.**

The planned Web component inventory is implemented. The next work is evidence
and defect closure across assistive technology, directionality, localization,
forced colors, and motion preferences.

Acceptance criteria:

1. Build a manual AT matrix and close defects found with representative screen
   readers and keyboard-only operation.
2. Add RTL, pseudo-localization, long-text, and locale-sensitive content tests.
3. Add forced-colors and complete reduced-motion coverage for relevant states.
4. Expand deterministic visual baselines and document environment controls.
5. Do not claim screen-reader or WCAG compliance without recorded evidence.

## Post-audit Native command evidence

Executed after the initial audit:

- `meson setup native/build native --prefix=.../_install`: passed with GTK
  4.22.4, libadwaita 1.9.1, and GObject Introspection 1.86.0.
- `meson compile -C native/build`: passed; library, tests, development demo,
  GIR, and typelib were generated.
- `meson test -C native/build --print-errorlogs`: initially exposed floating
  test ownership and accessible-relation defects; after correction, 4/4 test
  suites passed, including the end-to-end AT-SPI catalog-tree inspection.
- `meson install -C native/build`: passed into the local `_install` prefix.
- Standalone `examples/native` configuration found `boldmerge-1` through
  pkg-config and compiled successfully.
- The standalone GTK application launched successfully on the available
  display and remained running without stderr output.
- `meson compile -C native/build capture-visuals`: passed against the locked
  Ubuntu/Cairo/Xvfb profile; four Light/Dark × Balanced/Compact Tier A images
  were visually inspected and had identical hashes across consecutive runs.

Remaining build warnings: deprecated GTK 4.0-compatible CSS loading calls and
missing GIR transfer annotations. These are release-hardening issues, not
evidence of v1 readiness.
