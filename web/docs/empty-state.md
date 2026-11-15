# Empty State

Import `@boldmerge/web/empty-state` to register only `bm-empty-state`.

```html
<bm-empty-state heading="No projects yet"
  description="Create a project to start tracking deployments.">
  <span slot="icon" aria-hidden="true">◇</span>
  <bm-button slot="action">Create project</bm-button>
</bm-empty-state>
```

Properties are `heading`, `description`, `label`, and `headingLevel` (clamped
to 1–6). The region uses its heading as the accessible name unless an explicit
localized `label` is supplied. Slots are `icon`, `description`, and `action`.
Parts are `state`, `icon`, `title`, `description`, and `action`.

The icon and action are optional and their empty layout regions are removed.
The package supplies no illustration or action behavior. Consumer-provided
actions keep their own native or component keyboard semantics. Use the
`description` slot for rich text; otherwise the `description` property supplies
plain text. Empty State is not a live region by default, so applications must
announce dynamic result changes separately when required.

Spacing uses existing canonical tokens. The restrained centered maximum width
and 32×24px container padding are provisional defaults recorded in the decision
log because the specification does not approve exact Empty State measurements.
