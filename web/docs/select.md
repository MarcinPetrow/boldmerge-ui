# Select

Import `@boldmerge/web/select` to register `<bm-select>` and `<bm-option>`
without registering unrelated elements.

```html
<bm-select name="region" label="Region" value="eu" required>
  <bm-option value="us">United States</bm-option>
  <bm-option value="eu">Europe</bm-option>
  <bm-option value="apac" disabled>Asia Pacific</bm-option>
</bm-select>
```

`bm-select` properties/attributes are `value`, `name`, `label`, `placeholder`,
`autocomplete`, `required`, `disabled`, and `error`. It is form-associated and
exposes `form`, validity state, `focus()`, `checkValidity()`, `reportValidity()`,
and `setCustomValidity()`. `bm-select-change` bubbles across shadow boundaries
with `{ value, originalEvent }`.

Direct-child `bm-option` elements expose `value`, `disabled`, and initial
`selected`. Their text is the native option label. Interactive or rich option
content is unsupported; use the later Combobox for filtering and richer data.

Select parts are `container`, `field`, `select`, `label`, `error`, and
`error-icon`. The `error-icon` slot customizes only the decorative error icon.
A native single-value `<select>` supplies popup, keyboard, focus, form, and
accessibility behavior. Manual screen-reader and cross-browser testing remains
a release gate.
