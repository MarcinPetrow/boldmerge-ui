# Native components

These widgets implement the shared semantic contract with GTK-native APIs.
They do not copy Web Component properties, DOM events, slots, or Shadow DOM.
Every component has a `theme-scope` property; setting it explicitly propagates
theme, density, preset/custom accent, and subsequent runtime scope changes.

## `BmButton`

`BmButton` subclasses `GtkButton`, preserving GTK's button accessible role,
focus rendering, Space/Enter activation, `clicked` signal, `GtkActionable`
integration, mnemonics, and `sensitive` state.

Properties:

- `text: string`
- `variant: BmButtonVariant` — Primary, Secondary, Outline, or Ghost
- `treatment: BmButtonTreatment` — Default or Destructive
- `loading: boolean`
- `content: GtkWidget` — replaces the text label for custom native composition
- `theme-scope: BmThemeScope`

Signals are inherited from `GtkButton`; connect to `clicked`. Loading adds a
native `GtkSpinner`, makes the button insensitive, and restores its prior
sensitivity afterward. The spinner and GTK state transitions respect the
platform animation setting; BoldMerge adds no click scaling.

Applications should use `text`/`content`, not inherited `GtkButton:label` or
`GtkButton:child`, because BoldMerge owns the internal layout containing the
spinner. Custom content must provide an accessible label when its visible
content does not already name the action.

## `BmInput`

`BmInput` composes a native `GtkEntry`. The entry retains GTK editing,
selection, input-method, clipboard, keyboard, focus, and text-box accessible
behavior. `bm_input_get_entry()` is the native extension point for input
purpose, visibility/password mode, completion, icons, and other `GtkEntry`
features.

Properties:

- `text`, `label`, `placeholder-text`
- `required`, `editable`
- `error`, `required-message`, read-only `valid`
- `leading-widget`, `trailing-widget`
- `theme-scope`

Signals:

- `changed` after the native entry text changes
- `activate` after native entry activation, normally Enter
- `validation-changed(boolean valid)`

`bm_input_validate()` performs the foundation's required-value check. Product
validation sets `error` directly. The default required message passes through
the `boldmerge-1` gettext domain and can also be replaced per instance. Error
text appears in the integrated region, and the entry receives accessible
label, required/read-only, invalid, description, and error-message metadata.

Leading/trailing widgets are full GTK widgets rather than Web icon slots. Any
interactive trailing action remains independently focusable and must supply its
own accessible label.

## `BmCard`

`BmCard` is a flat `GtkWidget` composition container with no default border or
shadow. Its properties are:

- `header: GtkWidget`
- `actions: GtkWidget`
- `child: GtkWidget`
- `footer: GtkWidget`
- `accessible-label: string`
- `theme-scope: BmThemeScope`

Empty regions are hidden. Card does not impose heading levels or button
semantics on supplied widgets. Set `accessible-label` when the card represents
a named group; otherwise its child content remains naturally exposed.

## `BmCheckbox`

`BmCheckbox` subclasses `GtkCheckButton` and preserves native Space activation,
focus, checked/mixed accessibility state, `toggled`, and sensitivity. Its API
adds `text`, `checked`, `indeterminate`, `required`, read-only `valid`,
`validate()`, and `theme-scope`. Required validation is intentionally narrow:
the checkbox is valid only when checked.

## `BmRadioGroup` and `BmRadio`

`BmRadio` subclasses `GtkCheckButton`; GTK owns exclusive selection and native
radio keyboard behavior. Each radio has `text`, `value`, `checked`, and
`theme-scope`. `BmRadioGroup` owns the group label, children, selected `value`,
required validation, and emits `changed(string value)`. Add options with
`bm_radio_group_append()` rather than grouping unrelated widgets manually.

## `BmSwitch`

`BmSwitch` composes a visible label with a native `GtkSwitch`. It exposes
`text`, `active`, `theme-scope`, the underlying switch through
`bm_switch_get_control()`, and emits `changed(boolean active)`. GTK retains the
switch role, focus, pointer and keyboard interaction, and platform animation.

## `BmSelect`

`BmSelect` composes a labelled native `GtkDropDown` backed by a string model.
Append plain text/value pairs with `bm_select_append()`. It exposes `label`,
`value`, `required`, `error`, read-only `valid`, `validate()`, `changed(string
value)`, and `theme-scope`. Rich/searchable options belong to the later
Combobox; per-option disabled state is not part of this initial Native slice.

## `BmCombobox`

`BmCombobox` is a required/error-capable searchable selection control backed by
GTK's native `GtkDropDown` search popup. Append plain text/value pairs, read or
set `value`, and use the same `valid`/`validate()` contract as Select. Its
`changed` signal supplies both value and visible label. This Native slice uses
the platform search field inside the open popup rather than the Web component's
always-editable inline field; per-option disabled state is not yet exposed.

## `BmIconButton`

`BmIconButton` subclasses `GtkButton` and shares Button variants, treatments,
loading, sensitivity, and theme scope. Consumers provide a decorative native
icon widget and a required accessible `label`. `bm_icon_new()` supplies the
reviewed Lucide 1.73.0 subset at 16, 20, or 24px; arbitrary consumer widgets
remain supported.

## `BmTag`

`BmTag` is a noninteractive accent-subtle label with optional native `prefix`
and `suffix` widgets. It intentionally provides no selection, dismissal, link,
or button behavior.

## `BmStatus`

`BmStatus` is static visible text plus a decorative dot. `tone` accepts
Neutral, Accent, Success, Warning, or Error. It does not create a live region;
applications own announcement timing for dynamic status changes.

## Layout and navigation

`BmPageLayout` provides Fluid, Contained (1200px proposal via `AdwClamp`), and
Split (320px aside proposal) modes with header/content/aside widget slots.
`BmSection` provides labelled header/actions/content/footer composition and an
optional separator with density-aware spacing.

`BmBreadcrumbs` owns a labelled navigation region and appends
`BmBreadcrumbItem` ancestors as native `GtkLinkButton` links; current items are
visible non-link text. Routing remains application-owned.

`BmTabs` composes native `GtkStackSwitcher`/`GtkStack` behavior and emits
`changed(string value)`. `BmSegmentedControl` groups native toggle buttons into
one exclusive value and emits the same shaped signal. The initial Native Tabs
slice delegates keyboard/focus behavior to GTK and does not yet expose disabled
pages or vertical orientation.

## Application shell

`BmSidebarItem` subclasses `GtkButton` and carries visible `text`, an
application-owned `uri`, an `active` state, and optional native icon content.
`BmSidebar` exposes a labelled navigation region with optional header/footer,
appended items, expanded/collapsed presentation, and a `navigate(string uri)`
signal. The signal expresses intent only; routing and active-item updates remain
application-owned.

`BmTopbar` is an open horizontal composition with `start`, central `child`,
`search`, `notifications`, and `account` widget regions. Supplied controls keep
their native GTK focus and accessibility behavior.

`BmAppShell` composes Sidebar, Topbar, and application content with
libadwaita's `AdwFlap`. `sidebar-collapsed` controls the compact desktop rail;
`sidebar-open` controls flap visibility; and `mobile` switches between permanent
desktop and modal folded behavior. Applications set `mobile` from their window
policy instead of the component inferring density or a Web breakpoint from
viewport size. `open_sidebar(trigger)` remembers a trigger weakly and
`close_sidebar()` attempts to restore focus to it.

## Tooltip and Dropdown Menu

`BmTooltip` owns exactly one native child and a noninteractive text `content`.
It delegates hover timing, placement, dismissal, and platform animation to
GTK's tooltip system, while applying the same content as the trigger's
accessible description. This intentionally follows desktop settings rather
than forcing the Web implementation's 400ms timer or browser collision logic.

`BmDropdown` wraps a native `GtkMenuButton` and `GtkPopover`. Append individual
`BmDropdownItem` buttons or labelled `BmDropdownGroup` containers. Items expose
plain text/value pairs, disabled state, destructive treatment, and optional
native prefix/suffix widgets. Activating an enabled item emits `select(string
value)` and closes the popover. `open`, `disabled`, and `open-changed` expose
the host-controlled state; `bm_dropdown_get_trigger()` provides the native
menu button for action integration when needed.

GTK owns trigger semantics, arrow-key focus traversal, Escape/outside-click
dismissal, placement, and focus restoration. Nested submenus, URI navigation,
and destructive confirmation remain application-owned.

## Dialog and Drawer

`BmDialog` is a reusable modal `GtkWindow` with labelled header, content,
footer, and actions regions. `bm_dialog_present()` accepts a transient parent,
focus-restoration trigger, and optional initial-focus widget. Escape and window
close attempts respect `dismissible`. Programmatic and action closure emit a
cancellable `request-close(BmCloseReason, return-value)` signal followed by
`closed` and `open-changed` when accepted. `bm_dialog_add_action()` creates a
native action button with an associated return value.

The implementation deliberately uses `GtkWindow` rather than `AdwDialog` so
the documented libadwaita 1.0 minimum remains true; `AdwDialog` requires
libadwaita 1.5. GTK owns modal input, focus traversal, window stacking, and
platform presentation. Unlike the Web dialog, clicking outside a modal native
window is not a dismissal gesture.

`BmDrawer` composes application content and an open panel using `AdwFlap`.
`mode` selects Overlay or Push, `position` selects logical Start or End, and an
explicit `mobile` flag forces Overlay without inferring a viewport breakpoint.
The panel exposes header/content/footer/actions regions and the same
cancellable close reasons and focus-trigger restoration as Dialog. Overlay
mode delegates modal backdrop/swipe behavior to libadwaita; Push mode keeps the
application content operable.

## Command Palette

`BmCommandPalette` is a reusable modal `GtkWindow` containing a native
`GtkSearchEntry` and grouped `GtkListBox`. Add `BmCommand` objects with label,
value, keywords, group, and optional disabled state. Filtering is local and
case-insensitive; activation emits `selected(string value, string label)` and
restores focus to the supplied trigger. `activate_first()` provides the same
first-enabled-command action for application key bindings and tests.

The application owns routing, authorization, side effects, and any global
accelerator such as Ctrl+K. Keeping accelerator registration outside the
component prevents conflicts when multiple windows or palettes exist.

## Pagination

`BmPagination` derives a bounded visible page range from `page`, `page-size`,
`total-items`, and `sibling-count`. The canonical default page size is 25;
invalid zero sizes normalize to that default, and page values are clamped when
the dataset changes. Previous, next, and numbered controls emit
`page-changed(page, page-size, total-items)` only for an accepted user or API
transition. Applications retain ownership of data loading and may compose a
native page-size control with `bm_pagination_set_page_size_widget()`.

## Table

`BmTable` wraps GTK's accessible `GtkColumnView` without accepting record or
column arrays. The application supplies a `GtkSelectionModel` and native
`GtkListItemFactory` for every column, preserving ownership of record objects,
cell composition, formatting, and updates. `bm_table_append_column()` transfers
ownership of its factory to GTK and returns a borrowed column.

Sortable columns use their `sort-key` only to expose native header direction
and emit `sort-request(key, direction)`; the component never reorders the
consumer's model. Native selection emits selected count plus all-selected
state. Optional row activation emits the model position and borrowed item
object. Ready, Loading, and Empty states share one labelled component, while
applications own responsive columns, routing, fetching, and virtualization.

## Avatar and Avatar Group

`BmAvatar` renders a person circle or organization rounded square at the fixed
approved 24/32/40/48px sizes. Fallback text is the uppercased first Unicode
character of the first and last whitespace-delimited name segments; a single
segment produces one initial. `name` or explicit `label` names the image.
Availability adds a supplemental dot and only extends the accessible label
when the application supplies localized `availability-label` text.

`src` is loaded through `GdkTexture`; failure keeps the deterministic fallback
and emits `image-error(string src)`. `BmAvatarGroup` composes caller-owned
avatars, applies one explicit size, and shows a caller-supplied overflow count
and label. It does not hide or derive members from a data model.

## Empty State

`BmEmptyState` is a named GTK region with a heading level clamped to 1–6,
optional plain or widget description, optional icon, and optional action
widget. It supplies no illustration, action behavior, or live announcement.
Applications own dynamic announcement timing and the semantics of composed
controls.

## Skeleton, KPI, and Segmented Progress

`BmSkeleton` provides text, rectangle, and circle geometry with an optional
single status label. Its 1.5-second GTK-drawn gradient becomes static when
system animations are disabled. Applications own the busy content boundary.

`BmKpi` keeps label, formatted value, change, and comparison as the complete
meaning. Favorable/unfavorable tone is explicit; its numeric sparkline is
decorative. `BmProgress` clamps an exact value to its range and exposes native
progressbar semantics while rounding only the 1–100 visual segment fill.
Applications own formatting and localized value text.

## Chart and Chart Tooltip

`BmChart` accepts categories and one or more `BmChartSeries` objects through
typed, programmatic APIs. Every point carries a finite numeric value for the
Cairo line plot and an application-formatted string for the tooltip and
assistive text. Applications retain responsibility for localization, units,
sorting, sampling, streaming, and missing-data policy; there is no runtime JSON
chart schema.

The plot is focusable. Left/Right inspect adjacent categories (with direction
reversed in RTL), Home/End move to the first/last category, and Escape closes
the owned `BmChartTooltip`. Pointer motion and click select the nearest
category. The active accessible label contains the category and every available
series value.

The constructor's summary is the visible textual interpretation of the trend.
Applications must also provide a durable table or equivalent widget through
`bm_chart_set_table()`; the tooltip is transient and is not the accessible
alternative. Series length is intentionally independent of category count, and
only their shared prefix is plotted.

## Toast and Validation Summary

`BmToast` is a typed GObject carrying an ID, title, message, Neutral/Success/
Warning/Error tone, and duration in seconds. `BmToastRegion` wraps a local
`AdwToastOverlay`; applications place the region explicitly around their
content and add or push toasts. libadwaita displays one toast at a time and owns
priority ordering, timeout pausing, swipe dismissal, animation, and accessible
announcement behavior. The region retains queued toasts and exposes their
total `count`; there is no global notification singleton.

`bm_toast_region_push()` returns a caller-owned `BmToast` reference while the
region holds its own reference until dismissal. Warning/Error map to high
libadwaita priority. Because libadwaita does not report whether an unrequested
dismissal was a timeout or swipe, such native dismissals are reported as
Timeout; explicit API dismissals preserve their supplied reason.

`BmValidationSummary` registers existing `BmInput`, `BmSelect`, `BmCombobox`,
`BmCheckbox`, or `BmRadioGroup` widgets. It calls each control's native `validate()` method,
reads its actual `valid`/error/label contract, renders an alert-role list, and
focuses the underlying native control when an error row is activated. Optional
registration labels/messages localize or override the control-derived text.
This is explicit GTK aggregation rather than a parallel form schema.

## C example

```c
BmThemeScope *scope = bm_theme_scope_new (NULL);
bm_theme_scope_set_theme (scope, BM_THEME_DARK);
bm_theme_scope_set_density (scope, BM_DENSITY_COMPACT);

GtkWidget *card = bm_card_new ();
bm_card_set_theme_scope (BM_CARD (card), scope);
bm_card_set_header (BM_CARD (card), gtk_label_new ("Workspace"));

GtkWidget *input = bm_input_new ("Workspace name");
bm_input_set_required (BM_INPUT (input), TRUE);
bm_input_set_theme_scope (BM_INPUT (input), scope);
bm_card_set_child (BM_CARD (card), input);

GtkWidget *save = bm_button_new ("Save");
bm_button_set_theme_scope (BM_BUTTON (save), scope);
g_signal_connect (save, "clicked", G_CALLBACK (save_workspace), NULL);
bm_card_set_footer (BM_CARD (card), save);
```
