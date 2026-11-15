# `<bm-checkbox>`

Status: Batch 2B implemented API. Import `@boldmerge/web/checkbox` to
register only `<bm-checkbox>`.

```html
<bm-checkbox name="terms" required>Accept the terms</bm-checkbox>
<bm-checkbox checked>Enable notifications</bm-checkbox>
<bm-checkbox indeterminate>Select all results</bm-checkbox>
```

## API

| Property/attribute | Type | Default | Purpose |
| --- | --- | --- | --- |
| `checked` | `boolean` | `false` | Selected state and submitted-state switch. |
| `indeterminate` | `boolean` | `false` | Mixed visual/accessibility state; user activation clears it. |
| `disabled` | `boolean` | `false` | Removes activation and focus. |
| `required` | `boolean` | `false` | Requires `checked` for valid form submission. |
| `name` | `string` | empty | Form-associated field name. |
| `value` | `string` | `on` | Value submitted when checked. |

The default slot is the visible and accessible label. Parts are `control`,
`indicator`, and `label`.

`bm-checkbox-change` is a bubbling, composed
`CustomEvent<CheckboxChangeDetail>` with
`checked`, `indeterminate`, `value`, and `originalEvent` fields.

Methods: `focus()`, `click()`, `setCustomValidity()`, `checkValidity()`, and
`reportValidity()`. Form-facing getters: `form`, `validity`,
`validationMessage`, and `willValidate`.

The component uses an internal native checkbox and `ElementInternals`. Space,
label click, focus order, disabled semantics, required validity, form reset, and
mixed-state accessibility therefore follow native browser behavior. Invalid
state is exposed with `aria-invalid` and error border treatment; Checkbox does
not invent the expanded error panel specified specifically for Input.

