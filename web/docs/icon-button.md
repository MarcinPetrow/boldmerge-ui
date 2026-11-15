# Icon Button

Import `@boldmerge/web/icon-button` to register only `<bm-icon-button>`.

```html
<bm-icon-button label="Add workspace" variant="primary">
  <svg aria-hidden="true" viewBox="0 0 24 24">…</svg>
</bm-icon-button>
```

`label` is required and becomes the native button's accessible name. Other
properties match Button: `variant` (`primary`, `secondary`, `outline`, `ghost`),
`treatment` (`default`, `destructive`), `type`, `disabled`, and `loading`.
`bm-activate` bubbles with the original mouse event. Submit/reset types operate
on the nearest form.

The default slot accepts one consumer-provided decorative icon. This package
does not currently bundle or download icons because the Lucide source/version
and license workflow remain open. Parts are `button`, `icon`, and `spinner`.
Native button focus, Enter, and Space behavior are preserved.
