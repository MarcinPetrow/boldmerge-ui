# Chart and Chart Tooltip

Import `@boldmerge/web/chart` to register `bm-chart` and its owned
`bm-chart-tooltip`.

```js
const chart = document.querySelector('bm-chart');
chart.categories = ['Mon', 'Tue', 'Wed'];
chart.series = [
  { label: 'Successful', values: [18, 24, 22] },
  { label: 'Failed', values: [4, 3, 5] },
];
```

`bm-chart` properties are `label`, `summary`, property-only `categories` and
`series`, and `activeIndex`. A series has `label`, numeric `values`, and optional
localized `formattedValues`. Data arrays are never parsed from HTML attributes,
so this is not a runtime JSON UI renderer.

The plot uses approved 1.5px data lines, visible 5px points, subtle 1px grid,
accent-aware series colors, and no area fill. Focus a category target and use
Left/Right, Home, End, and Escape. The tooltip header is the category/date;
rows contain a colored decorative dot, series label, and aligned monospaced
value. Pointer hover uses the same targets.

Supply concise textual `summary` and a complete consumer-authored table in the
`table` slot. Keyboard targets expose every series value for their category,
but the table remains the durable alternative for comparison, copying, and
large or complex datasets. Applications own localization, number/date formats,
sampling, and data updates.

Chart parts are `chart`, `heading`, `label`, `summary`, `plot`, `legend`, and
`alternative`. Tooltip parts are `tooltip`, `heading`, `row`, `dot`,
`series-label`, and `value`.
