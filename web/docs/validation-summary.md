# Validation Summary

Import `@boldmerge/web/validation-summary` to register
`<bm-validation-summary>`. It summarizes the real constraint-validation state
of an existing form; it does not define or duplicate validation rules.

```html
<form id="profile" novalidate>…</form>
<bm-validation-summary for="profile"></bm-validation-summary>
```

Properties are `formId` (`for` attribute), `heading`, and `fallbackLabel`.
`errors` and `form` are read-only. Methods are `validate({ focus })`,
`refresh()`, and `focusFirstInvalid()`. Focus may target `first` (default),
`summary`, or `none`. Each rendered error activates the actual invalid control
and requests its native/integrated validity report.

The named `heading` slot and parts `summary`, `heading`, `list`, `item`, and
`link` are exposed. `bm-validation-summary-change` carries
`{ valid, count, errors }`. Localize `heading` and `fallbackLabel`. Labels come
from the component `label`, `aria-label`, an associated HTML label, control
text/name, then the localized fallback. The summary observes form changes but
applications should call `validate()` on an intercepted submit when using
`novalidate`.
