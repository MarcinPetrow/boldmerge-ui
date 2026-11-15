# Breadcrumbs

Import `@boldmerge/web/breadcrumbs` to register `<bm-breadcrumbs>` and
`<bm-breadcrumb-item>`.

```html
<bm-breadcrumbs label="Project breadcrumbs">
  <bm-breadcrumb-item href="/projects">Projects</bm-breadcrumb-item>
  <bm-breadcrumb-item href="/projects/northstar">Northstar</bm-breadcrumb-item>
  <bm-breadcrumb-item current>Overview</bm-breadcrumb-item>
</bm-breadcrumbs>
```

Breadcrumbs requires a localized `label` for its navigation landmark. Items
expose `href` and `current`; mark exactly one current page. Ancestors use native
links, the current item uses `aria-current="page"`, and the approved `/`
separator is decorative. Breadcrumbs parts are `nav` and `list`; item parts are
`separator` and `link`. Routing and click interception remain application
responsibilities.
