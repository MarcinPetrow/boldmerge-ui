# Dropdown

Import `@boldmerge/web/dropdown` to register `<bm-dropdown>`,
`<bm-dropdown-group>`, and `<bm-dropdown-item>`.

```html
<bm-dropdown label="Project actions">
  <span slot="trigger">Project actions</span>
  <bm-dropdown-group label="Edit actions">
    <bm-dropdown-item value="rename">Rename</bm-dropdown-item>
  </bm-dropdown-group>
  <bm-dropdown-group label="Destructive actions" destructive>
    <bm-dropdown-item value="delete">Delete project</bm-dropdown-item>
  </bm-dropdown-group>
</bm-dropdown>
```

Dropdown properties are `label`, `open`, `disabled`, and `placement` (`auto`,
`bottom-start`, `bottom-end`, `top-start`, or `top-end`). It owns a native menu
trigger; the `trigger` slot replaces its visible fallback label. Methods are
`show({ focus, originalEvent })`, `hide({ restoreFocus, originalEvent })`, and
`toggle(originalEvent?)`. Parts are `trigger`, `indicator`, and `menu`.

Group properties are `label` and `destructive`; its part is `group`. Item
properties are `value`, `href`, and `disabled`; slots/parts are `prefix`,
default label, and `suffix`, plus the `item` part.

Arrow Up/Down, Home/End, printable-character typeahead, Enter/Space, Escape,
Tab, and outside-click behavior follow the non-modal menu model. Disabled items
are skipped. Escape and selection restore trigger focus; Tab and outside click
preserve normal focus movement. RTL affects start/end alignment.

`bm-dropdown-open-change` exposes `{ open, originalEvent? }`.
`bm-dropdown-select` exposes `{ value, href, originalEvent }`. Applications own
routing and destructive-action confirmation.
