# Button, Input, and Card visual parity review

Status: implementation and executable smoke review complete; deterministic
rendered baseline review remains pending a pinned capture environment.

## Shared contract retained

| Area | Native result |
| --- | --- |
| Button | Four variants, separate Destructive treatment, loading/disabled states, no click scaling, approved 40/32px heights and 8px radius |
| Input | Hybrid subtle surface, 1px border, 48/40px field, floating label, integrated divided error region, programmatic invalid metadata |
| Card | Flat surface, 8px radius, no border/shadow, 20/12px padding, open header/actions/content/footer composition |
| Context | Explicit Light/Dark, Balanced/Compact, five accents, custom accent, and runtime scope updates |
| Motion | Native GTK state behavior; no custom input/card animation; native spinner only |

All dimensional component selectors are generated from canonical tokens in
`component-metrics.css`. Color selectors reference the context-qualified
canonical GTK artifact. Density is explicit and never viewport-driven.

## Intentional GTK differences

- `BmButton` uses native `clicked`, `GtkActionable`, focus, keyboard, and
  sensitivity semantics instead of a Web custom event or form button type.
- Loading keeps the action label visible beside a native spinner. GTK owns the
  spinner rendering and reduced-motion behavior.
- `BmInput` exposes its `GtkEntry` for native input-purpose/password/IME
  configuration rather than reproducing HTML input `type`, `autocomplete`, or
  form-associated custom-element APIs.
- Floating-label position changes are immediate. This avoids inventing a GTK
  animation and remains correct when animations are disabled.
- Card composition uses owned GTK widget properties rather than named slots.
- GTK font metrics, theme rendering, focus rings, and antialiasing are expected
  to differ from browsers. Visual parity means semantic intent and approved
  dimensions, not cross-platform pixel equality.

## Review gaps

- No PNG baselines have been approved. A controlled renderer must
  capture all four theme/density combinations and relevant states described in
  `tests/visual/README.md`.
- Custom accent contrast is validated for canonical semantic pairs, but actual
  rendered component state contrast still needs platform sampling.
- AT-SPI/Orca validation, high-contrast testing, RTL, touch-target policy, and
  font-controlled rendering remain hardening work. No compliance claim is made.
