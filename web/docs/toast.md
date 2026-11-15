# Toast

Import `@boldmerge/web/toast` to register `<bm-toast>` and
`<bm-toast-region>`. A region owns its queue locally; the package does not
create a global notification singleton.

```ts
const region = document.querySelector('bm-toast-region');
region.push({
  title: 'Saved',
  message: 'Workspace settings updated.',
  tone: 'success',
});
```

`bm-toast` properties are `open`, `title`, `message`, `tone`, `duration`, and
`closeLabel`. Tones are `neutral`, `success`, `warning`, and `error`.
`duration=0` keeps a toast open; the default timeout is 5000 ms and pauses
while hovered, focused, or while the document is hidden. Warning/error use
`alert`; neutral/success use `status`. Slots are default message content and
`icon`. Parts are `toast`, `icon`, `content`, `title`, `message`, and `close`.

`bm-toast-region` provides `push(options)`, `dismiss(id)`, and `clear()`.
Properties are `placement`, `maxVisible` (default 3), and `label`. Placement is
logical `bottom-end`, `bottom-start`, `top-end`, or `top-start`. The `region`
part is exposed. Localize `closeLabel`, region `label`, title, and message.

Events are `bm-toast-close` (`{ id, reason, originalEvent? }`) and
`bm-toast-open-change`. Close reasons are `api`, `dismiss`, and `timeout`.
Reduced motion removes entrance animation without changing timeout behavior.
