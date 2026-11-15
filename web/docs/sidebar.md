# Sidebar

Import `@boldmerge/web/sidebar` to register `<bm-sidebar>` and
`<bm-sidebar-item>`.

```html
<bm-sidebar label="Main navigation">
  <strong slot="header">BoldMerge</strong>
  <bm-sidebar-item href="/" active>
    <span slot="icon" aria-hidden="true">⌂</span>Overview
  </bm-sidebar-item>
  <small slot="footer">Version 1.0</small>
</bm-sidebar>
```

`bm-sidebar` exposes `label`, `collapsed`, and `open`. Its slots are `header`,
default navigation items, and `footer`; its parts are `backdrop`, `sidebar`,
`header`, `nav`, and `footer`. At the provisional 768px breakpoint, `open`
controls an overlay drawer. Escape, backdrop activation, and item navigation
dismiss it through the composed `bm-sidebar-dismiss` event.

`bm-sidebar-item` exposes `href`, `label`, `active`, and `disabled`; slots are
default label and `icon`; parts are `item`, `icon`, `label`, and `tooltip`.
Active items expose `aria-current="page"`. The composed
`bm-sidebar-navigate` event contains `href` and the original pointer event.
Applications own routing and localized labels.

Desktop collapse is visual only and retains accessible item names. Density is
token-driven and never selected by viewport width.
