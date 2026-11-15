# Icon

`<bm-icon>` renders the reviewed BoldMerge subset of Lucide 1.73.0. Icons use
the canonical 1.5px rounded stroke and `small` (16px), `default` (20px), or
`large` (24px) sizes. The package bundles icon geometry; it performs no network
request and does not depend on an operating-system icon theme. The selective
entry imports the exactly pinned `lucide` ESM dependency.

```html
<bm-icon name="search"></bm-icon>
<bm-icon name="circle-alert" size="large" label="Warning"></bm-icon>
```

Icons are decorative by default. Set `label` only when the icon itself conveys
meaning; icon buttons still require their own accessible `label`. Invalid names
render nothing and set `data-invalid` on the host. `BOLDMERGE_ICON_NAMES` and
the `BoldMergeIconName` type expose the complete supported catalog.

Lucide is pinned to 1.73.0 under the ISC license. See `THIRD_PARTY_NOTICES.md`.

