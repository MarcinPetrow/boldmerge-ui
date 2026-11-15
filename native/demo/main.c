#include <boldmerge/boldmerge.h>

typedef struct {
  BmThemeScope *root_scope;
  BmThemeScope *nested_scope;
  GtkLabel *status;
  BmButton *theme_button;
  BmButton *density_button;
  BmButton *accent_button;
  BmInput *required_input;
  guint accent_index;
} DemoState;

static const BmAccent accents[] = {
  BM_ACCENT_CYAN,
  BM_ACCENT_BLUE,
  BM_ACCENT_VIOLET,
  BM_ACCENT_ORANGE,
  BM_ACCENT_GREEN,
  BM_ACCENT_CUSTOM,
};

static const char *accent_names[] = {
  "Cyan",
  "Blue",
  "Violet",
  "Orange",
  "Green",
  "Custom",
};

static GtkWidget *
new_label(const char *text, const char *css_class)
{
  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  if (css_class != NULL)
    gtk_widget_add_css_class(label, css_class);
  return label;
}

static GtkWidget *
new_stack(gint spacing)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
  gtk_widget_set_hexpand(box, TRUE);
  return box;
}

static GtkWidget *
new_flow(void)
{
  GtkWidget *flow = gtk_flow_box_new();
  gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flow), GTK_SELECTION_NONE);
  gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(flow), FALSE);
  gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(flow), 1);
  gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 4);
  gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(flow), 8);
  gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flow), 8);
  gtk_widget_set_halign(flow, GTK_ALIGN_START);
  return flow;
}

static GtkWidget *
new_section(DemoState *state,
            const char *title,
            const char *description,
            GtkWidget *content)
{
  GtkWidget *card_widget = bm_card_new();
  BmCard *card = BM_CARD(card_widget);
  bm_card_set_theme_scope(card, state->root_scope);
  bm_card_set_accessible_label(card, title);

  GtkWidget *header = new_stack(4);
  gtk_box_append(GTK_BOX(header), new_label(title, "title-2"));
  if (description != NULL)
    gtk_box_append(GTK_BOX(header), new_label(description, "dim-label"));
  bm_card_set_header(card, header);
  bm_card_set_child(card, content);
  return card_widget;
}

static GtkWidget *
new_button(DemoState *state,
           const char *text,
           BmButtonVariant variant,
           BmButtonTreatment treatment)
{
  GtkWidget *button = bm_button_new(text);
  bm_button_set_variant(BM_BUTTON(button), variant);
  bm_button_set_treatment(BM_BUTTON(button), treatment);
  bm_button_set_theme_scope(BM_BUTTON(button), state->root_scope);
  return button;
}

static void
update_status(DemoState *state)
{
  BmTheme theme = bm_theme_scope_get_effective_theme(state->root_scope);
  BmDensity density = bm_theme_scope_get_effective_density(state->root_scope);
  const char *theme_name = theme == BM_THEME_DARK ? "Dark" : "Light";
  const char *density_name = density == BM_DENSITY_COMPACT ? "Compact" : "Balanced";
  g_autofree char *status = g_strdup_printf("%s · %s · %s",
                                            theme_name,
                                            density_name,
                                            accent_names[state->accent_index]);
  g_autofree char *theme_text = g_strdup_printf("Theme: %s", theme_name);
  g_autofree char *density_text = g_strdup_printf("Density: %s", density_name);
  g_autofree char *accent_text = g_strdup_printf("Accent: %s",
                                                  accent_names[state->accent_index]);

  gtk_label_set_text(state->status, status);
  bm_button_set_text(state->theme_button, theme_text);
  bm_button_set_text(state->density_button, density_text);
  bm_button_set_text(state->accent_button, accent_text);
}

static void
on_theme_clicked(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  BmTheme next = bm_theme_scope_get_effective_theme(state->root_scope) == BM_THEME_DARK
                   ? BM_THEME_LIGHT
                   : BM_THEME_DARK;
  bm_theme_scope_set_theme(state->root_scope, next);
  bm_theme_scope_set_theme(state->nested_scope,
                           next == BM_THEME_DARK ? BM_THEME_LIGHT : BM_THEME_DARK);
  update_status(state);
}

static void
on_density_clicked(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  BmDensity next = bm_theme_scope_get_effective_density(state->root_scope) == BM_DENSITY_COMPACT
                     ? BM_DENSITY_BALANCED
                     : BM_DENSITY_COMPACT;
  bm_theme_scope_set_density(state->root_scope, next);
  bm_theme_scope_set_density(state->nested_scope,
                             next == BM_DENSITY_COMPACT
                               ? BM_DENSITY_BALANCED
                               : BM_DENSITY_COMPACT);
  update_status(state);
}

static void
on_accent_clicked(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  state->accent_index = (state->accent_index + 1) % G_N_ELEMENTS(accents);
  if (accents[state->accent_index] == BM_ACCENT_CUSTOM) {
    GdkRGBA seed;
    gdk_rgba_parse(&seed, "#c026d3");
    g_autoptr(GError) error = NULL;
    if (!bm_theme_scope_set_custom_accent(state->root_scope, &seed, &error)) {
      g_warning("Custom accent rejected: %s", error->message);
      state->accent_index = 0;
      bm_theme_scope_set_accent(state->root_scope, BM_ACCENT_CYAN);
    }
  } else {
    bm_theme_scope_set_accent(state->root_scope, accents[state->accent_index]);
  }
  update_status(state);
}

static void
on_validate_clicked(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  bm_input_validate(state->required_input);
  gtk_widget_grab_focus(GTK_WIDGET(bm_input_get_entry(state->required_input)));
}

static GtkWidget *
create_foundation_section(DemoState *state)
{
  GtkWidget *content = new_stack(12);
  GtkWidget *accent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(accent, "bm-accent-sample");
  gtk_widget_add_css_class(accent, "bm-density-sample");
  bm_theme_scope_apply(state->root_scope, accent);
  gtk_box_append(GTK_BOX(accent), new_label("Canonical accent sample", "heading"));
  gtk_box_append(GTK_BOX(content), accent);

  GtkWidget *nested = new_stack(8);
  gtk_widget_add_css_class(nested, "bm-surface");
  gtk_widget_add_css_class(nested, "bm-density-sample");
  bm_theme_scope_apply(state->nested_scope, nested);
  gtk_box_append(GTK_BOX(nested), new_label("Nested scope override", "heading"));
  gtk_box_append(GTK_BOX(nested),
                 new_label("This panel always uses the opposite theme and density. "
                           "The setting is explicit and never inferred from viewport size.",
                           "dim-label"));
  GtkWidget *nested_button = bm_button_new("Nested action");
  bm_button_set_variant(BM_BUTTON(nested_button), BM_BUTTON_VARIANT_OUTLINE);
  bm_button_set_theme_scope(BM_BUTTON(nested_button), state->nested_scope);
  gtk_widget_set_halign(nested_button, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(nested), nested_button);
  gtk_box_append(GTK_BOX(content), nested);

  return new_section(state,
                     "Foundation",
                     "Runtime theme, density, preset/custom accent, and nested scope propagation.",
                     content);
}

static GtkWidget *
create_button_section(DemoState *state)
{
  GtkWidget *content = new_stack(16);
  gtk_box_append(GTK_BOX(content), new_label("Variants", "heading"));
  GtkWidget *variants = new_flow();
  gtk_flow_box_append(GTK_FLOW_BOX(variants),
                      new_button(state, "Primary", BM_BUTTON_VARIANT_PRIMARY,
                                 BM_BUTTON_TREATMENT_DEFAULT));
  gtk_flow_box_append(GTK_FLOW_BOX(variants),
                      new_button(state, "Secondary", BM_BUTTON_VARIANT_SECONDARY,
                                 BM_BUTTON_TREATMENT_DEFAULT));
  gtk_flow_box_append(GTK_FLOW_BOX(variants),
                      new_button(state, "Outline", BM_BUTTON_VARIANT_OUTLINE,
                                 BM_BUTTON_TREATMENT_DEFAULT));
  gtk_flow_box_append(GTK_FLOW_BOX(variants),
                      new_button(state, "Ghost", BM_BUTTON_VARIANT_GHOST,
                                 BM_BUTTON_TREATMENT_DEFAULT));
  gtk_box_append(GTK_BOX(content), variants);

  gtk_box_append(GTK_BOX(content), new_label("Destructive treatment", "heading"));
  GtkWidget *destructive = new_flow();
  gtk_flow_box_append(GTK_FLOW_BOX(destructive),
                      new_button(state, "Delete", BM_BUTTON_VARIANT_PRIMARY,
                                 BM_BUTTON_TREATMENT_DESTRUCTIVE));
  gtk_flow_box_append(GTK_FLOW_BOX(destructive),
                      new_button(state, "Remove", BM_BUTTON_VARIANT_SECONDARY,
                                 BM_BUTTON_TREATMENT_DESTRUCTIVE));
  gtk_flow_box_append(GTK_FLOW_BOX(destructive),
                      new_button(state, "Revoke", BM_BUTTON_VARIANT_OUTLINE,
                                 BM_BUTTON_TREATMENT_DESTRUCTIVE));
  gtk_flow_box_append(GTK_FLOW_BOX(destructive),
                      new_button(state, "Discard", BM_BUTTON_VARIANT_GHOST,
                                 BM_BUTTON_TREATMENT_DESTRUCTIVE));
  gtk_box_append(GTK_BOX(content), destructive);

  gtk_box_append(GTK_BOX(content), new_label("States and custom content", "heading"));
  GtkWidget *states = new_flow();
  GtkWidget *loading = new_button(state, "Saving", BM_BUTTON_VARIANT_PRIMARY,
                                  BM_BUTTON_TREATMENT_DEFAULT);
  bm_button_set_loading(BM_BUTTON(loading), TRUE);
  gtk_flow_box_append(GTK_FLOW_BOX(states), loading);

  GtkWidget *disabled = new_button(state, "Disabled", BM_BUTTON_VARIANT_SECONDARY,
                                   BM_BUTTON_TREATMENT_DEFAULT);
  gtk_widget_set_sensitive(disabled, FALSE);
  gtk_flow_box_append(GTK_FLOW_BOX(states), disabled);

  GtkWidget *custom = new_button(state, NULL, BM_BUTTON_VARIANT_OUTLINE,
                                 BM_BUTTON_TREATMENT_DEFAULT);
  GtkWidget *custom_content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(custom_content),
                 gtk_image_new_from_icon_name("document-save-symbolic"));
  gtk_box_append(GTK_BOX(custom_content), gtk_label_new("Custom content"));
  bm_button_set_content(BM_BUTTON(custom), custom_content);
  gtk_accessible_update_property(GTK_ACCESSIBLE(custom),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, "Save with custom content",
                                 -1);
  gtk_flow_box_append(GTK_FLOW_BOX(states), custom);
  gtk_box_append(GTK_BOX(content), states);

  return new_section(state,
                     "Button",
                     "All four variants, both treatments, loading, disabled, and custom-content states.",
                     content);
}

static GtkWidget *
create_input_section(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_box_set_homogeneous(GTK_BOX(content), TRUE);
  GtkWidget *left = new_stack(12);
  GtkWidget *right = new_stack(12);
  gtk_box_append(GTK_BOX(content), left);
  gtk_box_append(GTK_BOX(content), right);

  GtkWidget *empty = bm_input_new("Workspace name");
  bm_input_set_placeholder_text(BM_INPUT(empty), "Example workspace");
  bm_input_set_theme_scope(BM_INPUT(empty), state->root_scope);
  gtk_box_append(GTK_BOX(left), empty);

  GtkWidget *filled = bm_input_new("Owner email");
  bm_input_set_text(BM_INPUT(filled), "person@example.com");
  bm_input_set_theme_scope(BM_INPUT(filled), state->root_scope);
  gtk_box_append(GTK_BOX(left), filled);

  GtkWidget *decorated = bm_input_new("Search projects");
  bm_input_set_placeholder_text(BM_INPUT(decorated), "Type to filter");
  bm_input_set_leading_widget(BM_INPUT(decorated),
                              gtk_image_new_from_icon_name("system-search-symbolic"));
  bm_input_set_trailing_widget(BM_INPUT(decorated),
                               gtk_image_new_from_icon_name("edit-clear-symbolic"));
  bm_input_set_theme_scope(BM_INPUT(decorated), state->root_scope);
  gtk_box_append(GTK_BOX(left), decorated);

  GtkWidget *required = bm_input_new("Required project code");
  state->required_input = BM_INPUT(required);
  bm_input_set_required(state->required_input, TRUE);
  bm_input_set_required_message(state->required_input, "Enter a project code to continue.");
  bm_input_set_theme_scope(state->required_input, state->root_scope);
  gtk_box_append(GTK_BOX(right), required);

  GtkWidget *invalid = bm_input_new("Server validation");
  bm_input_set_text(BM_INPUT(invalid), "reserved-name");
  bm_input_set_error(BM_INPUT(invalid), "This workspace name is already in use.");
  bm_input_set_theme_scope(BM_INPUT(invalid), state->root_scope);
  gtk_box_append(GTK_BOX(right), invalid);

  GtkWidget *readonly = bm_input_new("Organization ID");
  bm_input_set_text(BM_INPUT(readonly), "org_42BA7");
  bm_input_set_editable(BM_INPUT(readonly), FALSE);
  bm_input_set_theme_scope(BM_INPUT(readonly), state->root_scope);
  gtk_box_append(GTK_BOX(right), readonly);

  GtkWidget *validate = new_button(state, "Validate required input",
                                   BM_BUTTON_VARIANT_OUTLINE,
                                   BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(validate, "clicked", G_CALLBACK(on_validate_clicked), state);
  gtk_widget_set_halign(validate, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(right), validate);

  return new_section(state,
                     "Input",
                     "Empty, filled, decorated, required, invalid, and read-only examples using native GtkEntry behavior.",
                     content);
}

static GtkWidget *
create_navigation_section(DemoState *state)
{
  GtkWidget *content = new_stack(16);
  BmBreadcrumbs *breadcrumbs = BM_BREADCRUMBS(bm_breadcrumbs_new("Project breadcrumbs"));
  bm_breadcrumbs_set_theme_scope(breadcrumbs, state->root_scope);
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(bm_breadcrumb_item_new("Projects", "https://example.test/projects", FALSE)));
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(bm_breadcrumb_item_new("Northstar", "https://example.test/northstar", FALSE)));
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(bm_breadcrumb_item_new("Overview", NULL, TRUE)));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(breadcrumbs));

  BmSegmentedControl *segments = BM_SEGMENTED_CONTROL(bm_segmented_control_new("View mode"));
  bm_segmented_control_set_theme_scope(segments, state->root_scope);
  bm_segmented_control_append(segments, "List", "list");
  bm_segmented_control_append(segments, "Board", "board");
  bm_segmented_control_append(segments, "Timeline", "timeline");
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(segments));

  BmTabs *tabs = BM_TABS(bm_tabs_new("Project views"));
  bm_tabs_set_theme_scope(tabs, state->root_scope);
  bm_tabs_append(tabs, "overview", "Overview", gtk_label_new("Overview panel content"));
  bm_tabs_append(tabs, "activity", "Activity", gtk_label_new("Activity panel content"));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(tabs));

  BmPageLayout *layout = BM_PAGE_LAYOUT(bm_page_layout_new());
  bm_page_layout_set_theme_scope(layout, state->root_scope);
  bm_page_layout_set_mode(layout, BM_PAGE_LAYOUT_SPLIT);
  bm_page_layout_set_header(layout, new_label("Split page layout", "heading"));
  BmSection *section = BM_SECTION(bm_section_new("Deployment overview"));
  bm_section_set_theme_scope(section, state->root_scope);
  bm_section_set_header(section, new_label("Primary content", "heading"));
  bm_section_set_child(section, new_label("Production is healthy.", NULL));
  bm_section_set_footer(section, new_label("Updated now", "dim-label"));
  bm_section_set_separator(section, TRUE);
  bm_page_layout_set_child(layout, GTK_WIDGET(section));
  bm_page_layout_set_aside(layout, new_label("Supporting content", "dim-label"));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(layout));
  return new_section(state, "Layout and navigation",
                     "Page Layout, Section, Breadcrumbs, Tabs, and Segmented Control.", content);
}

static GtkWidget *
create_shell_section(DemoState *state)
{
  BmSidebar *sidebar = BM_SIDEBAR(bm_sidebar_new("Workspace navigation"));
  bm_sidebar_set_theme_scope(sidebar, state->root_scope);
  GtkWidget *brand = new_label("BoldMerge", "heading");
  bm_sidebar_set_header(sidebar, brand);
  BmSidebarItem *overview = BM_SIDEBAR_ITEM(
    bm_sidebar_item_new("Overview", "/overview",
                        gtk_image_new_from_icon_name("view-grid-symbolic")));
  bm_sidebar_item_set_active(overview, TRUE);
  bm_sidebar_append(sidebar, overview);
  bm_sidebar_append(sidebar, BM_SIDEBAR_ITEM(
    bm_sidebar_item_new("Projects", "/projects",
                        gtk_image_new_from_icon_name("folder-symbolic"))));
  bm_sidebar_append(sidebar, BM_SIDEBAR_ITEM(
    bm_sidebar_item_new("Reports", "/reports",
                        gtk_image_new_from_icon_name("document-open-recent-symbolic"))));

  BmTopbar *topbar = BM_TOPBAR(bm_topbar_new());
  bm_topbar_set_theme_scope(topbar, state->root_scope);
  bm_topbar_set_start(topbar, new_label("Workspace overview", "heading"));
  bm_topbar_set_search(topbar, gtk_search_entry_new());
  GtkWidget *notifications = bm_icon_button_new(
    "Notifications", gtk_image_new_from_icon_name("preferences-system-notifications-symbolic"));
  bm_icon_button_set_theme_scope(BM_ICON_BUTTON(notifications), state->root_scope);
  bm_topbar_set_notifications(topbar, notifications);
  GtkWidget *account = new_button(state, "MK", BM_BUTTON_VARIANT_GHOST,
                                  BM_BUTTON_TREATMENT_DEFAULT);
  bm_topbar_set_account(topbar, account);

  GtkWidget *content = new_stack(8);
  gtk_widget_set_margin_top(content, 32);
  gtk_widget_set_margin_start(content, 32);
  gtk_widget_set_margin_end(content, 32);
  gtk_box_append(GTK_BOX(content), new_label("Application content", "title-2"));
  gtk_box_append(GTK_BOX(content),
                 new_label("Sidebar, Topbar, and content remain native GTK composition.",
                           "dim-label"));

  BmAppShell *shell = BM_APP_SHELL(bm_app_shell_new());
  bm_app_shell_set_theme_scope(shell, state->root_scope);
  bm_app_shell_set_sidebar(shell, sidebar);
  bm_app_shell_set_topbar(shell, topbar);
  bm_app_shell_set_child(shell, content);
  gtk_widget_set_size_request(GTK_WIDGET(shell), 760, 400);
  return new_section(state, "Application shell",
                     "Sidebar, Topbar, and App Shell with explicit desktop/mobile policy.",
                     GTK_WIDGET(shell));
}

static GtkWidget *
create_overlay_section(DemoState *state)
{
  GtkWidget *content = new_flow();
  GtkWidget *tooltip_button = new_button(
    state, "Create project", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  BmTooltip *tooltip = BM_TOOLTIP(
    bm_tooltip_new(tooltip_button, "Create a new project"));
  bm_tooltip_set_theme_scope(tooltip, state->root_scope);
  gtk_flow_box_append(GTK_FLOW_BOX(content), GTK_WIDGET(tooltip));

  BmDropdown *dropdown = BM_DROPDOWN(bm_dropdown_new("Project actions"));
  bm_dropdown_set_theme_scope(dropdown, state->root_scope);
  BmDropdownItem *rename = BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Rename", "rename"));
  bm_dropdown_item_set_prefix(
    rename, gtk_image_new_from_icon_name("document-edit-symbolic"));
  bm_dropdown_append_item(dropdown, rename);
  BmDropdownGroup *edit = BM_DROPDOWN_GROUP(
    bm_dropdown_group_new("Edit actions"));
  bm_dropdown_group_append(edit, BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Duplicate", "duplicate")));
  BmDropdownItem *archive = BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Archive unavailable", "archive"));
  bm_dropdown_item_set_disabled(archive, TRUE);
  bm_dropdown_group_append(edit, archive);
  bm_dropdown_append_group(dropdown, edit);
  BmDropdownGroup *danger = BM_DROPDOWN_GROUP(
    bm_dropdown_group_new("Destructive actions"));
  bm_dropdown_group_set_destructive(danger, TRUE);
  bm_dropdown_group_append(danger, BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Delete project", "delete")));
  bm_dropdown_append_group(dropdown, danger);
  gtk_flow_box_append(GTK_FLOW_BOX(content), GTK_WIDGET(dropdown));

  return new_section(state, "Tooltip and Dropdown Menu",
                     "GTK-native tooltip timing/placement and a non-modal popover menu.",
                     content);
}

static void
destroy_demo_dialog(gpointer data)
{
  gtk_window_destroy(GTK_WINDOW(data));
  g_object_unref(data);
}

static void
open_demo_dialog(GtkButton *button, gpointer data)
{
  DemoState *state = data;
  BmDialog *dialog = g_object_get_data(G_OBJECT(button), "demo-dialog");
  if (dialog == NULL) {
    dialog = BM_DIALOG(g_object_ref_sink(bm_dialog_new("Create workspace")));
    bm_dialog_set_theme_scope(dialog, state->root_scope);
    bm_dialog_set_header(dialog, new_label("Create workspace", "title-2"));
    bm_dialog_set_child(dialog,
      new_label("Review the workspace settings before continuing.", NULL));
    bm_dialog_set_footer(dialog,
      new_label("This dialog uses native GTK modal focus behavior.", "dim-label"));
    bm_dialog_add_action(dialog, "Cancel", "cancel", FALSE, FALSE);
    bm_dialog_add_action(dialog, "Create", "create", TRUE, FALSE);
    g_object_set_data_full(G_OBJECT(button), "demo-dialog", dialog,
                           destroy_demo_dialog);
  }
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
  bm_dialog_present(dialog, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
                    GTK_WIDGET(button), NULL);
}

static void
open_demo_drawer(GtkButton *button, gpointer data)
{
  bm_drawer_open(BM_DRAWER(data), GTK_WIDGET(button), NULL);
}

static void
close_demo_drawer(GtkButton *button, gpointer data)
{
  (void) button;
  bm_drawer_close(BM_DRAWER(data), BM_CLOSE_REASON_ACTION, "close");
}

static GtkWidget *
create_modal_section(DemoState *state)
{
  GtkWidget *content = new_stack(16);
  GtkWidget *open_dialog = new_button(
    state, "Open dialog", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(open_dialog, "clicked", G_CALLBACK(open_demo_dialog), state);
  gtk_widget_set_halign(open_dialog, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), open_dialog);

  BmDrawer *drawer = BM_DRAWER(bm_drawer_new("Project details"));
  bm_drawer_set_theme_scope(drawer, state->root_scope);
  bm_drawer_set_mode(drawer, BM_DRAWER_MODE_PUSH);
  GtkWidget *workspace = new_stack(8);
  GtkWidget *open_drawer = new_button(
    state, "Open details", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(open_drawer, "clicked", G_CALLBACK(open_demo_drawer), drawer);
  gtk_box_append(GTK_BOX(workspace), new_label("Workspace content", "title-3"));
  gtk_box_append(GTK_BOX(workspace), open_drawer);
  bm_drawer_set_content(drawer, workspace);
  bm_drawer_set_header(drawer, new_label("Project details", "title-3"));
  bm_drawer_set_child(drawer,
    new_label("Owner: Platform team\nStatus: Ready for review", NULL));
  GtkWidget *close_drawer = new_button(
    state, "Close", BM_BUTTON_VARIANT_OUTLINE, BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(close_drawer, "clicked", G_CALLBACK(close_demo_drawer), drawer);
  bm_drawer_set_actions(drawer, close_drawer);
  gtk_widget_set_size_request(GTK_WIDGET(drawer), 760, 360);
  bm_drawer_open(drawer, NULL, NULL);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(drawer));

  return new_section(state, "Dialog and Drawer",
                     "Modal GtkWindow dialog plus explicit Overlay/Push drawer modes.",
                     content);
}

static void
open_demo_command_palette(GtkButton *button, gpointer data)
{
  DemoState *state = data;
  BmCommandPalette *palette = g_object_get_data(
    G_OBJECT(button), "demo-command-palette");
  if (palette == NULL) {
    palette = BM_COMMAND_PALETTE(g_object_ref_sink(
      bm_command_palette_new("Workspace commands")));
    bm_command_palette_set_theme_scope(palette, state->root_scope);
    g_autoptr(BmCommand) create = bm_command_new(
      "Create project", "project.create", "new workspace", "Projects");
    g_autoptr(BmCommand) settings = bm_command_new(
      "Open settings", "settings.open", "preferences", "Navigation");
    g_autoptr(BmCommand) archive = bm_command_new(
      "Archive project", "project.archive", "remove", "Projects");
    bm_command_set_disabled(archive, TRUE);
    bm_command_palette_append(palette, create);
    bm_command_palette_append(palette, settings);
    bm_command_palette_append(palette, archive);
    g_object_set_data_full(G_OBJECT(button), "demo-command-palette", palette,
                           destroy_demo_dialog);
  }
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
  bm_command_palette_present(
    palette, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
    GTK_WIDGET(button));
}

static GtkWidget *
create_search_section(DemoState *state)
{
  GtkWidget *content = new_stack(12);
  BmCombobox *region = BM_COMBOBOX(bm_combobox_new("Searchable region"));
  bm_combobox_append(region, "Europe", "eu");
  bm_combobox_append(region, "United States", "us");
  bm_combobox_append(region, "Asia Pacific", "apac");
  bm_combobox_set_value(region, "eu");
  bm_combobox_set_required(region, TRUE);
  bm_combobox_set_theme_scope(region, state->root_scope);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(region));

  GtkWidget *open = new_button(
    state, "Open command palette", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  gtk_widget_set_halign(open, GTK_ALIGN_START);
  g_signal_connect(open, "clicked",
                   G_CALLBACK(open_demo_command_palette), state);
  gtk_box_append(GTK_BOX(content), open);
  return new_section(state, "Combobox and Command Palette",
                     "Searchable native selection and grouped command filtering.",
                     content);
}

static GtkWidget *
create_pagination_section(DemoState *state)
{
  BmPagination *pagination = BM_PAGINATION(
    bm_pagination_new("Project pages"));
  bm_pagination_set_total_items(pagination, 236);
  bm_pagination_set_page(pagination, 4);
  bm_pagination_set_theme_scope(pagination, state->root_scope);
  return new_section(state, "Pagination",
                     "Application-owned paging with a bounded native page range.",
                     GTK_WIDGET(pagination));
}

static void
demo_table_setup(GtkSignalListItemFactory *factory, GtkListItem *item,
                 gpointer data)
{
  (void) factory; (void) data;
  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_list_item_set_child(item, label);
}

static void
demo_table_bind(GtkSignalListItemFactory *factory, GtkListItem *item,
                gpointer data)
{
  (void) factory; (void) data;
  gtk_label_set_text(GTK_LABEL(gtk_list_item_get_child(item)),
    gtk_string_object_get_string(
      GTK_STRING_OBJECT(gtk_list_item_get_item(item))));
}

static GtkWidget *
create_table_section(DemoState *state)
{
  GtkStringList *items = gtk_string_list_new(NULL);
  gtk_string_list_append(items, "Atlas · Operational · $12,400");
  gtk_string_list_append(items, "Northstar · In review · $8,920");
  gtk_string_list_append(items, "Beacon · Operational · $4,180");
  GtkMultiSelection *selection = gtk_multi_selection_new(G_LIST_MODEL(items));
  BmTable *table = BM_TABLE(bm_table_new(
    "Workspace projects", GTK_SELECTION_MODEL(selection)));
  g_object_unref(selection);
  GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
  g_signal_connect(factory, "setup", G_CALLBACK(demo_table_setup), NULL);
  g_signal_connect(factory, "bind", G_CALLBACK(demo_table_bind), NULL);
  bm_table_append_column(table, "Project · Status · Monthly spend", "name",
                         factory, TRUE);
  bm_table_set_row_activation(table, TRUE);
  bm_table_set_theme_scope(table, state->root_scope);
  gtk_widget_set_size_request(GTK_WIDGET(table), -1, 220);
  return new_section(state, "Table",
                     "Consumer-owned GTK model, native selection, row activation, and application-controlled sorting.",
                     GTK_WIDGET(table));
}

static GtkWidget *
create_avatar_empty_section(DemoState *state)
{
  GtkWidget *content = new_stack(20);
  BmAvatarGroup *group = BM_AVATAR_GROUP(
    bm_avatar_group_new("Platform team"));
  bm_avatar_group_set_theme_scope(group, state->root_scope);
  const char *names[] = { "Ada Lovelace", "Grace Hopper", "Alan Turing" };
  for (guint i = 0; i < G_N_ELEMENTS(names); i++) {
    BmAvatar *avatar = BM_AVATAR(bm_avatar_new(names[i]));
    bm_avatar_set_theme_scope(avatar, state->root_scope);
    if (i == 0) {
      bm_avatar_set_availability(avatar, BM_AVATAR_AVAILABILITY_ONLINE);
      bm_avatar_set_availability_label(avatar, "Online");
    }
    bm_avatar_group_append(group, avatar);
  }
  bm_avatar_group_set_overflow(group, 3, "3 more teammates");
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(group));

  BmEmptyState *empty = BM_EMPTY_STATE(bm_empty_state_new("No projects yet"));
  bm_empty_state_set_description(
    empty, "Create a project to start tracking deployments.");
  bm_empty_state_set_icon(
    empty, gtk_image_new_from_icon_name("folder-new-symbolic"));
  bm_empty_state_set_action(empty, new_button(
    state, "Create project", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT));
  bm_empty_state_set_theme_scope(empty, state->root_scope);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(empty));
  return new_section(state, "Avatar and Empty State",
                     "Deterministic identity fallbacks and compositional absence feedback.",
                     content);
}
static GtkWidget *create_metrics_section(DemoState*state){GtkWidget*c=new_stack(16);BmSkeleton*sk=BM_SKELETON(bm_skeleton_new(BM_SKELETON_RECTANGLE,"Loading metrics"));bm_skeleton_set_theme_scope(sk,state->root_scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(sk));BmKpi*k=BM_KPI(bm_kpi_new("Monthly recurring revenue","$128,400"));double v[]={72,74,73,79,82,86,91};bm_kpi_set_change(k,"+8.2%",BM_KPI_CHANGE_FAVORABLE);bm_kpi_set_comparison(k,"versus last month");bm_kpi_set_sparkline(k,v,G_N_ELEMENTS(v));bm_kpi_set_theme_scope(k,state->root_scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(k));BmProgress*p=BM_PROGRESS(bm_progress_new("Storage used"));bm_progress_set_value(p,37);bm_progress_set_value_label(p,"37 GB of 100 GB");bm_progress_set_theme_scope(p,state->root_scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(p));return new_section(state,"Skeleton, KPI, and Segmented Progress","Loading, textual metrics, and exact progress semantics.",c);}

static GtkWidget *
create_chart_section(DemoState *state)
{
  BmChart *chart = BM_CHART(bm_chart_new(
    "Weekly recurring revenue",
    "Actual revenue rises from $42K to $68K and remains above forecast after Tuesday."));
  g_autoptr(BmChartSeries) actual = bm_chart_series_new("Actual");
  g_autoptr(BmChartSeries) forecast = bm_chart_series_new("Forecast");
  const char *categories[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" };
  const double actual_values[] = { 42, 56, 61, 58, 68 };
  const double forecast_values[] = { 45, 52, 57, 61, 64 };
  for (guint index = 0; index < G_N_ELEMENTS(categories); index++) {
    g_autofree char *actual_label = g_strdup_printf("$%.0fK", actual_values[index]);
    g_autofree char *forecast_label = g_strdup_printf("$%.0fK", forecast_values[index]);
    bm_chart_append_category(chart, categories[index]);
    bm_chart_series_append(actual, actual_values[index], actual_label);
    bm_chart_series_append(forecast, forecast_values[index], forecast_label);
  }
  bm_chart_append_series(chart, actual);
  bm_chart_append_series(chart, forecast);
  bm_chart_set_theme_scope(chart, state->root_scope);

  GtkWidget *table = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(table), 18);
  gtk_grid_set_row_spacing(GTK_GRID(table), 4);
  gtk_grid_attach(GTK_GRID(table), gtk_label_new("Day"), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(table), gtk_label_new("Actual"), 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(table), gtk_label_new("Forecast"), 2, 0, 1, 1);
  for (guint index = 0; index < G_N_ELEMENTS(categories); index++) {
    g_autofree char *actual_label = g_strdup_printf("$%.0fK", actual_values[index]);
    g_autofree char *forecast_label = g_strdup_printf("$%.0fK", forecast_values[index]);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new(categories[index]), 0, index + 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new(actual_label), 1, index + 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new(forecast_label), 2, index + 1, 1, 1);
  }
  bm_chart_set_table(chart, table);
  return new_section(state, "Chart and Chart Tooltip",
                     "Programmatic multi-series data, pointer and keyboard inspection, plus summary and durable table alternative.",
                     GTK_WIDGET(chart));
}

static void
push_demo_toast(GtkButton *button, gpointer data)
{
  (void) button;
  BmToast *toast = bm_toast_region_push(
    BM_TOAST_REGION(data), "Saved", "Workspace settings updated.",
    BM_TOAST_TONE_SUCCESS, 5);
  g_object_unref(toast);
}

static void
validate_demo_summary(GtkButton *button, gpointer data)
{
  (void) button;
  bm_validation_summary_validate(BM_VALIDATION_SUMMARY(data),
                                 BM_VALIDATION_FOCUS_FIRST);
}

static GtkWidget *
create_feedback_section(DemoState *state)
{
  BmToastRegion *region = BM_TOAST_REGION(
    bm_toast_region_new("Notifications"));
  bm_toast_region_set_theme_scope(region, state->root_scope);
  GtkWidget *content = new_stack(12);

  GtkWidget *notify = new_button(
    state, "Show success toast", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(notify, "clicked", G_CALLBACK(push_demo_toast), region);
  gtk_widget_set_halign(notify, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), notify);

  BmValidationSummary *summary = BM_VALIDATION_SUMMARY(
    bm_validation_summary_new("Correct the following errors"));
  bm_validation_summary_set_theme_scope(summary, state->root_scope);
  BmInput *name = BM_INPUT(bm_input_new("Workspace name"));
  bm_input_set_required(name, TRUE);
  bm_input_set_theme_scope(name, state->root_scope);
  BmCheckbox *terms = BM_CHECKBOX(bm_checkbox_new("Accept terms"));
  bm_checkbox_set_required(terms, TRUE);
  bm_checkbox_set_theme_scope(terms, state->root_scope);
  bm_validation_summary_add(summary, GTK_WIDGET(name), NULL, NULL);
  bm_validation_summary_add(summary, GTK_WIDGET(terms), NULL, NULL);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(summary));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(name));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(terms));
  GtkWidget *validate = new_button(
    state, "Validate form", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(validate, "clicked", G_CALLBACK(validate_demo_summary), summary);
  gtk_widget_set_halign(validate, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), validate);
  bm_validation_summary_validate(summary, BM_VALIDATION_FOCUS_NONE);

  bm_toast_region_set_child(region, content);
  BmToast *initial = bm_toast_region_push(
    region, "Native feedback", "Toast queue is owned by this region.",
    BM_TOAST_TONE_NEUTRAL, 0);
  g_object_unref(initial);
  return new_section(state, "Toast and Validation Summary",
                     "Local AdwToast queue and native control validity aggregation.",
                     GTK_WIDGET(region));
}

static GtkWidget *
create_display_section(DemoState *state)
{
  GtkWidget *content = new_stack(16);
  GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_set_homogeneous(GTK_BOX(top), TRUE);

  GtkWidget *selects = new_stack(8);
  BmSelect *region = BM_SELECT(bm_select_new("Region"));
  bm_select_append(region, "Europe", "eu");
  bm_select_append(region, "United States", "us");
  bm_select_append(region, "Asia Pacific", "apac");
  bm_select_set_value(region, "eu");
  bm_select_set_required(region, TRUE);
  bm_select_set_theme_scope(region, state->root_scope);
  gtk_box_append(GTK_BOX(selects), GTK_WIDGET(region));
  BmSelect *invalid = BM_SELECT(bm_select_new("Environment"));
  bm_select_append(invalid, "Production", "production");
  bm_select_set_error(invalid, "Choose a non-production environment.");
  bm_select_set_theme_scope(invalid, state->root_scope);
  gtk_box_append(GTK_BOX(selects), GTK_WIDGET(invalid));
  gtk_box_append(GTK_BOX(top), selects);

  GtkWidget *icons = new_stack(8);
  gtk_box_append(GTK_BOX(icons), new_label("Icon Button", "heading"));
  GtkWidget *icon_flow = new_flow();
  static const char *icon_names[] = { "list-add-symbolic", "document-save-symbolic", "edit-delete-symbolic" };
  static const char *icon_labels[] = { "Add", "Save", "Delete" };
  for (guint index = 0; index < G_N_ELEMENTS(icon_names); index++) {
    GtkWidget *button = bm_icon_button_new(icon_labels[index],
                                           gtk_image_new_from_icon_name(icon_names[index]));
    bm_icon_button_set_variant(BM_ICON_BUTTON(button),
      index == 0 ? BM_BUTTON_VARIANT_PRIMARY : BM_BUTTON_VARIANT_OUTLINE);
    if (index == 2) bm_icon_button_set_treatment(BM_ICON_BUTTON(button), BM_BUTTON_TREATMENT_DESTRUCTIVE);
    bm_icon_button_set_theme_scope(BM_ICON_BUTTON(button), state->root_scope);
    gtk_flow_box_append(GTK_FLOW_BOX(icon_flow), button);
  }
  gtk_box_append(GTK_BOX(icons), icon_flow);
  gtk_box_append(GTK_BOX(top), icons);
  gtk_box_append(GTK_BOX(content), top);

  GtkWidget *tags = new_flow();
  GtkWidget *featured = bm_tag_new("Featured");
  bm_tag_set_prefix(BM_TAG(featured), gtk_image_new_from_icon_name("starred-symbolic"));
  bm_tag_set_theme_scope(BM_TAG(featured), state->root_scope);
  gtk_flow_box_append(GTK_FLOW_BOX(tags), featured);
  GtkWidget *new_tag = bm_tag_new("New");
  bm_tag_set_theme_scope(BM_TAG(new_tag), state->root_scope);
  gtk_flow_box_append(GTK_FLOW_BOX(tags), new_tag);
  gtk_box_append(GTK_BOX(content), tags);

  GtkWidget *statuses = new_flow();
  static const char *status_text[] = { "Neutral", "Active", "Operational", "Delayed", "Failed" };
  for (guint tone = BM_STATUS_TONE_NEUTRAL; tone <= BM_STATUS_TONE_ERROR; tone++) {
    GtkWidget *status = bm_status_new(status_text[tone], tone);
    bm_status_set_theme_scope(BM_STATUS(status), state->root_scope);
    gtk_flow_box_append(GTK_FLOW_BOX(statuses), status);
  }
  gtk_box_append(GTK_BOX(content), statuses);

  return new_section(state,
                     "Select and compact display",
                     "Native Select and Icon Button plus static Tag and Status components.",
                     content);
}

static GtkWidget *
create_selection_section(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);
  gtk_box_set_homogeneous(GTK_BOX(content), TRUE);

  GtkWidget *checks = new_stack(10);
  gtk_box_append(GTK_BOX(checks), new_label("Checkbox", "heading"));
  GtkWidget *unchecked = bm_checkbox_new("Unchecked");
  bm_checkbox_set_theme_scope(BM_CHECKBOX(unchecked), state->root_scope);
  gtk_box_append(GTK_BOX(checks), unchecked);
  GtkWidget *checked = bm_checkbox_new("Checked");
  bm_checkbox_set_checked(BM_CHECKBOX(checked), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(checked), state->root_scope);
  gtk_box_append(GTK_BOX(checks), checked);
  GtkWidget *mixed = bm_checkbox_new("Indeterminate");
  bm_checkbox_set_indeterminate(BM_CHECKBOX(mixed), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(mixed), state->root_scope);
  gtk_box_append(GTK_BOX(checks), mixed);
  GtkWidget *required = bm_checkbox_new("Required and invalid");
  bm_checkbox_set_required(BM_CHECKBOX(required), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(required), state->root_scope);
  gtk_box_append(GTK_BOX(checks), required);
  gtk_box_append(GTK_BOX(content), checks);

  GtkWidget *radios = new_stack(10);
  gtk_box_append(GTK_BOX(radios), new_label("Radio Group", "heading"));
  BmRadioGroup *group = BM_RADIO_GROUP(bm_radio_group_new("Deployment channel"));
  bm_radio_group_set_required(group, TRUE);
  bm_radio_group_set_theme_scope(group, state->root_scope);
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Stable", "stable")));
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Preview", "preview")));
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Nightly", "nightly")));
  bm_radio_group_set_value(group, "stable");
  gtk_box_append(GTK_BOX(radios), GTK_WIDGET(group));
  gtk_box_append(GTK_BOX(content), radios);

  GtkWidget *switches = new_stack(10);
  gtk_box_append(GTK_BOX(switches), new_label("Switch", "heading"));
  GtkWidget *switch_on = bm_switch_new("Email notifications");
  bm_switch_set_active(BM_SWITCH(switch_on), TRUE);
  bm_switch_set_theme_scope(BM_SWITCH(switch_on), state->root_scope);
  gtk_box_append(GTK_BOX(switches), switch_on);
  GtkWidget *switch_off = bm_switch_new("Automatic updates");
  bm_switch_set_theme_scope(BM_SWITCH(switch_off), state->root_scope);
  gtk_box_append(GTK_BOX(switches), switch_off);
  GtkWidget *switch_disabled = bm_switch_new("Unavailable setting");
  bm_switch_set_theme_scope(BM_SWITCH(switch_disabled), state->root_scope);
  gtk_widget_set_sensitive(switch_disabled, FALSE);
  gtk_box_append(GTK_BOX(switches), switch_disabled);
  gtk_box_append(GTK_BOX(content), switches);

  return new_section(state,
                     "Selection controls",
                     "Native Checkbox, grouped Radio, and Switch states with GTK keyboard and accessibility behavior.",
                     content);
}

static GtkWidget *
create_card_section(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_box_set_homogeneous(GTK_BOX(content), TRUE);

  GtkWidget *simple_widget = bm_card_new();
  BmCard *simple = BM_CARD(simple_widget);
  bm_card_set_theme_scope(simple, state->root_scope);
  bm_card_set_accessible_label(simple, "Content-only card");
  GtkWidget *simple_content = new_stack(8);
  gtk_box_append(GTK_BOX(simple_content), new_label("Content only", "heading"));
  gtk_box_append(GTK_BOX(simple_content),
                 new_label("Empty card regions are omitted automatically.", "dim-label"));
  bm_card_set_child(simple, simple_content);
  gtk_box_append(GTK_BOX(content), simple_widget);

  GtkWidget *full_widget = bm_card_new();
  BmCard *full = BM_CARD(full_widget);
  bm_card_set_theme_scope(full, state->root_scope);
  bm_card_set_accessible_label(full, "Structured project card");
  bm_card_set_header(full, new_label("Release candidate", "heading"));
  GtkWidget *action = new_button(state, "Open", BM_BUTTON_VARIANT_GHOST,
                                 BM_BUTTON_TREATMENT_DEFAULT);
  bm_card_set_actions(full, action);
  GtkWidget *full_content = new_stack(6);
  gtk_box_append(GTK_BOX(full_content), new_label("Owner: Platform team", NULL));
  gtk_box_append(GTK_BOX(full_content), new_label("Status: ready for review", "dim-label"));
  bm_card_set_child(full, full_content);
  bm_card_set_footer(full, new_label("Updated a few seconds ago", "dim-label"));
  gtk_box_append(GTK_BOX(content), full_widget);

  return new_section(state,
                     "Card",
                     "Content-only and fully structured compositions with header, actions, body, footer, and accessible labels.",
                     content);
}

static void
demo_state_free(DemoState *state)
{
  g_clear_object(&state->nested_scope);
  g_clear_object(&state->root_scope);
  g_free(state);
}

static void
on_activate(GtkApplication *application, gpointer user_data)
{
  (void) user_data;
  g_autoptr(GError) error = NULL;
  if (!bm_init_for_display(gdk_display_get_default(), &error)) {
    g_critical("Could not load BoldMerge styles: %s", error->message);
    return;
  }

  DemoState *state = g_new0(DemoState, 1);
  state->root_scope = bm_theme_scope_new(NULL);
  state->nested_scope = bm_theme_scope_new(state->root_scope);
  bm_theme_scope_set_theme(state->root_scope, BM_THEME_LIGHT);
  bm_theme_scope_set_density(state->root_scope, BM_DENSITY_BALANCED);
  bm_theme_scope_set_accent(state->root_scope, BM_ACCENT_CYAN);
  bm_theme_scope_set_theme(state->nested_scope, BM_THEME_DARK);
  bm_theme_scope_set_density(state->nested_scope, BM_DENSITY_COMPACT);

  GtkWidget *window = adw_application_window_new(GTK_APPLICATION(application));
  gtk_window_set_title(GTK_WINDOW(window), "BoldMerge Native Component Catalog");
  gtk_window_set_default_size(GTK_WINDOW(window), 1040, 760);

  GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *header = adw_header_bar_new();
  GtkWidget *title = adw_window_title_new("BoldMerge Native", "Component catalog");
  adw_header_bar_set_title_widget(ADW_HEADER_BAR(header), title);

  state->theme_button = BM_BUTTON(new_button(state, "Theme", BM_BUTTON_VARIANT_GHOST,
                                              BM_BUTTON_TREATMENT_DEFAULT));
  state->density_button = BM_BUTTON(new_button(state, "Density", BM_BUTTON_VARIANT_GHOST,
                                                BM_BUTTON_TREATMENT_DEFAULT));
  state->accent_button = BM_BUTTON(new_button(state, "Accent", BM_BUTTON_VARIANT_GHOST,
                                               BM_BUTTON_TREATMENT_DEFAULT));
  g_signal_connect(state->theme_button, "clicked", G_CALLBACK(on_theme_clicked), state);
  g_signal_connect(state->density_button, "clicked", G_CALLBACK(on_density_clicked), state);
  g_signal_connect(state->accent_button, "clicked", G_CALLBACK(on_accent_clicked), state);
  adw_header_bar_pack_end(ADW_HEADER_BAR(header), GTK_WIDGET(state->accent_button));
  adw_header_bar_pack_end(ADW_HEADER_BAR(header), GTK_WIDGET(state->density_button));
  adw_header_bar_pack_end(ADW_HEADER_BAR(header), GTK_WIDGET(state->theme_button));
  gtk_box_append(GTK_BOX(layout), header);

  GtkWidget *catalog = new_stack(20);
  gtk_widget_add_css_class(catalog, "bm-canvas");
  gtk_widget_add_css_class(catalog, "bm-density-sample");
  bm_theme_scope_apply(state->root_scope, catalog);

  GtkWidget *intro = new_stack(6);
  gtk_box_append(GTK_BOX(intro), new_label("Native components", "title-1"));
  gtk_box_append(GTK_BOX(intro),
                 new_label("All controls currently implemented by @boldmerge/native. "
                           "Use the toolbar to inspect every example across theme, density, and accent settings.",
                           "dim-label"));
  state->status = GTK_LABEL(new_label(NULL, "heading"));
  gtk_box_append(GTK_BOX(intro), GTK_WIDGET(state->status));
  gtk_box_append(GTK_BOX(catalog), intro);
  gtk_box_append(GTK_BOX(catalog), create_foundation_section(state));
  gtk_box_append(GTK_BOX(catalog), create_button_section(state));
  gtk_box_append(GTK_BOX(catalog), create_input_section(state));
  gtk_box_append(GTK_BOX(catalog), create_selection_section(state));
  gtk_box_append(GTK_BOX(catalog), create_display_section(state));
  gtk_box_append(GTK_BOX(catalog), create_navigation_section(state));
  gtk_box_append(GTK_BOX(catalog), create_shell_section(state));
  gtk_box_append(GTK_BOX(catalog), create_overlay_section(state));
  gtk_box_append(GTK_BOX(catalog), create_modal_section(state));
  gtk_box_append(GTK_BOX(catalog), create_search_section(state));
  gtk_box_append(GTK_BOX(catalog), create_table_section(state));
  gtk_box_append(GTK_BOX(catalog), create_pagination_section(state));
  gtk_box_append(GTK_BOX(catalog), create_avatar_empty_section(state));
  gtk_box_append(GTK_BOX(catalog), create_metrics_section(state));
  gtk_box_append(GTK_BOX(catalog), create_chart_section(state));
  gtk_box_append(GTK_BOX(catalog), create_feedback_section(state));
  gtk_box_append(GTK_BOX(catalog), create_card_section(state));

  GtkWidget *footer = new_label("34 public control families · controls, shell, identity, data, charts, metrics, feedback, and validation", "dim-label");
  gtk_widget_set_margin_bottom(footer, 8);
  gtk_box_append(GTK_BOX(catalog), footer);

  GtkWidget *scroller = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), catalog);
  gtk_widget_set_vexpand(scroller, TRUE);
  gtk_box_append(GTK_BOX(layout), scroller);

  update_status(state);
  adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), layout);
  g_object_set_data_full(G_OBJECT(window), "demo-state", state,
                         (GDestroyNotify) demo_state_free);
  gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char **argv)
{
  g_autoptr(AdwApplication) application = adw_application_new("com.boldmerge.NativeDemo",
                                                               G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(application, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(application), argc, argv);
}
