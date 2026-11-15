# `<bm-card>`

Import `@boldmerge/web/card` to register only `<bm-card>`.

```html
<bm-card label="Deployment summary">
  <h2 slot="header">Deployment</h2>
  <bm-button slot="actions" variant="ghost">Details</bm-button>
  <p>Production is healthy.</p>
  <span slot="footer">Updated two minutes ago</span>
</bm-card>
```

`label` supplies the internal article's accessible label. The component has no
interactive behavior and does not impose heading levels.

Slots: default content, `header`, `actions`, and `footer`. Empty named regions
are removed from layout. Parts: `card`, `header`, `actions`, `content`, and
`footer`.

The surface follows the approved flat treatment: semantic surface color, 8px
radius, density-aware 20/12px padding, no default shadow, and no visible border.

