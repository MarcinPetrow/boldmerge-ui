# BoldMerge Adaptive decision log

This log records implementation decisions and unresolved decisions beneath the
approved baseline in [`../BOLDMERGE_ADAPTIVE_SPEC.md`](../BOLDMERGE_ADAPTIVE_SPEC.md).
It does not override that baseline. A proposal becomes accepted only through
review; unspecified numeric values remain proposals requiring validation.

## Status vocabulary

- **Approved baseline:** directly required by the approved specification.
- **Accepted for implementation:** local implementation choice consistent with
  the baseline.
- **Proposed:** recommended but awaiting review.
- **Open/blocking:** must be resolved before the named release or milestone.
- **Deferred:** intentionally outside the current milestone.

## Decision records

### BM-001 — Treat this workspace as the design-system bootstrap

- Status: Superseded as a repository-layout assumption; publication identity remains open
- Date: 2026-11-10
- Decision: Plan this workspace as `boldmerge-design-system`, not
  `boldmerge-frameworks` and not a third `boldmerge-ui` runtime.
- Rationale: It contains only the canonical baseline, and the approved sequence
  requires specification/tokens before Web and Native.
- Consequence: The workspace subsequently became a local integration area with
  independent `web/` and `native/` prototypes. This does not authorize a third
  runtime or resolve repository ownership. Confirm the canonical repository
  name/ownership before initializing a remote or publishing artifacts.

### BM-002 — Keep token JSON build-time only

- Status: Approved baseline
- Date: 2026-11-10
- Decision: JSON describes design tokens and is consumed only by build and
  validation tools. Runtime outputs are static CSS/GTK CSS; JSON never drives UI
  construction.
- Consequence: No component registry, JSON UI schema, runtime token loader, or
  shared renderer is allowed.

### BM-003 — Preserve two independent framework runtimes

- Status: Approved baseline
- Date: 2026-11-10
- Decision: Web and Native share a versioned semantic/design contract, not
  implementation code. Each has its own dependencies, builds, tests, APIs,
  release cycle, and platform-specific visual baselines.
- Consequence: A future frameworks monorepo may share repository automation but
  must not introduce a shared UI/runtime module or make either framework depend
  on the other.

### BM-004 — Use an explicit, validated token contract

- Status: Proposed for M1
- Date: 2026-11-10
- Decision: Validate token files with local JSON Schema Draft 2020-12. Require a
  stable logical name, value/type, description, and provenance/status. Reject
  duplicate names, unknown references, cycles, invalid units, and unknown
  fields.
- Rationale: The baseline defines layers but not a serialization schema.
  Explicit validation prevents accidental API and value drift before framework
  implementation.
- Consequence: Schema changes are reviewed contract changes. M1 tests include
  invalid fixtures and deterministic output goldens.

### BM-005 — Use small deterministic exporters first

- Status: Accepted for implementation
- Date: 2026-11-10
- Decision: Implement narrow ECMAScript module exporters for CSS custom
  properties and GTK CSS rather than adopting a broad token framework.
- Rationale: The required transformations are initially small, GTK semantics
  need explicit handling, and avoiding premature abstraction follows the
  foundation-first guardrail.
- Consequence: Re-evaluate if theme/accent work demonstrates needs that a mature
  token tool handles more safely. Generated files have stable ordering and are
  checked for byte-level drift.

### BM-006 — Use npm and a locked, development-only toolchain

- Status: Accepted for implementation; revised after implementation spike
- Date: 2026-11-10
- Decision: Use npm with a committed lockfile and Node's built-in test runner.
  The validator, deterministic compiler, and tests use standard ECMAScript
  modules and no third-party development or production dependencies.
- Rationale: The compiler is small enough for explicit validation and exporters;
  removing the proposed TypeScript/Ajv/Vitest stack reduces supply-chain and
  maintenance cost without weakening the checked contract.
- Consequence: The supported Node version must be pinned before CI acceptance;
  the locally observed version alone is not a support decision.

### BM-007 — Separate compact visual size from accessible hit area

- Status: Accepted interpretation of approved baseline
- Date: 2026-11-10
- Decision: Compact measurements define visible component geometry. In
  touch-oriented contexts, controls below 44px require at least a 44px effective
  target or use of Balanced density.
- Rationale: This satisfies both specified density and touch-target requirements
  without coupling density to viewport width.
- Consequence: Component specifications and tests must distinguish visual bounds
  from effective target bounds.

### BM-008 — Prefer logical direction in public contracts

- Status: Proposed
- Date: 2026-11-10
- Decision: Use start/end and logical CSS/GTK concepts where applicable; map
  approved physical visuals to logical direction deliberately for RTL.
- Consequence: Sidebar indicator placement and accent-surface anchor behavior
  need explicit RTL examples before their component/foundation specifications
  are accepted.

### BM-009 — Keep generated artifacts co-versioned initially

- Status: Proposed; open before cross-repository handoff
- Date: 2026-11-10
- Decision: During M1, check generated CSS/GTK CSS into `dist/` beside the source
  tokens for review and deterministic drift checks. Do not publish them yet.
- Consequence: Before frameworks consume them, decide whether distribution is a
  release archive, package, vendored snapshot, or another version-pinned method.
  Manual unversioned copying is not acceptable.

### BM-010 — Adopt restrained provisional neutral and semantic palettes

- Status: Accepted for implementation as proposals, not approved design values
- Date: 2026-11-10
- Decision: Use a cool-neutral palette with Light canvas `#f5f7f9`, surface
  `#ffffff`, and primary text `#15181e`; use Dark canvas `#0d0f13`, surface
  `#15181e`, and primary text `#f9fafb`. Success, warning, and error use
  independent green, amber, and red pairs rather than arbitrary accent colors.
- Rationale: The implementation requires complete Light/Dark artifacts while
  the baseline supplies no full palette. The values are restrained, technical,
  and independently mapped rather than mechanically inverted.
- Consequence: Every value is marked `proposal`. Design review, real component
  usage, and platform contrast tests may revise them before a stable release.

### BM-011 — Fix a deterministic OKLCH accent strategy

- Status: Accepted for implementation as a proposal
- Date: 2026-11-10
- Decision: Convert the sRGB seed to OKLCH, preserve hue, apply fixed tone
  lightness values, cap chroma at 0.12/0.20/0.14 for light/mid/dark tones,
  reduce chroma with 24 binary-search iterations until in sRGB gamut, then round
  to 8-bit lowercase hex. Light maps text/solid to tone 700; Dark maps them to
  tone 300. Subtle and focus roles have independent mappings.
- Consequence: The compiler validates 4.5:1 text/content and 3:1 focus-indicator
  pairs for every preset and rejects a generated custom accent if those fixed
  mappings fail. Actual component/state usage still requires contrast testing.

### BM-012 — Export GTK colors and audit numeric tokens without pretending to cascade

- Status: Accepted for implementation
- Date: 2026-11-10
- Decision: Export context-qualified GTK `@define-color` declarations. Emit
  resolved non-color tokens as valid CSS comments because GTK CSS has no custom
  properties for reusable arbitrary dimensions.
- Consequence: Native code must explicitly propagate theme/density/accent and
  apply numeric values through GTK style classes or properties. The artifact is
  parseable and auditable but loading it does not recreate Web inheritance.

### BM-013 — Choose restrained non-color defaults

- Status: Accepted for implementation as proposals
- Date: 2026-11-10
- Decision: Use regular/medium weights 400/500, disabled opacity 0.46, dialog
  shadow `0 16px 48px rgb(0 0 0 / 0.24)`, accent-surface pattern opacity 0.08,
  and normalized gradient spread 0.65. Tag padding is interpreted as 4px block
  by 8px inline. Font stacks add system fallbacks without bundling font files.
- Consequence: These tokens remain visibly marked `proposal`; accent-surface
  opacity in particular requires calibration against the missing reference.

### BM-014 — Pin Web to a canonical token snapshot, not token JSON at runtime

- Status: Accepted for Web foundation implementation
- Date: 2026-11-10
- Decision: `@boldmerge/web` synchronizes the canonical browser CSS artifact at
  build time and records its design-package version and SHA-256. The sync step
  also derives custom-accent strategy constants from that CSS artifact. Web
  application code never reads canonical JSON.
- Rationale: The design-system artifact is present locally, but its version is
  still the private placeholder `0.0.0` and the cross-repository distribution
  policy remains open. An exact hash prevents silent drift without inventing a
  second token source.
- Consequence: Web builds fail their token check when the canonical artifact and
  snapshot differ. Publishing remains blocked by O-002 and O-009.

### BM-015 — Add minimal proposal tokens for the first Web vertical slice

- Status: Accepted for implementation as proposals
- Date: 2026-11-10
- Decision: Preserve all approved Button/Input/Card measurements and add only
  the missing implementation values: 16/12px button inline padding, 12/8px
  input inline padding, 16/12px card region gaps, 8px error padding, 8%/12%
  hover/press color mixes, and an 800ms loading-spinner cycle. Add independent
  Light/Dark content colors for solid error surfaces and 1/1.4 line-height
  tokens for single-line controls and compact validation text.
- Rationale: Production component CSS must consume canonical tokens rather than
  hardcoded values, while the approved baseline intentionally leaves these
  measurements unspecified.
- Consequence: These values remain `proposal` and may be calibrated without
  changing approved heights, radii, border rules, density independence, or
  semantic behavior.

### BM-016 — Keep Checkbox native-semantic and defer Radio grouping

- Status: Accepted for Web Core Batch 2B
- Date: 2026-11-10
- Decision: Implement Checkbox as a form-associated Lit wrapper around an
  internal native checkbox. Preserve the approved 4px radius and add proposal
  tokens for a 20px control, 6×10px check mark, 10px indeterminate mark, 2px
  stroke, 8px label gap, and standard 120ms state motion. Implement Radio only
  after its cross-shadow-root group and arrow-key contract is reviewed.
- Rationale: Native checkbox behavior provides reliable Space, focus, required,
  disabled, mixed-state, and form semantics. Native radios in separate shadow
  roots do not group correctly by name alone, so treating Radio as an identical
  follow-on would hide an accessibility defect.
- Consequence: Checkbox is the only component in Batch 2B. It exposes invalid
  state programmatically and visually but does not borrow Input's expanded
  error-panel anatomy, which the approved specification does not assign to it.

### BM-017 — Model Native local overrides as explicit theme scopes

- Status: Accepted for the initial Native foundation
- Date: 2026-11-10
- Decision: Use the `Bm`/`bm_` namespace and a `BmThemeScope` GObject whose
  Light/Dark, Balanced/Compact, and accent dimensions inherit through an
  explicit parent scope. Bind each scope to a GTK subtree root and update its
  GTK classes at runtime. Load the canonical generated GTK CSS as a resource;
  do not load token JSON. Generate custom accents from an sRGB seed using the
  canonical fixed OKLCH constants and contrast pairs, then install
  scope-qualified literal GTK CSS.
- Rationale: GTK CSS has inherited properties but does not reproduce Web custom
  property cascading. Explicit scope objects make local overrides observable
  and testable without introducing a shared runtime or walking arbitrary widget
  internals.
- Consequence: Consumers explicitly create and bind nested scopes. The minimum
  APIs currently used are GTK 4.0 and libadwaita 1.0, but this is not an Ubuntu
  support claim. The API remains provisional until 1.0 and the specification
  still needs a release version.

### BM-018 — Preserve GTK semantics in the first Native component slice

- Status: Accepted for Native Core Batch 2A
- Date: 2026-11-10
- Decision: Implement `BmButton` as a `GtkButton` subclass, `BmInput` as a
  composite around a native `GtkEntry`, and `BmCard` as a flat GTK composition
  widget. Components receive an explicit `BmThemeScope`, use native signals and
  focus/keyboard behavior, and expose GTK widget composition points. Generate
  their resolved density measurements from canonical tokens.
- Rationale: The specification requires a shared semantic contract, not Web API
  signatures. Explicit scopes prevent ambiguous nested GTK selector behavior,
  while native controls retain GTK/AT-SPI and input-method behavior.
- Consequence: Button uses inherited `clicked`; Input exposes `changed`,
  `activate`, validation state, and its native entry; Card owns header, actions,
  child, and footer widgets. Screenshot approval remains blocked until a pinned
  GTK/libadwaita renderer and fonts are available.

### BM-019 — Make Radio Group the form and keyboard owner on Web

- Status: Accepted for Web Core Batch 2C
- Date: 2026-11-10
- Decision: Implement `bm-radio-group` as the form-associated owner of `name`,
  selected `value`, `required`, `disabled`, validity, and arrow-key navigation.
  `bm-radio` owns one native radio control and its label but does not submit an
  independent form value. A standalone radio remains operable, while grouped
  use is required for exclusive selection and form participation. Use roving
  tab stops and logical previous/next navigation across enabled radios.
- Rationale: Native radio inputs in separate open shadow roots are not grouped
  by a shared `name`. Centralizing group state avoids duplicate form values and
  provides one explicit, testable exclusive-selection contract.
- Consequence: Group authors must provide a visible or programmatic group label.
  Proposal geometry adds a 10px selected indicator, 1px border, and 8px label
  gap while preserving the approved 20px outer diameter.

### BM-020 — Use a native checkbox as the Web Switch interaction primitive

- Status: Accepted for Web Core Batch 2C
- Date: 2026-11-10
- Decision: Implement `bm-switch` as a form-associated Lit wrapper around an
  internal native checkbox with `role="switch"`. Preserve the approved 44×24px
  track and 8px radius; use a proposal 18px circular thumb with 3px inset and
  an 8px label gap. Space, focus, disabled, required, and form behavior remain
  native; reduced motion removes thumb animation.
- Rationale: Switch is a binary form control, and the native checkbox supplies
  robust keyboard and form semantics without custom key emulation.
- Consequence: The component exposes switch semantics rather than pretending to
  be a push button. Manual AT coverage remains a Phase 6 release gate.

### BM-021 — Mirror declarative options into a native Web Select

- Status: Accepted for Web Core Batch 2D
- Date: 2026-11-10
- Decision: Implement `bm-select` as a form-associated Lit component containing
  a native single-value `<select>`. Authors declare direct-child `bm-option`
  elements with `value`, `disabled`, and optional initial `selected` state; the
  host mirrors them into native `<option>` elements and remains the owner of
  `name`, `value`, required validity, disabled state, and integrated error text.
  Select reuses the canonical Input geometry and validation tokens.
- Rationale: Slotted `<option>` nodes cannot become actual children of a native
  `<select>` across Shadow DOM. Mirroring a narrow declarative option contract
  preserves native keyboard, focus, popup, form, and accessibility behavior
  without building a custom listbox or runtime JSON renderer.
- Consequence: Batch 2D supports classic, non-multiple selection for short
  lists as specified. Rich option content, filtering, async data, and free-form
  entry belong to the later Combobox component. Option text updates are
  observed; consumers must not place interactive content inside `bm-option`.

### BM-022 — Keep Web Icon Button asset-agnostic and display components narrow

- Status: Accepted for Web Core Batch 2E
- Date: 2026-11-10
- Decision: `bm-icon-button` shares Button variants, destructive treatment,
  unavailable states, square density geometry, activation event, and form type,
  but requires an explicit accessible `label` and accepts icon markup only
  through its default slot. `bm-tag` is a noninteractive accent-subtle label.
  `bm-status` always combines slotted text with the approved 7px dot and offers
  neutral, accent, success, warning, and error tones.
- Rationale: The Lucide source/version and conversion policy remain open, so
  embedding or downloading icons would silently resolve O-008. Narrow display
  APIs preserve the approved visual and semantic contracts without inventing
  dismissible tags or interactive status behavior.
- Consequence: Consumers supply licensed icons and must provide Icon Button's
  accessible label. Tag does not emit removal events. Status tone is never the
  sole carrier of meaning because visible text is required by usage guidance.

### BM-023 — Keep initial Web layout primitives explicit and composition-first

- Status: Accepted for Web Application Shell Batch 3A
- Date: 2026-11-10
- Decision: `bm-page-layout` exposes `fluid` (default), `contained`, and `split`
  variants with header, default content, and aside slots. Add provisional
  canonical tokens of 1200px for contained content and 320px for split aside.
  `bm-section` provides spacing-first header/actions/content/footer composition
  and an optional end separator. `bm-breadcrumbs` composes direct-child
  `bm-breadcrumb-item` links/current page with the approved `/` separator.
- Rationale: The specification approves layout modes and section spacing but
  does not supply content/aside widths. Explicit proposal tokens avoid hidden
  magic numbers while keeping layout independent of theme and density.
- Consequence: Batch 3A does not infer density or layout mode from viewport
  width. Responsive shell policy and breakpoints remain Batch 3C. Breadcrumb
  authors provide a localized navigation label and mark exactly one current
  item; the components do not invent application routing.

### BM-024 — Own selection and relationships in Web Tabs and Segmented Control

- Status: Accepted for Web Application Shell Batch 3B
- Date: 2026-11-11
- Decision: `bm-tabs` owns selected `value`, roving focus, keyboard navigation,
  and explicit host-level `aria-controls`/`aria-labelledby` relationships
  between slotted `bm-tab` and `bm-tab-panel` elements. `bm-segmented-control`
  is a form-associated single-choice radiogroup whose direct `bm-segment`
  children expose radio state. Both controls select the first enabled item when
  no valid initial value is supplied and emit typed change events.
- Rationale: Tab and segment controls live in separate open shadow roots, so
  browser-native grouping cannot be inferred from names or internal IDs.
  Host-level roles and relationships keep the accessible selection model in one
  document scope while preserving component styling and composition.
- Consequence: Tabs are horizontal in Batch 3B and reverse Left/Right movement
  under RTL. Dynamic children and disabled state are synchronized. Overflow,
  vertical tabs, persistence, routing, and responsive shell policy are not
  silently added to this batch.

### BM-025 — Use an explicit responsive Web shell contract

- Status: Accepted for Web Application Shell Batch 3C
- Date: 2026-11-11
- Decision: `bm-app-shell` owns desktop sidebar columns and mobile overlay state,
  while `bm-sidebar` owns navigation composition and `bm-topbar` owns slim
  global-action slots. Preserve approved 240px expanded, 64px collapsed, and
  3px active-indicator measurements. Add a provisional canonical 768px mobile
  breakpoint; below it Sidebar becomes an overlay drawer controlled by
  `sidebarOpen`, with backdrop/Escape dismissal and explicit focus helpers.
- Rationale: CSS custom properties cannot drive media-query conditions, so the
  canonical breakpoint documents the chosen value while component CSS repeats
  its literal with a source comment. Viewport changes layout mode only and do
  not infer or mutate density.
- Consequence: Consumers own routing and connect a labeled menu trigger to
  `openSidebar(trigger)`. Topbar exposes search, notifications, account, start,
  and default action slots without inventing application actions. The mobile
  breakpoint remains subject to design/device validation.

### BM-026 — Keep Web Tooltip descriptive and Dropdown non-modal

- Status: Accepted for Web Complex Interactions Batch 4A
- Date: 2026-11-11
- Decision: Tooltip accepts exactly one trigger, uses the approved 400ms hover
  delay, opens immediately on focus, ignores touch hover, and auto-places using
  available viewport space. Its content is noninteractive. Dropdown owns a
  native trigger and a non-modal menu with roving focus, typeahead,
  outside/Escape dismissal, and focus restoration. Placement defaults to
  bottom-start and flips vertically when space is insufficient.
- Rationale: The specification fixes visual geometry and essential keyboard
  access but does not define a browser API, collision algorithm, touch
  behavior, or menu-trigger ownership. A component-owned native Dropdown
  trigger provides reliable `aria-haspopup`/`aria-expanded` semantics across
  shadow roots. Noninteractive Tooltip content avoids creating inaccessible
  hover-only controls.
- Consequence: Tooltip uses `aria-description` as the cross-shadow accessible
  description fallback and does not overwrite a consumer-provided value.
  Dropdown is not a dialog: Tab exits and closes it without a focus trap.
  Tooltip spacing, Dropdown elevation/radius/insets, overlay stack level, and a
  500ms typeahead reset are restrained implementation proposals requiring
  later design and AT validation.

### BM-027 — Use native modal dialogs and explicit Drawer modes on Web

- Status: Accepted for Web Complex Interactions Batch 4B
- Date: 2026-11-11
- Decision: Implement Dialog and Overlay Drawer with the browser's native
  `<dialog>.showModal()` behavior. This supplies top-layer rendering, an inert
  outside document, modal focus containment, and native Escape semantics.
  Push Drawer is an explicit non-modal grid composition. Small viewports force
  Overlay using the same provisional 768px boundary as App Shell. Both restore
  focus to an explicit trigger and expose cancellable request-close events.
- Rationale: Reimplementing modal containment and inert behavior across open
  shadow boundaries would duplicate fragile browser behavior. Drawer cannot be
  universally modal because the approved Push master-detail mode must keep its
  content region operable. Explicit modes make that platform difference
  testable.
- Consequence: Consumers provide localized headings and close buttons; no
  built-in English string is added. Nested native modals share a reference-
  counted document scroll lock. The 560px dialog maximum, 360px drawer width,
  density padding, content/action gaps, drawer side default, backdrop dismissal,
  and action-marker convention are restrained proposals requiring design and
  manual AT validation. Native `<dialog>` browser support becomes part of the
  unresolved browser-support policy O-006.

### BM-028 — Keep Toast queues local and validation native-owned

- Status: Accepted for Web Complex Interactions Batch 4C
- Date: 2026-11-11
- Decision: `bm-toast-region` owns a local FIFO queue and creates Toast elements
  only through a typed JavaScript options object; applications choose where a
  region exists. Toast auto-dismiss pauses for pointer hover, keyboard focus,
  and document visibility. `bm-validation-summary` attaches to one form and
  derives errors from the platform validity API and form-associated custom
  elements rather than accepting a parallel validation schema.
- Rationale: A global notification singleton creates hidden application state,
  while a JSON-driven notification/form renderer would violate repository
  architecture. Native validity remains interoperable with browser submission,
  current component error UI, and future framework consumers.
- Consequence: Default Toast placement is logical bottom-end, maximum visible
  count is three, and default timeout is 5 seconds. `closeLabel`, summary
  heading, and fallback field label are localizable properties. These timing,
  queue, placement, announcement-role, summary-focus, and spacing decisions are
  restrained proposals requiring product, localization, and manual AT review.

### BM-029 — Keep Combobox editable, declarative, and token-compatible

- Status: Accepted for Web Complex Interactions Batch 4D-A
- Date: 2026-11-11
- Decision: Implement `bm-combobox` as a form-associated editable field backed
  by declarative `bm-option` children. Reuse Input geometry and Dropdown/Menu
  popup tokens. Filtering is label-based and local; consumers may disable it
  and replace options asynchronously. IME composition defers filtering events
  and suppresses selection keys.
- Rationale: Select remains the native short-list control. A searchable field
  needs combobox/listbox relationships and editable text without inventing a
  runtime JSON data source or another visual token system.
- Consequence: Option labels are plain text, selection values are strings, and
  async loading/remote ranking stay application-owned. Popup collision,
  virtualization, mobile keyboard behavior, translated matching, and manual AT
  behavior require later hardening.

### BM-030 — Make Command Palette ownership and shortcut opt-in

- Status: Accepted for Web Complex Interactions Batch 4D-B
- Date: 2026-11-11
- Decision: Build Command Palette from declarative command/group elements and
  a native modal dialog. Applications explicitly opt into Ctrl/⌘+K through the
  `shortcut` property. Selection emits a typed event; the component does not
  route, execute JSON actions, or maintain a global command registry.
- Rationale: Centralized search requires modal focus and grouped keyboard
  navigation, but application routing and authorization cannot safely be
  inferred by the design system. Opt-in shortcut ownership avoids conflicts
  when multiple application roots coexist.
- Consequence: Filtering is plain label/keyword matching. Ranking, remote
  search, command history, routing, and authorization remain consumer-owned.

### BM-031 — Keep Pagination presentation-only and application-controlled

- Status: Accepted for Web Data & Visualization Batch 5A-A
- Date: 2026-11-11
- Decision: `bm-pagination` derives and presents a bounded visible page range
  from `page`, `pageSize`, and `totalItems`, and emits explicit page changes.
  The application owns data loading and may compose its own page-size selector
  through a slot. The default page size is the approved canonical value 25.
- Rationale: Pagination must work for local and remote datasets without
  embedding a data transport, runtime schema, or second data-rendering system.
- Consequence: Built-in labels are localizable properties. Locale-aware number
  formatting, supported page-size choices, RTL/manual AT verification, and
  remote loading announcements remain hardening work.

### BM-032 — Enhance native tables instead of rendering record schemas

- Status: Accepted for Web Data & Visualization Batch 5A-B
- Date: 2026-11-11
- Decision: `bm-table` wraps a consumer-authored native `<table>` in light DOM
  and coordinates sorting requests, selection, states, and optional row
  activation. It does not accept columns/rows as arrays and never sorts,
  fetches, paginates, or renders records itself.
- Rationale: Native table relationships are stronger and more interoperable
  than recreating them with ARIA grid elements, while declarative HTML avoids
  a runtime UI schema. Keeping independent cell controls native also preserves
  their expected keyboard behavior.
- Consequence: Table descendant styling is scoped in the exported
  `styles.css`, so consumers of Table must load that documented stylesheet.
  Consumers own captions, header scopes, responsive column decisions, record
  updates, and routing. Virtualization remains outside this batch.

### BM-033 — Keep Avatar fallback deterministic without a generated color scale

- Status: Accepted for Web Data & Visualization Batch 5B-A
- Date: 2026-11-11
- Decision: Avatar fallbacks derive one or two initials from whitespace-
  delimited name segments and use the current semantic accent surface. Avatar
  Group receives an explicit overflow count and overlaps members by the
  restrained default 8px.
- Rationale: The specification requires deterministic fallback but does not
  approve name-derived colors, hashing rules, overlap geometry, or automatic
  member truncation. Initials are deterministic without creating a second
  palette or hidden data model.
- Consequence: One-segment names produce one initial. Non-Latin/bidi naming,
  overlap approval, and locale-specific initials need product/localization
  review. Availability is supplemental and requires a localized
  `availabilityLabel` for assistive technology.

### BM-034 — Keep Empty State compositional and non-announcing by default

- Status: Accepted for Web Data & Visualization Batch 5B-B
- Date: 2026-11-11
- Decision: Empty State exposes a named region, configurable heading level,
  optional description, and consumer-provided icon/action slots. It supplies no
  default illustration and does not create a live region. Use a restrained
  centered 560px maximum with 32×24px padding until exact geometry is approved.
- Rationale: Empty State communicates absence but does not know whether it was
  present initially or resulted from a dynamic operation. Automatic live
  announcements could be redundant or disruptive, and action execution remains
  application behavior.
- Consequence: Applications own announcement timing and icon/action content.
  Maximum width and padding remain provisional values for design review rather
  than additions to the canonical token system.

### BM-035 — Keep Skeleton geometry content-owned and loading explicit

- Status: Accepted for Web Data & Visualization Batch 5B-C
- Date: 2026-11-11
- Decision: `bm-skeleton` exposes text, rectangle, and circle shapes with
  restrained defaults and CSS custom properties for content-specific width and
  height. Its visual is decorative. One optional localized label supplies a
  loading status, while the application marks the actual content region
  `aria-busy` and owns the transition to loaded content.
- Rationale: A skeleton cannot infer the content it replaces or the boundary
  whose loading state changed. Content-owned geometry avoids a size taxonomy
  not approved by the specification, and a single explicit status avoids
  announcing every decorative placeholder in a composed layout.
- Consequence: Applications must remove `aria-busy`, the labelled status, and
  placeholders together. The 1.5s canonical shimmer becomes static under
  reduced motion. Default 40px circle and 64px rectangle geometry remains a
  restrained implementation default rather than a new canonical token.

### BM-036 — Separate exact progress and KPI meaning from visual approximation

- Status: Accepted for Web Data & Visualization Batch 5C
- Date: 2026-11-11
- Decision: `bm-progress` exposes an exact clamped value through progressbar
  semantics and localized text while rounding only its visual segment count.
  `bm-kpi` accepts already-formatted metric/change text plus an explicit
  favorable, unfavorable, or neutral assessment. Its numeric sparkline property
  renders a decorative point-free line.
- Rationale: Segment counts cannot represent every exact value, and direction
  alone cannot determine business meaning: rising revenue and falling failures
  may both be favorable. Keeping meaning textual prevents color, geometry, or a
  generated chart from becoming the only semantic carrier.
- Consequence: Applications own number/unit formatting and change assessment.
  Sparkline data is property-only and does not introduce an attribute JSON
  renderer. Rich chart interaction and accessible tabular alternatives remain
  in the later Charts batch.

### BM-037 — Keep chart data property-only and pair interaction with text

- Status: Accepted for Web Data & Visualization Batch 5D
- Date: 2026-11-11
- Decision: `bm-chart` accepts property-only category and series arrays, renders
  an SVG line chart, and provides one keyboard target per category. Its owned
  tooltip presents every series label and formatted value. Applications supply
  a concise summary and a durable table through the `table` slot.
- Rationale: Category-level keyboard navigation scales better than a tab stop
  per plotted point, while the table supports comparison, copying, and complete
  access without treating SVG geometry or color as the data model. Property-only
  arrays avoid an attribute JSON UI schema.
- Consequence: Applications own sampling, sorting, number/date localization,
  table content, and live-update announcements. The initial chart supports line
  series only; area fills, axes, zooming, brushing, and automatic aggregation
  remain outside this v1 slice.

### BM-038 — Preserve GTK selection primitives in Native form controls

- Status: Accepted for Native Core Batches 2B–2C
- Date: 2026-11-12
- Decision: Implement Native Checkbox and Radio as `GtkCheckButton`
  subclasses, own radio grouping/value/required validation in `BmRadioGroup`,
  and compose Switch from a labelled native `GtkSwitch`. Keep GTK activation,
  focus, exclusive grouping, accessibility roles, and platform animation.
- Rationale: GTK already provides the reliable keyboard and AT-SPI behavior
  required by the shared semantic contract. Reimplementing these interactions
  in gesture controllers would add a second state machine and weaken platform
  interoperability.
- Consequence: Native APIs use GObject properties and signals rather than Web
  form-associated contracts. Required Checkbox/Radio validation is explicit;
  application-wide validation summaries remain a later Native component.

### BM-039 — Use narrow GTK contracts for Native Select and compact display

- Status: Accepted for Native Core Batches 2D–2E
- Date: 2026-11-12
- Decision: Build Select on labelled `GtkDropDown` text/value options, Icon
  Button on `GtkButton` with consumer-owned icon content, and keep Tag/Status
  static and noninteractive. Status never creates a live region automatically.
- Rationale: Native popup/focus/keyboard behavior belongs to GTK, while rich
  filtering belongs to Combobox. Consumer-owned icons avoid resolving the open
  icon source/license decision in component code. Static display components
  must not invent selection, dismissal, or announcement behavior.
- Consequence: Initial Native Select does not support rich or individually
  disabled options; this limitation is explicit rather than emulated with a
  second popup state machine. Applications own status announcements.

### BM-040 — Compose Native layout and selection from GTK containers

- Status: Accepted for Native Application Shell Batches 3A–3B
- Date: 2026-11-12
- Decision: Implement Page Layout with `AdwClamp`/GTK boxes, Section as open
  widget composition, Breadcrumb ancestors as `GtkLinkButton`, Tabs with
  `GtkStackSwitcher`/`GtkStack`, and Segmented Control with grouped native
  toggle buttons.
- Rationale: GTK already owns link activation, stack focus/selection, and
  exclusive button grouping. Native composition preserves these behaviors and
  avoids importing Web slot, routing, or roving-tabindex machinery.
- Consequence: Routing remains application-owned. Initial Native Tabs has no
  disabled-page or vertical mode; these gaps stay explicit pending platform
  behavior and AT-SPI review.

### BM-041 — Use an explicit Native shell policy around `AdwFlap`

- Status: Accepted for Native Application Shell Batch 3C
- Date: 2026-11-12
- Decision: Compose Native App Shell from Sidebar, Topbar, content, and
  libadwaita `AdwFlap`. Expose desktop collapse, flap visibility, and mobile
  mode as independent GObject state rather than copying the Web viewport
  listener or breakpoint.
- Rationale: GTK/libadwaita owns flap transitions, modal dismissal, focus, and
  platform behavior. The host application already owns window breakpoints and
  can set `mobile` from its chosen adaptive policy without coupling density to
  geometry.
- Consequence: Navigation/routing and breakpoint selection remain
  application-owned. The initial shell restores focus to a supplied trigger,
  but still requires manual AT-SPI, RTL, narrow-window, and rendered review.

### BM-042 — Delegate Native transient hints and menus to GTK

- Status: Accepted for Native Complex Interactions Batch 4A
- Date: 2026-11-12
- Decision: Implement Tooltip with GTK's system tooltip facility and Dropdown
  with `GtkMenuButton`/`GtkPopover`, while retaining BoldMerge value, grouping,
  disabled, destructive, theme-scope, and selection contracts.
- Rationale: GTK already owns desktop tooltip policy, popover placement,
  outside/Escape dismissal, keyboard traversal, and focus restoration. A
  second overlay state machine would weaken platform behavior and duplicate the
  Web implementation rather than share its semantic contract.
- Consequence: Native tooltip delay and placement follow GTK/platform settings,
  not the Web-specific 400ms timer. Dropdown does not yet expose nested menus,
  links, or custom placement; those gaps remain explicit pending AT-SPI, RTL,
  and rendered review.

### BM-043 — Preserve the Native minimum with GtkWindow and AdwFlap

- Status: Accepted for Native Complex Interactions Batch 4B
- Date: 2026-11-12
- Decision: Implement Dialog as a modal, reusable `GtkWindow` and Drawer as an
  `AdwFlap` composition with explicit Overlay/Push, Start/End, and mobile
  state. Both expose cancellable close requests and focus-trigger restoration.
- Rationale: `AdwDialog` would be the modern dialog primitive but requires
  libadwaita 1.5, while the Native package currently declares 1.0. `GtkWindow`
  provides the required native modal/focus behavior without silently changing
  the support floor; `AdwFlap` already underpins the compatible shell policy.
- Consequence: Native backdrop clicks do not close a separate modal window,
  and the host application explicitly supplies the Drawer's mobile policy.
  `AdwFlap` deprecation warnings remain accepted until the minimum version is
  deliberately raised and both shell consumers migrate together.

### BM-044 — Use local AdwToast queues and registered native validity

- Status: Accepted for Native Complex Interactions Batch 4C
- Date: 2026-11-12
- Decision: Wrap `AdwToastOverlay` in a locally placed Toast Region and build
  Validation Summary from explicitly registered BoldMerge Native controls and
  their existing `validate`/`valid` contracts.
- Rationale: libadwaita already owns platform toast priority, timeout, pause,
  swipe, animation, and accessibility behavior. GTK has no HTML-form owner, so
  explicit control registration is more honest than inventing a parallel
  runtime schema or global validation registry.
- Consequence: Native shows one toast at a time and durations use seconds,
  following `AdwToast`; non-API dismissal reason cannot distinguish swipe from
  timeout. Validation Summary currently supports Input, Select, Combobox,
  Checkbox, and Radio Group and requires explicit refresh for message-only
  changes that do not change `valid`.

### BM-045 — Use native searchable selection and app-owned command routing

- Status: Accepted for Native Complex Interactions Batch 4D
- Date: 2026-11-12
- Decision: Implement Combobox with searchable `GtkDropDown` behavior and
  Command Palette as a modal `GtkWindow` composed from `GtkSearchEntry` and
  grouped `GtkListBox` rows.
- Rationale: GTK already provides platform search, selection, focus, and modal
  window semantics. Native APIs should share value/filter/selection meaning
  with Web without copying DOM structure or introducing a cross-platform
  runtime.
- Consequence: Native Combobox search appears in the open popup rather than an
  always-editable closed field. Applications own command routing,
  authorization, side effects, and global accelerators such as Ctrl+K.

### BM-046 — Keep Native Pagination presentation-only

- Status: Accepted for Native Data & Visualization Batch 5A-A
- Date: 2026-11-12
- Decision: Derive a bounded GTK page range from page, page size, total items,
  and sibling count, emitting explicit page requests without loading data.
- Rationale: The same contract must support local and remote GTK models without
  embedding transport, a record schema, or a second data ownership layer.
- Consequence: Applications own loading, localization, and page-size choices;
  Native provides the canonical default size 25 and a composition point for a
  page-size widget.

### BM-047 — Compose Native Table around consumer-owned GTK models

- Status: Accepted for Native Data & Visualization Batch 5A-B
- Date: 2026-11-12
- Decision: Wrap `GtkColumnView` and accept a consumer-owned
  `GtkSelectionModel` plus native cell factories. Header interaction emits sort
  requests but does not reorder or replace the model.
- Rationale: `GtkColumnView` supplies native grid, focus, selection, activation,
  and virtualized list behavior. Accepting record arrays would introduce the
  runtime data schema explicitly rejected for Web Table.
- Consequence: Applications own record types, cell widgets, formatting,
  sorting, routing, responsive columns, and data loading. Native Table exposes
  Ready/Loading/Empty presentation and selection/activation intent only.

### BM-048 — Keep Native identity fallback and empty feedback compositional

- Status: Accepted for Native Data & Visualization Batch 5B
- Date: 2026-11-12
- Decision: Generate Avatar initials deterministically from the first and last
  name segments, and implement Empty State as a named GTK region with optional
  consumer widgets for icon, rich description, and action.
- Rationale: This matches shared identity and absence semantics without a
  generated color scale, hidden avatar collection, bundled illustration, or
  automatic announcement policy.
- Consequence: Avatar images use GDK-loadable resources and fall back on error;
  applications localize availability and overflow labels. Empty State action
  execution and dynamic result announcements remain application-owned.

### BM-049 — Separate Native metric meaning from decorative geometry

- Status: Accepted for Native Data & Visualization Batches 5B-C and 5C
- Date: 2026-11-12
- Decision: Draw Skeleton and KPI sparkline with GTK while keeping loading and
  metric meaning textual; expose exact progress independently of rounded visual
  segments.
- Rationale: Decorative motion, a line trend, and segment count cannot be the
  sole semantic source. GTK system animation settings provide reduced motion.
- Consequence: Applications own busy boundaries, formatting, change meaning,
  and localization; Skeleton uses the approved 1.5-second duration.

### BM-050 — Keep Native chart data programmatic and its alternative durable

- Status: Accepted for Native Data & Visualization Batch 5D
- Date: 2026-11-12
- Decision: Accept chart categories, numeric series, and application-formatted
  values through typed C/GObject APIs. Draw the visual layer with Cairo, expose
  pointer and keyboard category inspection through a native popover tooltip,
  and keep a visible textual summary plus a consumer-owned table composition
  point in the component.
- Rationale: This matches the Web ownership boundary without introducing a
  runtime JSON schema or duplicating application formatting, sorting, sampling,
  and streaming policy inside the library.
- Consequence: Consumers must supply localized formatted values, meaningful
  summary text, and a durable tabular alternative. The initial slice is a
  multi-series line chart; additional chart geometries require separate API and
  accessibility review.

### BM-051 — Pin and self-host fonts; expose a curated Lucide foundation

- Status: Accepted for Foundation Hardening
- Date: 2026-11-15
- Decision: Pin Inter v20 and JetBrains Mono v24 through Fontsource Variable
  5.3.0 for Web, and pin Lucide 1.73.0 for a shared, reviewed icon catalog.
  Web self-hosts normal variable WOFF2 subsets. Native applications and the
  controlled render image install the named fonts, while Native embeds
  generated Lucide SVG resources. Both platforms keep consumer composition
  points for application-specific icons.
- Rationale: Deterministic Web font delivery and identical icon geometry close
  the asset provenance gap without a CDN, runtime JSON renderer, or dependence
  on a host icon theme. A GTK component library must not mutate the process
  font registry, so Native font installation belongs to the application/runtime
  image and is verified as part of the visual environment.
- Consequence: BoldMerge components use weights 400/500 and sizes 16/20/24 with
  1.5px icon strokes. Unsupported scripts and failed font loads use documented
  system fallbacks. New icon names require public API, generated-resource, and
  license review. Exact attributions ship with redistributed assets.

## Open decision register

| ID | Decision needed | Blocking point | Owner/evidence needed |
| --- | --- | --- | --- |
| O-001 | Confirm canonical repository identity/name and Git ownership | Repository setup/publishing | Repository owner |
| O-002 | Assign the baseline specification a SemVer version | First versioned contract | Product/design + engineering |
| O-003 | Approve or revise the implemented provisional Light/Dark primitive and semantic palettes | Stable theme foundation | Design plus component/platform contrast results |
| O-004 | Approve or revise the implemented deterministic OKLCH strategy and supported usages | Stable accent contract | Design/accessibility experiments |
| O-005 | Supply earlier accent-surface references or a measurable replacement opacity target | Accent Surface approval | Design reference review |
| O-006 | Select minimum Node/browser/Ubuntu/GTK/libadwaita/compiler versions | Reproducible CI and support claims | Engineering/support policy |
| O-009 | Choose version-pinned cross-repository artifact distribution | Framework bootstrap | Release engineering |
| O-010 | Define native runtime custom-accent API and propagation semantics | Native theme implementation | GTK prototype |
| O-011 | Approve token naming/versioning/deprecation policy | Stable contract/pre-release | Design-system maintainers |
| O-012 | Define supported localization list and RTL test locales | Framework hardening | Product/localization |

None of the remaining open decisions should be silently resolved by component code.
O-007 and O-008 were resolved by BM-051. M1 may proceed with O-001, O-002,
O-006, O-009, and O-011 visibly open because
it is an unpublished contract prototype; M2 cannot complete while O-003 through
O-005 are open.

