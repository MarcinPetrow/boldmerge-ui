# Pagination

Import `@boldmerge/web/pagination` to register only `bm-pagination`, or use the
full `@boldmerge/web` entry point.

```html
<bm-pagination page="3" page-size="25" total-items="236"></bm-pagination>
```

## API

Properties: `page`, `pageSize` (default `25`), `totalItems`, `siblingCount`,
`disabled`, `label`, `previousLabel`, `nextLabel`, `pageLabel`, and `ofLabel`. The
read-only `pageCount` getter derives the bounded count. `goToPage()`, `next()`,
and `previous()` return whether the page changed.

The `bm-page-change` event bubbles and is composed. Its typed detail contains
`page`, `pageSize`, `totalItems`, and the optional `originalEvent`. The
application responds by loading or displaying its own data; Pagination never
fetches, sorts, or renders records.

Slot `page-size` accepts an application-owned page-size control. Parts are
`nav`, `previous`, `pages`, `page`, `current`, `ellipsis`, `next`, `summary`,
and `page-size`.

## Accessibility and localization

The host renders a labelled native `nav` and native buttons. The current page
uses `aria-current="page"`; unavailable boundary controls are disabled, and a
polite text summary reports the current and total pages. All built-in visible
labels are properties and must be localized by the application. Balanced and
Compact geometry comes from canonical tokens and never from viewport width.
Numeric values use bidi isolation, and the page list scrolls locally rather
than forcing page-level horizontal overflow at narrow reflow widths.
