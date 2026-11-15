# Page Layout

Import `@boldmerge/web/page-layout` to register only `<bm-page-layout>`.

```html
<bm-page-layout layout="split">
  <header slot="header">…</header>
  <main>Primary application content</main>
  <aside slot="aside">Supporting content</aside>
</bm-page-layout>
```

`layout` accepts `fluid` (default), `contained`, or `split`. The `header` slot
spans the layout, the default slot supplies primary content, and `aside` is
shown only by the split variant. Parts are `layout`, `header`, `body`,
`content`, and `aside`.

Contained content uses the provisional canonical 1200px maximum; split aside
uses the provisional 320px width. Gaps follow density tokens. The component
does not add landmarks, padding, routing, or viewport-driven mode changes;
applications supply semantic `<main>`/`<aside>` elements where appropriate.
