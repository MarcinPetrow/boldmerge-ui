# Section

Import `@boldmerge/web/section` to register only `<bm-section>`.

```html
<bm-section label="Deployment overview" separator>
  <h2 slot="header">Deployment</h2>
  <bm-button slot="actions">Configure</bm-button>
  <p>Production is healthy.</p>
  <span slot="footer">Updated now</span>
</bm-section>
```

`label` provides an accessible section name and `separator` adds the optional
end divider. Slots are `header`, `actions`, default content, and `footer`.
Parts are `section`, `header`, `actions`, `content`, and `footer`. Vertical
spacing is the approved density-aware `--bm-section-gap` (32px Balanced, 24px
Compact); the component does not impose heading levels.
