# Web accessibility hardening evidence

This record separates automated browser evidence from manual assistive-
technology evidence. Passing DOM, keyboard, forced-colors, or screenshot tests
does not establish screen-reader interoperability or WCAG conformance.

## Automated evidence

| Area | Environment | Result | Evidence |
| --- | --- | --- | --- |
| Keyboard-only component contracts | Linux Chromium, keyboard events | Passed | Playwright component, overlay, modal, shell, table, chart, and hardening suites |
| RTL horizontal navigation | Linux Chromium, `dir=rtl`, Arabic content | Passed | Tabs and Chart reverse physical Left/Right movement; Home/End remain boundaries |
| Localization and long text | Linux Chromium, Polish labels, Arabic content, bracketed pseudo-locale | Passed | Localized Pagination, long Empty State, formatted chart values, overflow assertion |
| Reflow | Linux Chromium, 320 CSS px viewport | Passed for hardening composition | No page-level horizontal scroll; Tabs, Segmented Control, Pagination, and chart table keep overflow local |
| Reduced motion | Chromium emulated `prefers-reduced-motion: reduce` | Passed for covered motion-bearing components | Skeleton animation and Progress transition are static; existing overlay/modal/toast/pagination assertions remain |
| Forced colors | Chromium emulated `forced-colors: active` | Passed for custom visualization primitives | Progress retains filled/empty boundaries, Skeleton retains a border, Chart series use line patterns |
| Visual directionality | Linux Chromium, Light/Balanced | Passed | Reviewed RTL/pseudo-localized baseline |

The automated locale is otherwise pinned to `en-US`, timezone `UTC`, Chromium,
1× device scale, and system fallback fonts. These controls are insufficient for
a release-wide localization or platform claim.

## Manual browser/AT matrix

| Platform | Browser | Assistive technology | Required scenarios | Status |
| --- | --- | --- | --- | --- |
| Windows | Chrome | NVDA | Forms/errors, Combobox, dialogs/drawers, notifications, Table, Charts | Not run — environment unavailable |
| Windows | Edge | JAWS | Forms/errors, overlays, shell navigation, data components | Not run — environment unavailable |
| macOS | Safari | VoiceOver | Rotor landmarks, forms/errors, modal focus, Tables, Charts | Not run — environment unavailable |
| iOS | Safari | VoiceOver | Touch exploration, mobile shell/drawer, forms, overlays | Not run — environment unavailable |
| Android | Chrome | TalkBack | Touch exploration, forms, menus, data alternatives | Not run — environment unavailable |
| Linux | Firefox/Chromium | Orca | Forms/errors, modal focus, announcements, data alternatives | Not run — Orca/display stack unavailable |

For each manual run, record OS/browser/AT versions, input mode, speech or
braille output, focus order, announcement text, defects, retest result, and a
link to the reviewed evidence. Do not change a row to Passed from memory or an
automated accessibility-tree inspection.

## Remaining release blockers

- Execute and review the manual matrix above.
- Expand forced-colors inspection beyond visualization primitives.
- Add Firefox and WebKit automated suites and platform-specific baselines.
- Test real translated bundles and locale-aware number/date formatting; expand
  bidi-isolation, zoom/reflow, and long-content coverage throughout the catalog.
- Run a maintained automated accessibility rules engine once its version,
  browser support, exclusions, and triage policy are approved.
