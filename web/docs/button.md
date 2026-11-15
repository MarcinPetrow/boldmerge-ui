# `<bm-button>`

Import `@boldmerge/web/button` to register only `<bm-button>`.

```html
<bm-button variant="primary">Save</bm-button>
<bm-button variant="outline" treatment="destructive">Delete</bm-button>
<bm-button loading>Saving</bm-button>
```

## API

| Property/attribute | Type | Default | Purpose |
| --- | --- | --- | --- |
| `variant` | `primary \| secondary \| outline \| ghost` | `primary` | Approved visual variant. |
| `treatment` | `default \| destructive` | `default` | Applies independent error semantics. |
| `type` | `button \| submit \| reset` | `button` | Requests the closest form action. |
| `disabled` | `boolean` | `false` | Disables native activation and focus. |
| `loading` | `boolean` | `false` | Shows a reduced-motion-safe spinner and blocks activation. |

Slots: default label, `prefix`, and `suffix`. Loading temporarily replaces both
named slots with the spinner. Parts: `button`, `label`, and `spinner`.

`bm-activate` is a bubbling, composed `CustomEvent<ButtonActivateDetail>` after
successful pointer or keyboard activation. Native `click` remains available.
Disabled and loading controls emit neither event.

The inner native button supplies Enter/Space behavior, focus, and disabled
semantics. A `submit` button calls `requestSubmit()` on its closest form; because
custom elements cannot currently be passed as native submitters, it does not add
a submitter name/value pair to `FormData`.

