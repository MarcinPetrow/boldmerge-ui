# Status

Import `@boldmerge/web/status` to register only `<bm-status>`.

```html
<bm-status tone="success">Operational</bm-status>
```

`tone` accepts `neutral`, `accent`, `success`, `warning`, or `error`. Visible
default-slot text is required by usage guidance: the 7px semantic dot is
decorative and hidden from assistive technology, so color is never the only
status indicator. Parts are `status`, `dot`, and `label`.

The component is static and does not add a live-region role. Applications that
need to announce a status change should choose the appropriate live-region
container based on urgency rather than making every status disruptive.
