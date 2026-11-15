# BoldMerge Web v1.0 implementation batches

This roadmap follows the six approved phases in `BOLDMERGE_ADAPTIVE_SPEC.md`.
Each batch is independently reviewable and must include API documentation,
selective exports, examples, behavior/keyboard/integration tests, and
platform-specific visual baselines before it is marked complete.

## Current status

| Phase | Batch | Scope | Status |
| --- | --- | --- | --- |
| 1. Foundation | 1A | Canonical token consumption, themes, density, accent, typography, focus, motion | Complete |
| 2. Core | 2A | Button, Input, Card | Complete |
| 2. Core | 2B | Checkbox | Complete |
| 2. Core | 2C | Radio plus required grouping semantics; Switch | Complete |
| 2. Core | 2D | Select | Complete |
| 2. Core | 2E | Icon Button; Tag; Status | Complete |
| 3. Application Shell | 3A | Page layouts, Sections, Breadcrumbs | Complete |
| 3. Application Shell | 3B | Tabs and segmented controls | Complete |
| 3. Application Shell | 3C | Sidebar, Topbar, responsive shell | Complete |
| 4. Complex Interactions | 4A | Tooltip and Dropdown primitives | Complete |
| 4. Complex Interactions | 4B | Dialog and Drawer | Complete |
| 4. Complex Interactions | 4C | Toast and integrated validation hardening | Complete |
| 4. Complex Interactions | 4D-A | Combobox | Complete |
| 4. Complex Interactions | 4D-B | Command Palette | Complete |
| 5. Data & Visualization | 5A-A | Pagination | Complete |
| 5. Data & Visualization | 5A-B | Table | Complete |
| 5. Data & Visualization | 5B-A | Avatar and Avatar Group | Complete |
| 5. Data & Visualization | 5B-B | Empty State | Complete |
| 5. Data & Visualization | 5B-C | Skeleton | Complete |
| 5. Data & Visualization | 5C | Segmented Progress and KPI | Complete |
| 5. Data & Visualization | 5D | Charts and chart tooltip | Complete |
| 6. Hardening | 6A | Accessibility/AT testing, RTL/localization, reduced motion | In progress |
| 6. Hardening | 6B | Visual baseline matrix, docs/examples, API stability, integration app | Complete |

Command Palette remains Phase 4 despite its shell entry point because it
depends on overlay, focus-management, and grouped keyboard-navigation
primitives. Radio grouping is deliberately designed before Radio implementation
because native radios in separate shadow roots do not form a browser-native
group merely by sharing a `name`.

## Batch 2B contract: Checkbox

- Public element: `<bm-checkbox>`; selective import `@boldmerge/web/checkbox`.
- Properties/attributes: `checked`, `indeterminate`, `disabled`, `required`,
  `name`, and `value` (`"on"` by default).
- State: unchecked, checked, indeterminate, disabled, focused, and invalid.
- Slot: default label content. Accessible use requires a non-empty label.
- Parts: `control`, `indicator`, and `label`.
- Event: bubbling/composed `bm-checkbox-change` with typed checked,
  indeterminate, value, and original-event detail.
- Forms: `ElementInternals`, form value/reset/disabled/restore callbacks,
  `checkValidity`, `reportValidity`, and `setCustomValidity`.
- Keyboard/accessibility: an internal native checkbox supplies Space activation,
  focus order, required validity, disabled behavior, and mixed-state exposure.
  The host delegates `focus()` to it. No color-only state or custom keyboard
  emulation is introduced.
- Styling: canonical tokens only; approved 4px radius; no viewport-driven
  density. Coarse-pointer target is at least 44px.

Checkbox does not render an Input-style integrated error panel because the
approved specification assigns that anatomy to Input only. Invalid state is
programmatically exposed and receives error border treatment; applications may
associate group-level explanatory text in the subsequent form-pattern work.

## Batch 2B gap report

Delivered: typed/form-associated API, open Shadow DOM, selective export,
unchecked/checked/mixed/disabled/invalid/focus states, native Space and label
activation, Light/Balanced and Dark/Compact baselines, documentation, and Vite
example.

Remaining by design:

- Radio is blocked on a reviewed cross-shadow-root grouping and roving/arrow-key
  contract; it is Batch 2C, not partially scaffolded here.
- Checkbox group-level explanatory errors are deferred to the shared form
  pattern. The component exposes validity, `aria-invalid`, and its validation
  message without inventing Input's expanded-error anatomy.
- Automated tests cover browser semantics and accessible state, but manual
  screen-reader/AT matrix testing remains Phase 6. No standalone WCAG compliance
  claim is made.
- Canonical and Web package versions remain the private `0.0.0` placeholder;
  publishing remains blocked.

Batch 2C followed this gap with an explicit group-owned contract because native
radio inputs placed in separate shadow roots do not group by `name` alone.

## Batch 2C contract: Radio Group, Radio, and Switch

- `bm-radio-group` owns exclusive selection, form value, required validity,
  disabled state, group label, and roving Arrow/Home/End keyboard navigation.
- `bm-radio` owns one native radio, its label, `value`, `checked`, and disabled
  state. Grouped use is required for form submission.
- `bm-switch` is a form-associated native checkbox exposed as a switch, with
  checked, disabled, required, name, and value state.
- Selective entries are `@boldmerge/web/radio` and `@boldmerge/web/switch`;
  neither registers unrelated components.
- Both controls consume canonical tokens, inherit local theme/density/accent,
  preserve native focus/Space behavior, and disable transitions for reduced
  motion.

## Batch 2C gap report

Automated tests cover exclusivity, form values, Space and arrow navigation,
disabled/required behavior, selective registration, and the Light/Balanced and
Dark/Compact visual matrix. Manual screen-reader/AT verification, RTL review,
and approved cross-browser baselines remain Phase 6 release gates. The geometry
values marked `proposal` in the canonical source still require design review.

Batch 2D followed with the native Select contract described below.

## Batch 2D contract: Select

- `bm-select` owns single-value form state, label, placeholder, native required
  validity, disabled state, and integrated error association.
- Direct-child `bm-option` elements supply plain-text native option labels and
  expose value, disabled, and initial selected state.
- `@boldmerge/web/select` registers only Select and Option.
- A native `<select>` retains popup, keyboard, focus, and platform semantics;
  filtering and rich options remain in the later Combobox batch.
- Input geometry and validation tokens are reused rather than creating a
  divergent form-field system.

## Batch 2D gap report

Automated tests cover native keyboard selection, form value/reset, required and
custom errors, disabled options/control, runtime option mirroring, selective
registration, and representative Light/Balanced and Dark/Compact visuals.
Manual AT, mobile popup, Firefox/WebKit, localization/RTL, and forced-colors
coverage remain Phase 6 gates.

Batch 2E followed with the narrow action/display contracts below.

## Batch 2E contract: Icon Button, Tag, and Status

- Icon Button shares Button variants, treatment, loading/disabled states,
  activation, form type, density, and focus behavior. It is square, requires an
  accessible `label`, and receives consumer-owned icon markup through its slot.
- Tag is a static accent-subtle label with optional decorative prefix/suffix.
- Status combines required visible text with the approved 7px dot and neutral,
  accent, success, warning, or error tone. It is not a live region by default.
- Selective entries register exactly their corresponding element.

## Batch 2E gap report

Automated tests cover Icon Button keyboard activation and unavailable states,
selective registration, visible Tag/Status text, decorative-dot semantics, and
representative paired theme/density visuals. The library intentionally ships no
icon assets while O-008 remains open. Manual AT, forced-colors, localization,
RTL, cross-browser, and complete state matrices remain Phase 6 gates.

Batch 3A followed with the explicit composition contracts below.

## Batch 3A contract: Page Layout, Section, and Breadcrumbs

- Page Layout exposes fluid, contained, and split composition without imposing
  landmarks, routing, page padding, or viewport-driven density.
- Section provides density-aware 32/24px spacing, open header/actions/content/
  footer composition, optional accessible label, and optional end separator.
- Breadcrumbs uses a labeled native navigation landmark, native ancestor links,
  `aria-current="page"`, visible current text, and decorative `/` separators.
- Selective entries register exactly their corresponding element family.

## Batch 3A gap report

Automated tests cover slot visibility, layout variants and canonical widths,
section labels/density spacing, link focus, current-page semantics, selective
registration, and paired theme/density screenshots. The 1200px contained width
and 320px split aside remain proposal tokens. Responsive shell breakpoints,
Sidebar, and Topbar remain Batch 3C; manual AT/RTL/localization and browser
matrices remain Phase 6.

Batch 3B followed with the exclusive-selection contracts below.

## Batch 3B contract: Tabs and Segmented Control

- Tabs owns selection, tab/panel relationships, horizontal roving focus,
  Home/End, disabled items, RTL-aware arrows, and typed change events.
- Segmented Control is a form-associated single-choice radiogroup with the same
  roving/arrow model plus Enter/Space activation.
- Both choose the first enabled value when no valid initial value exists and
  expose separate selective entry points.

## Batch 3B gap report

Automated tests cover relationships, hidden panels, exclusive selection,
keyboard behavior, form values, disabled options, selective registration, and
Light/Balanced plus Dark/Compact screenshots. Vertical tabs, persistence,
routing, responsive overflow policy, manual AT, RTL browser coverage, and
cross-browser matrices remain open.

## Batch 3C contract: Sidebar, Topbar, and App Shell

- Sidebar owns desktop expanded/collapsed presentation and a mobile overlay,
  while Sidebar Item owns link, active, disabled, icon, and accessible-label
  semantics. Routing remains application-owned.
- Topbar exposes start, content, search, notification, and account composition
  without imposing application actions or responsive hiding.
- App Shell composes named sidebar/topbar regions with default content,
  propagates shell state explicitly, and restores the mobile menu trigger focus
  after dismissal.
- The approved 240px/64px sidebar geometry and 3px indicator are canonical.
  The 768px overlay breakpoint is a restrained proposal recorded in BM-025.
- Viewport width changes shell layout only; density remains an independent
  theme dimension.

## Batch 3C gap report

Automated tests cover desktop geometry, active navigation semantics, mobile
inert/hidden state, open focus, Escape dismissal, focus restoration, selective
registration, and representative Light/Balanced, Dark/Compact, and mobile
visual baselines. Focus trapping within the mobile drawer is intentionally not
claimed: it is a navigation region rather than a modal dialog, and the main
content is not made inert while the drawer is open. The closed drawer itself is
inert and hidden from the accessibility tree. Consumer routing, localization,
long labels, RTL, forced colors,
manual AT, and cross-browser matrices remain Phase 6 work. The provisional
breakpoint requires design approval.

## Batch 4A contract: Tooltip and Dropdown

- `bm-tooltip` owns one default slotted trigger and noninteractive tooltip
  content from the `content` property or `content` slot. It opens after the
  approved 400ms mouse/pen hover delay, immediately for keyboard focus, closes
  on blur/pointer leave/Escape, and supports auto/top/bottom/left/right
  placement. Touch taps do not create sticky tooltip state.
- Tooltip mirrors its plain-text content to the trigger's `aria-description`
  only when the consumer has not supplied an accessible description. It
  restores the original trigger attribute when replaced or disconnected.
- `bm-dropdown` owns its native menu trigger, `open` state, auto or explicit
  top/bottom start/end placement, outside-click/Escape dismissal, focus
  restoration, Arrow/Home/End navigation, and printable-character typeahead.
- `bm-dropdown-group` provides optional group labeling and the approved subtle
  separator before destructive groups. `bm-dropdown-item` owns link/button,
  disabled, value, prefix/default/suffix composition, and menuitem semantics.
- Typed composed events are `bm-tooltip-open-change`,
  `bm-dropdown-open-change`, and `bm-dropdown-select`. Selective imports
  register only their documented element families.
- Neither primitive is modal. Dropdown closes on Tab without preventing normal
  focus movement; Tooltip content cannot contain interactive controls.

## Batch 4A gap report

Automated tests cover the approved hover delay, immediate focus opening,
auto-placement collision, Escape/blur/pointer dismissal, accessible Tooltip
description, Dropdown trigger semantics, roving arrows/Home/End, disabled-item
skipping, typeahead, selection, Tab/outside dismissal, focus restoration,
reduced-motion behavior, selective registration, and representative
Light/Balanced and Dark/Compact
visual baselines. Placement is viewport-based and does not yet account for
arbitrary clipping ancestors. Touch long-press, zoom/reflow extremes, nested
menus, manual screen-reader/AT, forced colors, RTL browser coverage, and the
full cross-browser matrix remain Phase 6 work. Proposal tokens and
`aria-description` behavior require design/AT review.

## Batch 4B contract: Dialog and Drawer

- `bm-dialog` uses a native modal `<dialog>` in open Shadow DOM. It owns `open`,
  `label`, and `dismissible`; explicit `show({ trigger, initialFocus })` and
  `close(reason, returnValue, originalEvent)` methods; initial focus, native
  modal containment, document scroll locking, Escape/backdrop policy, and
  trigger focus restoration.
- Dialog slots are `header`, default content, `footer`, and `actions`. It uses
  the approved compact 8px geometry, overlay/shadow, bottom-right actions, and
  no default section dividers. Consumers provide localized close controls.
- `bm-drawer` owns `open`, `label`, `dismissible`, `mode` (`overlay` default or
  `push`), and logical `position` (`end` default or `start`). Its `content` slot
  is the application/master region; `header`, default, `footer`, and `actions`
  compose the drawer panel.
- Overlay Drawer uses native modal dialog behavior, focus restoration, backdrop
  and Escape dismissal. Push Drawer remains non-modal and participates in a
  desktop grid. The existing provisional 768px shell breakpoint forces Overlay
  on small viewports without changing density.
- Both components expose cancellable `*-request-close`, typed `*-close`, and
  `*-open-change` events. A slotted element with `data-bm-dialog-close` or
  `data-bm-drawer-close` requests an action close; the attribute value becomes
  `returnValue`.
- Modal document scroll locking is Web-internal infrastructure only. It is not
  a shared Web/Native runtime and no runtime JSON schema is introduced.

## Batch 4B gap report

Automated tests cover native modal opening, initial focus, Escape, backdrop and
action close paths, cancellable close requests, focus restoration, reference-
counted scroll locking across nested modals, non-modal desktop Push behavior,
mobile Push→Overlay switching, reduced motion, selective registration, and
representative Light/Balanced Dialog plus Dark/Compact Drawer baselines. Native
`<dialog>` provides the focus boundary; no custom focus-trap runtime was added.
Manual screen-reader/AT, browser back-button policy, nested same-component
stress tests, iOS virtual keyboard behavior, RTL start/end screenshots, forced
colors, and Firefox/WebKit matrices remain Phase 6 work. Proposal geometry and
the 768px responsive boundary still require design approval.

## Batch 4C contract: Toast and Validation Summary

- `bm-toast-region` is an explicit local notification owner; no global
  singleton is registered. Its typed `push(options)`, `dismiss(id)`, and
  `clear()` API manages at most `maxVisible` toasts and a FIFO queue. Placement
  defaults to logical bottom-end, matching the approved bottom-right intent in
  LTR while remaining RTL-aware.
- `bm-toast` provides neutral surface, semantic icon, title, message, close
  control, and neutral/success/warning/error tone without tinted backgrounds or
  colored strips. The provisional default duration is 5 seconds; zero is
  persistent. Timeout pauses for hover, focus, and hidden documents. Warning/
  error use alert semantics; neutral/success use status semantics.
- Toast exposes `closeLabel` for localization, `duration`, `open`, typed close/
  open-change events, and `dismiss(reason)`. Reduced motion removes entrance
  animation but does not silently alter notification duration.
- `bm-validation-summary` binds to one existing form by `for`. It reads native
  `form.elements`, `validity`, and `validationMessage`; it does not duplicate
  field rules. `validate({ focus })`, `refresh()`, and item activation focus and
  report the actual invalid control.
- Validation Summary observes invalid/input/change/reset updates, emits a typed
  change event, and renders a linked actionable error list in an alert region.
  Existing Input/Select integrated errors remain the field-level source of
  truth; other controls retain their native/custom-element validity contracts.

## Batch 4C gap report

Automated tests cover bounded FIFO queue draining, local ownership, status vs
alert roles, API dismissal, actionable native-form errors, dynamic clearing,
first-invalid focus, reduced-motion animation removal, selective registration,
and representative Light/Balanced plus Dark/Compact baselines. The focused demo
and standalone catalog exercise nested theme/density scope and a real composed
form. Timeout pause/resume is implemented for hover, focus, and document
visibility; manual screen-reader announcement timing, rapid-queue stress,
forced colors, translated long strings, RTL screenshots, and Firefox/WebKit
remain Phase 6 work. The 5-second timeout, maximum of three, placement, and
announcement-role policy remain proposals pending product and AT review.

## Batch 4D-A contract: Combobox

- `bm-combobox` is a form-associated editable field for searchable option
  lists; classic short-list selection remains `bm-select`. Declarative
  `bm-option` children are the build/runtime DOM source, not a JSON schema.
- Public properties are `value`, `inputValue`, `label`, `name`, `placeholder`,
  `required`, `disabled`, `error`, `open`, and `filter`. Read-only form,
  validity, and validation-message access mirrors the existing field family.
- The internal text field owns `role=combobox`, `aria-expanded`,
  `aria-controls`, `aria-autocomplete=list`, and active-option linkage. The
  popup owns listbox/option semantics. Disabled options are never active or
  selected.
- ArrowDown/ArrowUp open and move through enabled visible options; Home/End
  move to boundaries; Enter selects; Escape closes without selection; Tab
  preserves normal focus movement. Printable input filters labels
  case-insensitively when `filter=true`.
- IME composition suppresses navigation/selection and defers the typed input
  event until `compositionend`. Empty/loading application states remain
  consumer-owned through the `empty` slot and explicit option updates.
- Typed composed events are `bm-combobox-input`, `bm-combobox-change`, and
  `bm-combobox-open-change`. Selective import registers only Combobox and the
  shared declarative Option element.
- Styling reuses canonical Input field tokens plus Dropdown/Menu popup tokens;
  no divergent Combobox token family is introduced without approved geometry.

## Batch 4D-A gap report

Automated tests cover declarative filtering, combobox/listbox relationships,
active-option linkage, disabled-option skipping, Arrow/Home/End/Enter/Escape
behavior, form value and required validity, IME event deferral, selective
registration, and representative Light/Balanced and Dark/Compact baselines.
The focused demo and standalone catalog include nested theme/density scope.
Remote ranking, loading announcements, virtualization, popup collision near
clipping ancestors, mobile virtual keyboards, locale-aware matching beyond
case folding, RTL, forced colors, manual AT, and Firefox/WebKit remain hardening
work. Reusing Input and Dropdown/Menu tokens avoids an unapproved geometry fork.

## Batch 4D-B contract: Command Palette

- `bm-command-palette` is one application-owned modal search surface composed
  from declarative `bm-command-group` and `bm-command` children. No global
  command registry or runtime JSON schema is introduced.
- Public properties are `open`, `label`, `placeholder`, `emptyLabel`, and
  opt-in `shortcut`. `show({ trigger, originalEvent })`, `close(reason,
  originalEvent)`, and `focus()` own native-dialog opening, restoration, and
  deterministic search focus.
- Commands expose `value`, `keywords`, and `disabled`; groups expose `label`.
  Filtering uses label plus keywords. ArrowUp/Down and Home/End move through
  enabled visible commands, Enter activates, Escape closes, and Tab remains
  inside the native modal focus boundary.
- Ctrl/⌘+K is registered only when `shortcut=true`; editable targets are not
  intercepted. IME composition suppresses navigation and selection.
- Typed events are `bm-command-palette-select`, `bm-command-palette-close`, and
  `bm-command-palette-open-change`. Selective import registers only the palette,
  group, and command family.
- The modal reuses Dialog geometry and shadow plus Input/Dropdown/Menu tokens;
  no Command Palette token fork is created without approved measurements.

## Batch 4D-B gap report

Automated tests cover grouped filtering, keyword matching, opt-in Ctrl/⌘+K,
native modal search focus, disabled-command skipping, keyboard selection,
Escape closing, trigger restoration, selective registration, and paired
Light/Balanced plus Dark/Compact baselines. Routing and command execution remain
explicit application behavior. Remote search, ranking, history, authorization,
conflicting shortcut owners, translated matching, manual AT, RTL, forced
colors, and Firefox/WebKit remain hardening work.

## Batch 5A-A contract: Pagination

- `bm-pagination` owns page-range presentation and boundary behavior, while the
  application owns records, fetching, filtering, sorting, and page-size UI.
- Public state is `page`, `pageSize` (canonical default 25), `totalItems`,
  `siblingCount`, and `disabled`. The derived `pageCount` is never below one.
- `goToPage()`, `next()`, and `previous()` emit the typed, bubbling and composed
  `bm-page-change` event only when the bounded page actually changes.
- A labelled native navigation region, native buttons, `aria-current="page"`,
  disabled boundaries, and a polite current/total summary provide the keyboard
  and accessibility foundation. Built-in labels are localizable properties.
- The optional `page-size` slot composes an application-owned selector without
  creating a data model or runtime renderer. Selective import registers only
  `bm-pagination`.

## Batch 5A-A gap report

Automated coverage includes range rendering, page count, boundary clamping,
typed events, native Enter/Space activation, disabled boundary controls,
reduced motion, selective registration, and Light/Balanced plus Dark/Compact
visual baselines. Locale-aware number formatting, RTL/manual AT verification,
forced colors, Firefox/WebKit, responsive long-label behavior, and a product
decision on page-size choices remain Phase 6 hardening work.

## Batch 5A-B contract: Table

- `bm-table` enhances one consumer-authored native `<table>`; it never accepts
  row/column arrays and never renders a runtime data schema.
- `state` selects ready/loading/empty presentation. `sortKey` and
  `sortDirection` expose application-controlled sorting; the component changes
  `aria-sort` and emits `bm-table-sort` but never reorders records.
- `rowActivation` makes keyed body rows pointer and Enter/Space operable.
  Native links, buttons, fields, editable content, and explicit row actions do
  not activate the row. `bm-table-row-activate` identifies the requested row.
- Native row/select-all checkboxes are coordinated through declarative data
  attributes and `bm-table-selection-change`; disabled selections are excluded.
- Approved horizontal-only dividers, no zebra/vertical rules, numeric end
  alignment, and 48/36px density row heights live in the exported document
  stylesheet because the real table intentionally remains in light DOM.

## Batch 5A-B gap report

Automated coverage includes native table/caption semantics, keyboard sorting,
`aria-sort`, selection and indeterminate select-all behavior, disabled rows,
pointer and keyboard row activation, independent controls, exact density row
heights, selective registration, and paired visual baselines. Consumers remain
responsible for captions/header scopes, record updates, pagination integration,
responsive column policy, and detail routing. Virtualization, sticky headers,
RTL/manual AT, forced colors, Firefox/WebKit, large-data performance, and
responsive narrow-screen/long-content matrices remain hardening work.

## Batch 5B-A contract: Avatar

- `bm-avatar` exposes person/organization kinds and the approved
  24/32/40/48px sizes. People are circular; organizations use the approved 8px
  radius. Geometry is independent of density and viewport.
- Fallback initials deterministically use the first character of the first and
  last whitespace-delimited name segments. Semantic accent tokens provide the
  surface; no name-derived color palette is introduced.
- Images are decorative inside the labelled Avatar. Load failures retain the
  fallback and emit `bm-avatar-image-error`. Availability dots are supplemental
  and enter the accessible name only through explicit `availabilityLabel`.
- `bm-avatar-group` composes slotted avatars plus an application-owned explicit
  overflow count. It never hides or derives members from a data model.

## Batch 5B-A gap report

Automated coverage includes deterministic initials, accessible labels,
availability labeling, exact sizes, person/organization geometry, density and
viewport independence, group/overflow semantics, selective registration, and
paired visual baselines. Real image loading/CDN/CORS behavior, broken-image
visual baselines, non-Latin naming conventions, bidi names, manual AT, forced
colors, and Firefox/WebKit remain hardening work. Availability must be paired
with visible text when it carries business-critical meaning.

## Batch 5B-B contract: Empty State

- `bm-empty-state` is a named region with plain-text `heading` and optional
  `description`; `headingLevel` provides explicit 1–6 heading semantics.
- Optional `icon`, `description`, and `action` slots preserve consumer-owned
  content and action behavior. Empty wrappers leave layout when unused.
- No icon or illustration is generated by default, and the component is not a
  live region. Applications explicitly own dynamic-result announcements.
- Styling uses canonical typography, color, radius, and spacing tokens. The
  centered maximum width and 32×24px padding are restrained provisional
  defaults, not new canonical tokens.

## Batch 5B-B gap report

Automated coverage includes region naming, heading-level clamping, optional
composition regions, absence of a default illustration, native keyboard action
behavior, selective registration, and Light/Balanced plus Dark/Compact visual
baselines. Long localized content, rich-description links, RTL, manual AT,
forced colors, Firefox/WebKit, and announcement integration remain hardening
work.

## Batch 5B-C contract: Skeleton

- `bm-skeleton` exposes `text`, `rectangle`, and `circle` shapes. Consumer CSS
  may set `--bm-skeleton-width` and `--bm-skeleton-height` to match eventual
  content; density and viewport do not infer content geometry.
- The visual placeholder is decorative. One optional localized `label` exposes
  a status for a loading group, while applications mark the content boundary
  `aria-busy` and own the transition to loaded content.
- Shimmer uses the approved 1.5s component token and becomes a static neutral
  gradient under reduced motion. An explicit `data-bm-motion="full"` scope
  retains the foundation motion override.
- The selective entry registers only `bm-skeleton`; no runtime data schema,
  renderer, or shared Web/Native runtime is introduced.

## Batch 5B-C gap report

Automated coverage includes all shapes, explicit loading status, the busy-region
composition, 1.5s shimmer timing, static reduced motion, selective registration,
nested theme/density scope, and Light/Balanced plus Dark/Compact baselines.
Manual AT, forced colors, RTL, multi-browser rendering, streamed-content
transitions, and application-specific placeholder geometry remain hardening
work.

## Batch 5C contract: Segmented Progress and KPI

- `bm-progress` exposes exact `min`, `max`, and clamped `value` through native
  progressbar semantics. Its configurable 1–100 segment rendering rounds to the
  nearest whole segment and remains only a visual approximation.
- A localized `valueLabel` supplies visible units/context and `aria-valuetext`.
  Geometry uses the approved 20-segment default, 7px height, 3px gap, and 2px
  radius. Reduced motion removes segment color transitions.
- `bm-kpi` receives formatted metric, change, and comparison text. Applications
  explicitly assign `neutral`, `favorable`, or `unfavorable`; direction never
  infers business meaning.
- KPI sparkline data is a property-only numeric array. The approved 1.5px,
  point-free line is decorative and hidden from accessibility APIs because the
  complete metric meaning remains textual.

## Batch 5C gap report

Automated coverage includes progress range clamping, exact accessible values,
rounded segments, custom segment counts, localized value text, explicit KPI
change assessment, decorative sparklines, reduced motion, selective imports,
nested scopes, and paired Light/Balanced plus Dark/Compact baselines. Locale
formatting, live metric announcements, forced colors, RTL, manual AT,
multi-browser rendering, large-series sampling, and interactive chart behavior
remain hardening or Charts-batch work.

## Batch 5D contract: Charts and chart tooltip

- `bm-chart` accepts property-only categories and line-series arrays; no data is
  parsed from HTML JSON attributes. Series may provide localized formatted
  values independently from their numeric geometry.
- SVG rendering uses approved 1.5px lines, visible 5px points, subtle 1px grid,
  accent-aware series colors, and no default area fill.
- One focus target per category supports Left/Right, Home, End, and Escape. The
  owned tooltip presents the category/date plus every series label and aligned
  monospaced value; pointer hover uses the same model.
- Applications supply a concise textual summary and complete table in the
  `table` slot. Plot geometry and series color are never the sole data carrier.

## Batch 5D gap report

Automated coverage includes geometry tokens, absence of area fill, multiple
series, localized formatted values, category keyboard navigation, tooltip
rows, textual summary, table alternative, selective registration, nested
theme/density scope, and paired baselines. Manual AT, forced colors, RTL,
locale/date formatting, live streams, large-series sampling, axes, zooming,
touch exploration, and Firefox/WebKit remain hardening work.

## Batch 6A status

Automated Chromium evidence now covers keyboard operation, RTL navigation and
chart geometry, pseudo-localized long text, localized Pagination strings,
forced colors, reduced motion, bidi isolation, and 320 CSS px reflow. The
manual screen-reader matrix remains unexecuted because the required platform
environments are unavailable, so 6A remains in progress and no WCAG or
screen-reader conformance claim is made.

## Batch 6B status

The API-stability slice adds a deterministic declaration/export baseline and
validates that every selective component entry stays aligned across package
exports, Rollup inputs, side-effect metadata, source entries, built outputs,
full-package registration, and API documentation. A separate Release Operations
consumer now composes shell navigation, metrics, progress, chart/table data,
Pagination, Dialog confirmation, and Toast feedback into a tested approval
workflow with Light/Balanced and Dark/Compact baselines.

Tier A representative fixtures now run the complete Light/Dark ×
Balanced/Compact Cartesian matrix. Focused state, modal/overlay, responsive,
and RTL baselines remain deliberately targeted under the documented baseline
review policy rather than multiplying visually redundant images.

Portable behavior is separated from Chromium/Linux pixel baselines with an
explicit `@visual` tag. Firefox passes 95/95 behavior tests. WebKit passes 94
with one explicit skip because Playwright WebKit cannot emulate forced colors;
Chromium and Firefox retain that evidence. The matrix exposed and closed a
Combobox defect where repeated Firefox `input` events cleared
`aria-activedescendant` while the listbox remained open.

## Batch 6B gap report

The final inventory found 34 selective component entries with 34 matching API
documents and no orphan component documents. The catalog and Release
Operations consumer exercise all 48 public custom elements; the owned
`bm-chart-tooltip` is created through `bm-chart` rather than presented as a
standalone consumer primitive. The 37-entry API baseline, 61 Chromium/Linux
snapshots, complete Tier A theme/density matrix, consumer workflow, and portable
Firefox/WebKit behavior configurations are in place.

Remaining release work is outside the completed 6B implementation slice:
manual AT execution from 6A, controlled fonts and additional platform images,
pinned multi-browser CI/minimum-version policy, stable SemVer/deprecation and
license/provenance decisions. Completion of 6B is not a v1 readiness claim.

Exact next work: **return to 6A and execute the recorded manual AT matrix in
provisioned Windows, macOS/iOS, Android, and Linux Orca environments**.

