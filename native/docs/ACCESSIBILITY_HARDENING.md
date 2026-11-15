# Native accessibility hardening

Passing GTK role/property tests or inspecting an AT-SPI tree is implementation
evidence, not a WCAG or screen-reader compliance claim. Manual Orca behavior,
announcement timing, navigation order, and user comprehension remain separate
release gates.

## Automated evidence

`native/tests/test-accessibility.py` launches the in-tree component catalog as
an independent GTK application and discovers its uniquely titled frame through
AT-SPI. It verifies representative interactive, navigation, progress, chart,
summary/table, and validation nodes. The required invalid workspace field must
expose both `REQUIRED` and `INVALID_ENTRY` states.

The Meson test exits with code 77 when no graphical display or Atspi 2.0
introspection binding is available. A skip is reported as a skip and must never
be described as a pass. Run the full check with:

```sh
meson test -C native/build --print-errorlogs
```

## Manual matrix

| Scenario | Expected observation | Status |
| --- | --- | --- |
| Catalog landmarks and headings | Orca can navigate the window, major sections, navigation, and named regions | Not run |
| Required validation | The field label, required state, error state/message, and Validation Summary target are understandable | Not run |
| Selection controls | Checkbox, radio group, switch, select, and combobox expose state and changes once | Not run |
| Dialog, Drawer, and Command Palette | Entry announcement, modal context, focus containment/restoration, and close result are coherent | Not run |
| Toast queue | New messages are announced without interrupting critical input and dismissal is understandable | Not run |
| Table and Pagination | Headers, selection, sort request, page context, and boundaries are navigable | Not run |
| Chart | Summary and durable table are discoverable; tooltip is supplemental and keyboard category changes are announced usefully | Not run |
| RTL and mixed scripts | Arabic direction plus Arabic/Polish values preserve reading and movement order | Not run |

Record the Orca version, GTK/libadwaita versions, desktop/session, locale, and
result for every execution. Defects found manually require focused automated
regression coverage where the platform exposes a stable assertion surface.
