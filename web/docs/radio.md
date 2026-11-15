# Radio and Radio Group

Import `@boldmerge/web/radio` to register `<bm-radio>` and
`<bm-radio-group>` without registering unrelated elements.

```html
<bm-radio-group name="plan" label="Plan" value="team" required>
  <bm-radio value="starter">Starter</bm-radio>
  <bm-radio value="team">Team</bm-radio>
  <bm-radio value="enterprise" disabled>Enterprise</bm-radio>
</bm-radio-group>
```

`bm-radio-group` owns `name`, `value`, `label`, `required`, and `disabled`, and
exposes the form methods and state supplied by `ElementInternals`. It emits the
bubbling/composed `bm-radio-group-change` event with `{ value, originalEvent }`.
The group provides one roving tab stop; Arrow keys wrap across enabled options,
and Home/End choose the first/last enabled option.

`bm-radio` owns `value`, `checked`, and `disabled`. Its default slot is the
required visible label. Parts are `label`, `label-text`, `control`, and
`indicator`; it emits `bm-radio-change`. Standalone radios do not submit a form
value—use the group for exclusive form selection.

The group exposes `group` and `label` parts and a default slot. Supply a
meaningful `label`; automated browser coverage does not replace manual AT
testing.
