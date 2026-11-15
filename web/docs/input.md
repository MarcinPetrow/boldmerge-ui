# `<bm-input>`

Import `@boldmerge/web/input` to register only `<bm-input>`.

```html
<bm-input name="email" label="Email address" type="email" required></bm-input>
<bm-input label="Workspace" error="Workspace is required."></bm-input>
```

## API

| Property/attribute | Type | Default | Purpose |
| --- | --- | --- | --- |
| `value` | `string` | empty | Current form value. |
| `label` | `string` | empty | Visible floating label and accessible name. Required for accessible use. |
| `type` | `text \| email \| password \| search \| tel \| url` | `text` | Native input type. |
| `name` | `string` | empty | Form-associated field name. |
| `required`, `disabled`, `readonly` | `boolean` | `false` | Native-equivalent states. |
| `error` | `string` | empty | Programmatic custom error displayed inside the expanded field. |
| `autocomplete`, `inputMode`, `placeholder` | `string` | empty | Native input hints. |

Methods: `focus()`, `select()`, `setCustomValidity()`, `checkValidity()`, and
`reportValidity()`. Read-only form properties: `form`, `validity`,
`validationMessage`, and `willValidate`.

Slots: `leading`, `trailing`, and `error-icon`. Parts: `container`, `field`,
`input`, `label`, `error`, and `error-icon`.

`bm-input` and `bm-change` are bubbling, composed
`CustomEvent<InputValueDetail>` events containing `value` and `originalEvent`.

The component is form-associated through `ElementInternals`. Its integrated
error has a stable ID referenced by the native input's `aria-errormessage` and
`aria-describedby`; errors are not exposed by color alone. Native validation is
shown after blur, `reportValidity()`, or an invalid form submission.

