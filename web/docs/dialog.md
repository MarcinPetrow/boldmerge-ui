# Dialog

Import `@boldmerge/web/dialog` to register only `<bm-dialog>`.

```html
<bm-dialog id="confirm" label="Delete project">
  <h2 slot="header">Delete project?</h2>
  <p>This action cannot be undone.</p>
  <button slot="actions" data-bm-dialog-close="cancel">Cancel</button>
  <button slot="actions" data-bm-dialog-close="confirmed">Delete</button>
</bm-dialog>
```

Properties are `open`, `label`, and `dismissible` (default `true`). Slots are
`header`, default content, `footer`, and `actions`. Parts are `dialog`, `frame`,
`header`, `content`, `footer`, and `actions`.

`show({ trigger, initialFocus, originalEvent })` opens the native modal dialog.
`close(reason?, returnValue?, originalEvent?)` requests closing and returns
`false` when a listener cancels it. Supplying the trigger enables deterministic
focus restoration. A slotted element with `data-bm-dialog-close="value"`
requests an action close and supplies the return value.

Events are:

- `bm-dialog-request-close`: cancelable detail `{ reason, returnValue,
  originalEvent? }`.
- `bm-dialog-close`: completed close with the same detail.
- `bm-dialog-open-change`: `{ open, originalEvent? }` for component API/user
  transitions.

Reasons are `api`, `action`, `escape`, or `backdrop`. `dismissible=false`
blocks Escape and backdrop dismissal but not explicit API or action closing.
The native `<dialog>` supplies top-layer rendering, outside inertness, and
modal focus containment. Consumers must provide localized headings, close
controls, destructive confirmation copy, and a meaningful initial focus.
