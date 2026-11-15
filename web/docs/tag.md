# Tag

Import `@boldmerge/web/tag` to register only `<bm-tag>`.

```html
<bm-tag><span slot="prefix" aria-hidden="true">◆</span>Featured</bm-tag>
```

Tag is an intentionally noninteractive accent-subtle label. The default slot
contains its visible text; optional `prefix` and `suffix` slots accept small
decorative content. Parts are `tag` and `label`.

Tag does not implement selection, dismissal, links, or buttons. Compose the
appropriate native interactive element separately when interaction is needed.
