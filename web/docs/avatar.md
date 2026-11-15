# Avatar and Avatar Group

Import `@boldmerge/web/avatar` to register `bm-avatar` and `bm-avatar-group`.

```html
<bm-avatar name="Ada Lovelace" availability="online"
  availability-label="Online"></bm-avatar>
<bm-avatar kind="organization" name="BoldMerge" size="large"></bm-avatar>
```

`bm-avatar` properties are `name`, `src`, `label`, `kind` (`person` or
`organization`), `size` (`small`, `medium`, `large`, `xlarge`), `availability`
(`none`, `online`, `away`, `busy`, `offline`), and `availabilityLabel`.
Image failures retain the deterministic fallback and emit the bubbling,
composed `bm-avatar-image-error` event. Slot `fallback` replaces generated
initials. Parts: `avatar`, `image`, `fallback`, and `availability`.

Fallback initials use the first Unicode character of the first and last name
segments. A single segment produces one initial. The fallback uses semantic
accent tokens rather than generating another color palette.

`bm-avatar-group` composes avatars in its default slot. Properties are `label`,
`size`, `overflowCount`, and `overflowLabel`; parts are `group` and `overflow`.
The consumer decides which avatars are visible and supplies the explicit
overflow count—there is no hidden data model.

People are circular; organizations use the approved 8px radius. Sizes are the
approved 24/32/40/48px values and do not respond to viewport or density.
`name` or `label` supplies the image accessible name. Availability is appended
only when the localized `availabilityLabel` is supplied; the dot is
supplemental and must not be the sole visible status where availability is
business-critical.
