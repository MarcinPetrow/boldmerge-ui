# Drawer

Import `@boldmerge/web/drawer` to register only `<bm-drawer>`.

```html
<bm-drawer id="details" mode="overlay" position="end" label="Project details">
  <main slot="content">Application content</main>
  <h2 slot="header">Project details</h2>
  <p>Drawer content</p>
  <button slot="actions" data-bm-drawer-close="done">Done</button>
</bm-drawer>
```

Properties are `open`, `label`, `dismissible`, `mode` (`overlay` default or
`push`), and logical `position` (`end` default or `start`). Slots are `content`,
`header`, default panel content, `footer`, and `actions`. Parts are `layout`,
`content`, `panel`, `overlay` or `push`, `frame`, `header`, `body`, `footer`, and
`actions`.

`show({ trigger, initialFocus, originalEvent })` and
`close(reason?, returnValue?, originalEvent?)` mirror Dialog. Close markers use
`data-bm-drawer-close`. Events are `bm-drawer-request-close` (cancelable),
`bm-drawer-close`, and `bm-drawer-open-change` with the corresponding typed
details.

Overlay uses a native modal dialog, backdrop, Escape policy, scroll lock, focus
containment, and restoration. Push is a non-modal desktop grid that keeps the
`content` region operable. At the provisional 768px breakpoint Push becomes
Overlay; viewport size never changes Balanced/Compact density. Consumers own
master-detail state and localized close controls.
