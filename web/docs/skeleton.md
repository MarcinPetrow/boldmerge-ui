# Skeleton

Import `@boldmerge/web/skeleton` to register only `bm-skeleton`.

```html
<section aria-busy="true" aria-label="Loading project">
  <bm-skeleton shape="rectangle" label="Loading project"></bm-skeleton>
  <bm-skeleton></bm-skeleton>
</section>
```

`shape` supports `text` (the default), `rectangle`, and `circle`. The default
geometry is one text line, a 64px-high rectangle, or a 40px circle. Set
`--bm-skeleton-width` and `--bm-skeleton-height` on an instance when the
placeholder must match its eventual content. Density does not silently change
these content-owned dimensions.

The visual placeholder is hidden from accessibility APIs. An optional
localized `label` adds a visually hidden status; use it once per loading group,
not on every placeholder. Set `aria-busy="true"` on the content region being
loaded, then remove the skeletons, label, and busy state together when content
arrives. The component does not infer or mutate a loading target.

The shimmer moves left to right on the approved 1.5s token. Under reduced
motion it becomes a static neutral gradient. `data-bm-motion="full"` retains
the documented explicit motion override. The exposed parts are `skeleton`,
`text`, `rectangle`, and `circle`.
