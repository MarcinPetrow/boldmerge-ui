# KPI

Import `@boldmerge/web/kpi` to register only `bm-kpi`.

```js
const kpi = document.querySelector('bm-kpi');
kpi.sparkline = [72, 74, 73, 79, 82, 86, 91];
```

```html
<bm-kpi label="Monthly recurring revenue" value="$128,400"
  change="+8.2%" change-tone="favorable"
  comparison="versus last month"></bm-kpi>
```

Properties are `label`, `value`, `change`, `changeTone`, `comparison`, and the
property-only numeric `sparkline` array. `changeTone` supports `neutral`,
`favorable`, and `unfavorable`. Parts are `kpi`, `copy`, `label`, `value`,
`context`, `change`, `comparison`, and `sparkline`.

The application determines whether a change is favorable: rising revenue and
falling errors may both be favorable. The component never infers meaning from
the sign or sparkline direction. Keep the textual metric, change, and comparison
complete because the 1.5px sparkline is decorative, has no visible points, and
is hidden from accessibility APIs. The array property is numeric series input,
not an HTML attribute or runtime JSON UI schema.
