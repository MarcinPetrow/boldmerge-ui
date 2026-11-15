# Segmented Control

Import `@boldmerge/web/segmented-control` to register
`<bm-segmented-control>` and `<bm-segment>`.

```html
<bm-segmented-control name="view" label="View mode" value="list">
  <bm-segment value="list">List</bm-segment>
  <bm-segment value="board">Board</bm-segment>
</bm-segmented-control>
```

The form-associated group owns `name`, `value`, `label`, and `disabled` and
selects the first enabled segment when needed. Segments expose `value` and
`disabled`. Radiogroup/radio roles, `aria-checked`, roving focus, Home/End,
Left/Right, Enter, and Space are managed explicitly. RTL reverses horizontal
arrows. `bm-segmented-change` emits `{ value, originalEvent }`.

The group exposes `control` part. Segments intentionally represent one mode
choice, not independent toggle buttons.
