# Command Palette

Import `@boldmerge/web/command-palette` to register the palette, command group,
and command elements. Commands are declarative DOM, not a JSON action schema.

```html
<bm-command-palette id="commands" shortcut label="Application commands">
  <bm-command-group label="Navigation">
    <bm-command value="settings" keywords="preferences">Open settings</bm-command>
  </bm-command-group>
</bm-command-palette>
```

Palette properties are `open`, `label`, `placeholder`, `emptyLabel`, and
opt-in `shortcut`. Methods are `show({ trigger, originalEvent })`,
`close(reason, originalEvent)`, and `focus()`. Commands expose `value`,
`keywords`, and `disabled`; groups expose `label`.

Events are `bm-command-palette-select`, `bm-command-palette-close`, and
`bm-command-palette-open-change`. The application handles routing and command
authorization after selection. Parts are `dialog`, `search`, `input`,
`shortcut`, `results`, `group`, `group-label`, `command`, and `empty`.
