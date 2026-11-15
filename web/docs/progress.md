# Segmented Progress

Import `@boldmerge/web/progress` to register only `bm-progress`.

```html
<bm-progress label="Storage used" value="37" min="0" max="100"
  value-label="37 GB of 100 GB"></bm-progress>
```

Properties are `value`, `min`, `max`, `segments`, `label`, and `valueLabel`.
The default is 20 segments. Segment count is clamped to 1–100 and the exact
value is clamped to the valid range. Parts are `header`, `label`, `value`,
`track`, `segment`, `filled`, and `empty`.

The track exposes native `progressbar` semantics with the exact numeric value.
Filled segments use the nearest whole visual segment, so they are deliberately
only an approximation. Supply a localized `valueLabel` when units or context
matter; it becomes both the visible value and `aria-valuetext`. Do not derive
business decisions from the number of filled segments.

The approved geometry is 7px height, 3px gap, and 2px radius. Segment color
transitions use the foundation interaction duration and become immediate under
reduced motion.
