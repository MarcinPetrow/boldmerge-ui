# App Shell

Import `@boldmerge/web/app-shell` to register only `<bm-app-shell>`. Import the
Sidebar and Topbar entries separately when composing a complete shell.

```html
<bm-app-shell>
  <bm-sidebar slot="sidebar" label="Main navigation">…</bm-sidebar>
  <bm-topbar slot="topbar">…</bm-topbar>
  <main>Application content</main>
</bm-app-shell>
```

`sidebarCollapsed` / `sidebar-collapsed` selects the approved 240px or 64px
desktop column. `sidebarOpen` / `sidebar-open` controls the mobile overlay.
`openSidebar(trigger)`, `closeSidebar()`, and `toggleSidebar(trigger)` manage the
drawer and restore focus to the supplied trigger after dismissal.

Slots are `sidebar`, `topbar`, and default content. Parts are `shell`,
`sidebar`, `workspace`, `topbar`, and `content`. The component propagates its
two state properties to a slotted `bm-sidebar`; it does not register that
component as a side effect.

The responsive overlay begins at the provisional canonical 768px breakpoint.
CSS media queries cannot consume custom properties, so the generated token and
component query intentionally repeat this value and are covered by structural
review. Responsiveness changes layout, never Balanced/Compact density.
