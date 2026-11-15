# Topbar

Import `@boldmerge/web/topbar` to register only `<bm-topbar>`.

```html
<bm-topbar>
  <bm-icon-button slot="start" label="Open navigation">☰</bm-icon-button>
  <strong slot="start">Projects</strong>
  <bm-icon-button slot="notifications" label="Notifications">●</bm-icon-button>
  <bm-button slot="account" variant="ghost">Account</bm-button>
</bm-topbar>
```

Slots are `start`, default, `search`, `notifications`, and `account`. Parts are
`topbar`, `start`, and `actions`. The component supplies layout and the
canonical surface/border treatment only; consumers supply controls, landmarks,
search behavior, account menus, and localized accessible names.

The minimum height follows the active density through canonical button and
spacing tokens. Topbar does not switch density or hide actions based on the
viewport.
