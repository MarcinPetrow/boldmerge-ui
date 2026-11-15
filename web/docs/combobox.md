# Combobox

Import `@boldmerge/web/combobox` to register `<bm-combobox>` and the shared
declarative `<bm-option>`. Use Select for short, non-searchable lists.

```html
<bm-combobox name="region" label="Region" required>
  <bm-option value="us">United States</bm-option>
  <bm-option value="eu">Europe</bm-option>
  <span slot="empty">No matching regions</span>
</bm-combobox>
```

Properties are `value`, `inputValue`, `label`, `name`, `placeholder`,
`required`, `disabled`, `error`, `open`, and `filter`. It implements the native
form validity methods and exposes `show()`, `close()`, and `focus()`.

Events are `bm-combobox-input` (`{ inputValue, originalEvent }`),
`bm-combobox-change` (`{ value, label, originalEvent }`), and
`bm-combobox-open-change`. Input notification is deferred until IME composition
ends. Arrow keys, Home/End, Enter, Escape, and Tab follow the documented
combobox/listbox behavior; disabled options are skipped.

Parts are `container`, `field`, `input`, `label`, `toggle`, `listbox`, `option`,
`empty`, `error`, and `error-icon`. The `empty` slot is localizable. Consumers
doing remote filtering should set `filter=false`, update declarative options,
and own loading/error copy. Virtualization is not currently included.
