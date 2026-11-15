# Switch

Import `@boldmerge/web/switch` to register only `<bm-switch>`.

```html
<bm-switch name="alerts" value="enabled" checked>
  Deployment alerts
</bm-switch>
```

Properties/attributes are `checked`, `disabled`, `required`, `name`, and
`value` (`"on"` by default). The component is form-associated and exposes
`form`, `validity`, `validationMessage`, `willValidate`, `checkValidity()`,
`reportValidity()`, and `setCustomValidity()`.

The default slot is its required visible label. Parts are `label`,
`label-text`, `track`, and `thumb`. `bm-switch-change` bubbles across shadow
boundaries with `{ checked, value, originalEvent }`.

An internal native checkbox with `role="switch"` supplies focus, Space,
disabled, required, and form behavior. Motion follows the canonical transition
token and becomes static under `prefers-reduced-motion: reduce`.
