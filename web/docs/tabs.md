# Tabs

Import `@boldmerge/web/tabs` to register `<bm-tabs>`, `<bm-tab>`, and
`<bm-tab-panel>`.

```html
<bm-tabs label="Project views" value="overview">
  <bm-tab slot="tab" value="overview">Overview</bm-tab>
  <bm-tab slot="tab" value="activity">Activity</bm-tab>
  <bm-tab-panel value="overview">Overview content</bm-tab-panel>
  <bm-tab-panel value="activity">Activity content</bm-tab-panel>
</bm-tabs>
```

Tabs owns `value` and a required localized `label`. Tab exposes `value` and
`disabled`; panels expose matching `value`. The group applies tablist/tab/
tabpanel roles, `aria-controls`, `aria-labelledby`, one roving tab stop, and
`aria-selected`. Left/Right wrap enabled tabs, Home/End select endpoints, and
RTL reverses horizontal arrows. `bm-tab-change` emits `{ value, originalEvent }`.
Parts are `tablist` and `panels`. Tab overflow is horizontally scrollable;
vertical tabs and routing integration are intentionally outside this batch.
