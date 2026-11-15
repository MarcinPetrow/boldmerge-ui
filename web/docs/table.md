# Table

Import `@boldmerge/web/table` and `@boldmerge/web/styles.css`. The component
keeps a consumer-authored native table in light DOM; it does not accept row
arrays or render a runtime data schema.

```html
<bm-table row-activation>
  <table>
    <caption>Projects</caption>
    <thead><tr>
      <th aria-sort="none"><button data-bm-sort-key="name">Project</button></th>
      <th data-bm-numeric>Spend</th>
    </tr></thead>
    <tbody><tr data-bm-row-key="atlas"><td>Atlas</td><td data-bm-numeric>120</td></tr></tbody>
  </table>
</bm-table>
```

## API and events

Properties are `state` (`ready`, `loading`, or `empty`), `sortKey`,
`sortDirection`, `rowActivation`, `loadingLabel`, and `emptyLabel`.
`selectedKeys` reads checked row selections.

- `bm-table-sort`: `key`, `direction`, and `originalEvent`.
- `bm-table-row-activate`: `key`, native `row`, and `originalEvent`.
- `bm-table-selection-change`: `selectedKeys`, `allSelected`, and
  `originalEvent`.

Use `data-bm-row-key` on rows, `data-bm-sort-key` on native header buttons,
`data-bm-select` on row checkboxes, `data-bm-select-all` on the header checkbox,
and `data-bm-numeric` for end-aligned tabular numbers. Links, buttons, inputs,
selects, textareas, summaries, editable content, and `data-bm-row-action`
elements never activate a row.

Default slot: the native table. Named `loading` and `empty` slots customize
state content. Parts: `viewport`, `state`, `loading`, and `empty`.

## Accessibility

Consumers must provide a native `caption` or table `aria-label` and real
`th` scope relationships. Sorting uses native buttons and updates `aria-sort`.
Rows become Enter/Space operable only with `row-activation`; applications must
also provide a clear detail destination and localized labels. Balanced and
Compact row heights are the approved 48/36px tokens. Manual AT, RTL, forced
colors, virtualization, sticky headers, and responsive column policies remain
hardening work.
