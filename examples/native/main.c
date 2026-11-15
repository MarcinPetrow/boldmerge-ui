#include <boldmerge/boldmerge.h>

typedef struct {
  BmThemeScope *scope;
  BmThemeScope *nested_scope;
  GtkLabel *status;
  BmInput *validation_input;
  GtkLabel *validation_status;
  guint accent_index;
} DemoState;

static const BmAccent accents[] = {
  BM_ACCENT_CYAN, BM_ACCENT_BLUE, BM_ACCENT_VIOLET,
  BM_ACCENT_ORANGE, BM_ACCENT_GREEN, BM_ACCENT_CUSTOM,
};

static const char *accent_names[] = {
  "Cyan", "Blue", "Violet", "Orange", "Green", "Custom magenta",
};

static GtkWidget *
section_title(const char *title, const char *description)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  GtkWidget *heading = gtk_label_new(title);
  GtkWidget *copy = gtk_label_new(description);
  gtk_widget_add_css_class(heading, "title-2");
  gtk_widget_add_css_class(copy, "dim-label");
  gtk_label_set_xalign(GTK_LABEL(heading), 0.0f);
  gtk_label_set_xalign(GTK_LABEL(copy), 0.0f);
  gtk_label_set_wrap(GTK_LABEL(copy), TRUE);
  gtk_box_append(GTK_BOX(box), heading);
  gtk_box_append(GTK_BOX(box), copy);
  return box;
}

static GtkWidget *
flow_new(void)
{
  GtkWidget *flow = gtk_flow_box_new();
  gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flow), GTK_SELECTION_NONE);
  gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(flow), 8);
  gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flow), 8);
  gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 6);
  gtk_widget_set_halign(flow, GTK_ALIGN_START);
  return flow;
}

static void
flow_append(GtkWidget *flow, GtkWidget *child)
{
  gtk_flow_box_insert(GTK_FLOW_BOX(flow), child, -1);
}

static GtkWidget *
make_button(BmThemeScope *scope,
            const char *text,
            BmButtonVariant variant,
            BmButtonTreatment treatment)
{
  GtkWidget *button = bm_button_new(text);
  bm_button_set_variant(BM_BUTTON(button), variant);
  bm_button_set_treatment(BM_BUTTON(button), treatment);
  bm_button_set_theme_scope(BM_BUTTON(button), scope);
  return button;
}

static GtkWidget *
make_input(BmThemeScope *scope, const char *label)
{
  GtkWidget *input = bm_input_new(label);
  bm_input_set_theme_scope(BM_INPUT(input), scope);
  return input;
}

static void
set_status(DemoState *state)
{
  const char *theme = bm_theme_scope_get_effective_theme(state->scope) == BM_THEME_DARK
                        ? "Dark" : "Light";
  const char *density = bm_theme_scope_get_effective_density(state->scope) == BM_DENSITY_COMPACT
                          ? "Compact" : "Balanced";
  g_autofree char *text = g_strdup_printf("Root scope: %s / %s / %s",
                                          theme,
                                          density,
                                          accent_names[state->accent_index]);
  gtk_label_set_text(state->status, text);
}

static void
toggle_theme(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  BmTheme current = bm_theme_scope_get_effective_theme(state->scope);
  bm_theme_scope_set_theme(state->scope,
                           current == BM_THEME_DARK ? BM_THEME_LIGHT : BM_THEME_DARK);
  set_status(state);
}

static void
toggle_density(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  BmDensity current = bm_theme_scope_get_effective_density(state->scope);
  bm_theme_scope_set_density(state->scope,
                             current == BM_DENSITY_COMPACT
                               ? BM_DENSITY_BALANCED : BM_DENSITY_COMPACT);
  set_status(state);
}

static void
next_accent(GtkButton *button, gpointer user_data)
{
  (void) button;
  DemoState *state = user_data;
  state->accent_index = (state->accent_index + 1) % G_N_ELEMENTS(accents);

  if (accents[state->accent_index] == BM_ACCENT_CUSTOM) {
    GdkRGBA seed;
    g_autoptr(GError) error = NULL;
    gdk_rgba_parse(&seed, "#c026d3");
    if (!bm_theme_scope_set_custom_accent(state->scope, &seed, &error)) {
      g_warning("Custom accent rejected: %s", error->message);
      state->accent_index = 0;
      bm_theme_scope_set_accent(state->scope, BM_ACCENT_CYAN);
    }
  } else {
    bm_theme_scope_set_accent(state->scope, accents[state->accent_index]);
  }
  set_status(state);
}

static void
validate_workspace(GtkWidget *widget, gpointer user_data)
{
  (void) widget;
  DemoState *state = user_data;
  gboolean valid = bm_input_validate(state->validation_input);
  gtk_label_set_text(state->validation_status,
                     valid ? "Workspace is ready to create."
                           : "Correct the required workspace name.");
  if (!valid)
    gtk_widget_grab_focus(GTK_WIDGET(bm_input_get_entry(state->validation_input)));
}

static GtkWidget *
build_accent_gallery(BmThemeScope *parent)
{
  GtkWidget *flow = flow_new();
  static const BmAccent presets[] = {
    BM_ACCENT_CYAN, BM_ACCENT_BLUE, BM_ACCENT_VIOLET,
    BM_ACCENT_ORANGE, BM_ACCENT_GREEN,
  };

  for (guint index = 0; index < G_N_ELEMENTS(presets); index++) {
    BmThemeScope *scope = bm_theme_scope_new(parent);
    bm_theme_scope_set_accent(scope, presets[index]);
    GtkWidget *button = make_button(scope,
                                    accent_names[index],
                                    BM_BUTTON_VARIANT_PRIMARY,
                                    BM_BUTTON_TREATMENT_DEFAULT);
    g_object_set_data_full(G_OBJECT(button), "accent-scope", scope, g_object_unref);
    flow_append(flow, button);
  }
  return flow;
}

static GtkWidget *
build_button_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  GtkWidget *variants = flow_new();
  flow_append(variants, make_button(state->scope, "Primary", BM_BUTTON_VARIANT_PRIMARY, BM_BUTTON_TREATMENT_DEFAULT));
  flow_append(variants, make_button(state->scope, "Secondary", BM_BUTTON_VARIANT_SECONDARY, BM_BUTTON_TREATMENT_DEFAULT));
  flow_append(variants, make_button(state->scope, "Outline", BM_BUTTON_VARIANT_OUTLINE, BM_BUTTON_TREATMENT_DEFAULT));
  flow_append(variants, make_button(state->scope, "Ghost", BM_BUTTON_VARIANT_GHOST, BM_BUTTON_TREATMENT_DEFAULT));

  GtkWidget *states = flow_new();
  flow_append(states, make_button(state->scope, "Delete", BM_BUTTON_VARIANT_PRIMARY, BM_BUTTON_TREATMENT_DESTRUCTIVE));
  flow_append(states, make_button(state->scope, "Remove", BM_BUTTON_VARIANT_SECONDARY, BM_BUTTON_TREATMENT_DESTRUCTIVE));
  flow_append(states, make_button(state->scope, "Delete outline", BM_BUTTON_VARIANT_OUTLINE, BM_BUTTON_TREATMENT_DESTRUCTIVE));
  flow_append(states, make_button(state->scope, "Discard", BM_BUTTON_VARIANT_GHOST, BM_BUTTON_TREATMENT_DESTRUCTIVE));
  GtkWidget *loading = make_button(state->scope, "Saving", BM_BUTTON_VARIANT_PRIMARY, BM_BUTTON_TREATMENT_DEFAULT);
  bm_button_set_loading(BM_BUTTON(loading), TRUE);
  flow_append(states, loading);
  GtkWidget *disabled = make_button(state->scope, "Disabled", BM_BUTTON_VARIANT_SECONDARY, BM_BUTTON_TREATMENT_DEFAULT);
  gtk_widget_set_sensitive(disabled, FALSE);
  flow_append(states, disabled);
  GtkWidget *custom = make_button(state->scope, NULL, BM_BUTTON_VARIANT_OUTLINE, BM_BUTTON_TREATMENT_DEFAULT);
  GtkWidget *custom_content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(custom_content), gtk_image_new_from_icon_name("document-save-symbolic"));
  gtk_box_append(GTK_BOX(custom_content), gtk_label_new("Custom content"));
  bm_button_set_content(BM_BUTTON(custom), custom_content);
  gtk_accessible_update_property(GTK_ACCESSIBLE(custom),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, "Save with custom content",
                                 -1);
  flow_append(states, custom);

  gtk_box_append(GTK_BOX(content), variants);
  gtk_box_append(GTK_BOX(content), states);
  return content;
}

static GtkWidget *
build_input_catalog(DemoState *state)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
  gtk_widget_set_hexpand(grid, TRUE);

  GtkWidget *empty = make_input(state->scope, "Empty field");
  bm_input_set_placeholder_text(BM_INPUT(empty), "Type a value");
  GtkWidget *filled = make_input(state->scope, "Filled field");
  bm_input_set_text(BM_INPUT(filled), "Northstar");
  GtkWidget *readonly = make_input(state->scope, "Read only");
  bm_input_set_text(BM_INPUT(readonly), "BM-1042");
  bm_input_set_editable(BM_INPUT(readonly), FALSE);
  GtkWidget *invalid = make_input(state->scope, "Server address");
  bm_input_set_text(BM_INPUT(invalid), "invalid address");
  bm_input_set_error(BM_INPUT(invalid), "Enter a valid server address.");
  GtkWidget *adorned = make_input(state->scope, "Search");
  bm_input_set_leading_widget(BM_INPUT(adorned), gtk_label_new("⌕"));
  bm_input_set_trailing_widget(BM_INPUT(adorned), gtk_label_new("⌘K"));

  gtk_grid_attach(GTK_GRID(grid), empty, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), filled, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), readonly, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), invalid, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), adorned, 0, 2, 2, 1);
  return grid;
}

static GtkWidget *
build_navigation_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);

  BmBreadcrumbs *breadcrumbs = BM_BREADCRUMBS(
    bm_breadcrumbs_new("Project breadcrumbs"));
  bm_breadcrumbs_set_theme_scope(breadcrumbs, state->scope);
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(
    bm_breadcrumb_item_new("Projects", "https://example.test/projects", FALSE)));
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(
    bm_breadcrumb_item_new("Northstar", "https://example.test/projects/northstar", FALSE)));
  bm_breadcrumbs_append(breadcrumbs, BM_BREADCRUMB_ITEM(
    bm_breadcrumb_item_new("Overview", NULL, TRUE)));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(breadcrumbs));

  BmSegmentedControl *segments = BM_SEGMENTED_CONTROL(
    bm_segmented_control_new("View mode"));
  bm_segmented_control_set_theme_scope(segments, state->scope);
  bm_segmented_control_append(segments, "List", "list");
  bm_segmented_control_append(segments, "Board", "board");
  bm_segmented_control_append(segments, "Timeline", "timeline");
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(segments));

  BmTabs *tabs = BM_TABS(bm_tabs_new("Project views"));
  bm_tabs_set_theme_scope(tabs, state->scope);
  bm_tabs_append(tabs, "overview", "Overview", gtk_label_new("Overview panel content"));
  bm_tabs_append(tabs, "activity", "Activity", gtk_label_new("Activity panel content"));
  bm_tabs_append(tabs, "settings", "Settings", gtk_label_new("Settings panel content"));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(tabs));

  BmPageLayout *layout = BM_PAGE_LAYOUT(bm_page_layout_new());
  bm_page_layout_set_theme_scope(layout, state->scope);
  bm_page_layout_set_mode(layout, BM_PAGE_LAYOUT_SPLIT);
  bm_page_layout_set_header(layout, section_title("Split page layout", "Header spans content and aside."));
  BmSection *section = BM_SECTION(bm_section_new("Deployment overview"));
  bm_section_set_theme_scope(section, state->scope);
  bm_section_set_header(section, gtk_label_new("Primary content"));
  bm_section_set_child(section, gtk_label_new("Production is healthy."));
  bm_section_set_footer(section, gtk_label_new("Updated now"));
  bm_section_set_separator(section, TRUE);
  bm_page_layout_set_child(layout, GTK_WIDGET(section));
  bm_page_layout_set_aside(layout, gtk_label_new("Supporting content (320px proposal)"));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(layout));
  return content;
}

static GtkWidget *
build_shell_catalog(DemoState *state)
{
  BmSidebar *sidebar = BM_SIDEBAR(bm_sidebar_new("Workspace navigation"));
  bm_sidebar_set_theme_scope(sidebar, state->scope);
  GtkWidget *brand = gtk_label_new("BoldMerge");
  gtk_widget_add_css_class(brand, "title-3");
  gtk_label_set_xalign(GTK_LABEL(brand), 0.0f);
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
  bm_topbar_set_theme_scope(topbar, state->scope);
  GtkWidget *topbar_title = gtk_label_new("Workspace overview");
  gtk_widget_add_css_class(topbar_title, "title-3");
  bm_topbar_set_start(topbar, topbar_title);
  bm_topbar_set_search(topbar, gtk_search_entry_new());
  GtkWidget *notifications = bm_icon_button_new(
    "Notifications", gtk_image_new_from_icon_name("preferences-system-notifications-symbolic"));
  bm_icon_button_set_theme_scope(BM_ICON_BUTTON(notifications), state->scope);
  bm_topbar_set_notifications(topbar, notifications);
  bm_topbar_set_account(topbar,
    make_button(state->scope, "MK", BM_BUTTON_VARIANT_GHOST,
                BM_BUTTON_TREATMENT_DEFAULT));

  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_margin_top(content, 32);
  gtk_widget_set_margin_start(content, 32);
  gtk_widget_set_margin_end(content, 32);
  GtkWidget *content_title = gtk_label_new("Application content");
  gtk_widget_add_css_class(content_title, "title-2");
  gtk_label_set_xalign(GTK_LABEL(content_title), 0.0f);
  gtk_box_append(GTK_BOX(content), content_title);
  gtk_box_append(GTK_BOX(content),
                 gtk_label_new("Sidebar, Topbar, and content use native GTK composition."));

  BmAppShell *shell = BM_APP_SHELL(bm_app_shell_new());
  bm_app_shell_set_theme_scope(shell, state->scope);
  bm_app_shell_set_sidebar(shell, sidebar);
  bm_app_shell_set_topbar(shell, topbar);
  bm_app_shell_set_child(shell, content);
  gtk_widget_set_size_request(GTK_WIDGET(shell), 760, 400);
  return GTK_WIDGET(shell);
}

static GtkWidget *
build_overlay_catalog(DemoState *state)
{
  GtkWidget *content = flow_new();
  GtkWidget *tooltip_button = make_button(
    state->scope, "Create project", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  BmTooltip *tooltip = BM_TOOLTIP(
    bm_tooltip_new(tooltip_button, "Create a new project"));
  bm_tooltip_set_theme_scope(tooltip, state->scope);
  flow_append(content, GTK_WIDGET(tooltip));

  BmDropdown *dropdown = BM_DROPDOWN(bm_dropdown_new("Project actions"));
  bm_dropdown_set_theme_scope(dropdown, state->scope);
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
  flow_append(content, GTK_WIDGET(dropdown));
  return content;
}

static void
destroy_catalog_dialog(gpointer data)
{
  gtk_window_destroy(GTK_WINDOW(data));
  g_object_unref(data);
}

static void
open_catalog_dialog(GtkButton *button, gpointer data)
{
  DemoState *state = data;
  BmDialog *dialog = g_object_get_data(G_OBJECT(button), "catalog-dialog");
  if (dialog == NULL) {
    dialog = BM_DIALOG(g_object_ref_sink(bm_dialog_new("Create workspace")));
    bm_dialog_set_theme_scope(dialog, state->scope);
    GtkWidget *heading = gtk_label_new("Create workspace");
    gtk_widget_add_css_class(heading, "title-2");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0f);
    bm_dialog_set_header(dialog, heading);
    bm_dialog_set_child(dialog,
      gtk_label_new("Review the workspace settings before continuing."));
    bm_dialog_set_footer(dialog,
      gtk_label_new("Native GTK modal focus and Escape behavior."));
    bm_dialog_add_action(dialog, "Cancel", "cancel", FALSE, FALSE);
    bm_dialog_add_action(dialog, "Create", "create", TRUE, FALSE);
    g_object_set_data_full(G_OBJECT(button), "catalog-dialog", dialog,
                           destroy_catalog_dialog);
  }
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
  bm_dialog_present(dialog, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
                    GTK_WIDGET(button), NULL);
}

static void
open_catalog_drawer(GtkButton *button, gpointer data)
{
  bm_drawer_open(BM_DRAWER(data), GTK_WIDGET(button), NULL);
}

static void
close_catalog_drawer(GtkButton *button, gpointer data)
{
  (void) button;
  bm_drawer_close(BM_DRAWER(data), BM_CLOSE_REASON_ACTION, "close");
}

static GtkWidget *
build_modal_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
  GtkWidget *open_dialog = make_button(
    state->scope, "Open dialog", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(open_dialog, "clicked", G_CALLBACK(open_catalog_dialog), state);
  gtk_widget_set_halign(open_dialog, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), open_dialog);

  BmDrawer *drawer = BM_DRAWER(bm_drawer_new("Project details"));
  bm_drawer_set_theme_scope(drawer, state->scope);
  bm_drawer_set_mode(drawer, BM_DRAWER_MODE_PUSH);
  GtkWidget *workspace = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *workspace_heading = gtk_label_new("Workspace content");
  gtk_widget_add_css_class(workspace_heading, "title-3");
  gtk_label_set_xalign(GTK_LABEL(workspace_heading), 0.0f);
  GtkWidget *open_drawer = make_button(
    state->scope, "Open details", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(open_drawer, "clicked", G_CALLBACK(open_catalog_drawer), drawer);
  gtk_box_append(GTK_BOX(workspace), workspace_heading);
  gtk_box_append(GTK_BOX(workspace), open_drawer);
  bm_drawer_set_content(drawer, workspace);
  GtkWidget *panel_heading = gtk_label_new("Project details");
  gtk_widget_add_css_class(panel_heading, "title-3");
  gtk_label_set_xalign(GTK_LABEL(panel_heading), 0.0f);
  bm_drawer_set_header(drawer, panel_heading);
  bm_drawer_set_child(drawer,
    gtk_label_new("Owner: Platform team\nStatus: Ready for review"));
  GtkWidget *close_drawer = make_button(
    state->scope, "Close", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(close_drawer, "clicked", G_CALLBACK(close_catalog_drawer), drawer);
  bm_drawer_set_actions(drawer, close_drawer);
  gtk_widget_set_size_request(GTK_WIDGET(drawer), 760, 360);
  bm_drawer_open(drawer, NULL, NULL);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(drawer));
  return content;
}

static void
open_catalog_command_palette(GtkButton *button, gpointer data)
{
  DemoState *state = data;
  BmCommandPalette *palette = g_object_get_data(
    G_OBJECT(button), "catalog-command-palette");
  if (palette == NULL) {
    palette = BM_COMMAND_PALETTE(g_object_ref_sink(
      bm_command_palette_new("Workspace commands")));
    bm_command_palette_set_theme_scope(palette, state->scope);
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
    g_object_set_data_full(G_OBJECT(button), "catalog-command-palette", palette,
                           destroy_catalog_dialog);
  }
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
  bm_command_palette_present(
    palette, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
    GTK_WIDGET(button));
}

static GtkWidget *
build_search_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  BmCombobox *region = BM_COMBOBOX(bm_combobox_new("Searchable region"));
  bm_combobox_append(region, "Europe", "eu");
  bm_combobox_append(region, "United States", "us");
  bm_combobox_append(region, "Asia Pacific", "apac");
  bm_combobox_set_value(region, "eu");
  bm_combobox_set_required(region, TRUE);
  bm_combobox_set_theme_scope(region, state->scope);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(region));
  GtkWidget *open = make_button(
    state->scope, "Open command palette", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  gtk_widget_set_halign(open, GTK_ALIGN_START);
  g_signal_connect(open, "clicked",
                   G_CALLBACK(open_catalog_command_palette), state);
  gtk_box_append(GTK_BOX(content), open);
  return content;
}

static GtkWidget *
build_pagination_catalog(DemoState *state)
{
  BmPagination *pagination = BM_PAGINATION(
    bm_pagination_new("Project pages"));
  bm_pagination_set_total_items(pagination, 236);
  bm_pagination_set_page(pagination, 4);
  bm_pagination_set_theme_scope(pagination, state->scope);
  return GTK_WIDGET(pagination);
}

static void
catalog_table_setup(GtkSignalListItemFactory *factory, GtkListItem *item,
                    gpointer data)
{
  (void) factory; (void) data;
  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_list_item_set_child(item, label);
}

static void
catalog_table_bind(GtkSignalListItemFactory *factory, GtkListItem *item,
                   gpointer data)
{
  (void) factory; (void) data;
  gtk_label_set_text(GTK_LABEL(gtk_list_item_get_child(item)),
    gtk_string_object_get_string(
      GTK_STRING_OBJECT(gtk_list_item_get_item(item))));
}

static GtkWidget *
build_table_catalog(DemoState *state)
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
  g_signal_connect(factory, "setup", G_CALLBACK(catalog_table_setup), NULL);
  g_signal_connect(factory, "bind", G_CALLBACK(catalog_table_bind), NULL);
  bm_table_append_column(table, "Project · Status · Monthly spend", "name",
                         factory, TRUE);
  bm_table_set_row_activation(table, TRUE);
  bm_table_set_theme_scope(table, state->scope);
  gtk_widget_set_size_request(GTK_WIDGET(table), -1, 220);
  return GTK_WIDGET(table);
}

static GtkWidget *
build_avatar_empty_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  BmAvatarGroup *group = BM_AVATAR_GROUP(
    bm_avatar_group_new("Platform team"));
  bm_avatar_group_set_theme_scope(group, state->scope);
  const char *names[] = { "Ada Lovelace", "Grace Hopper", "Alan Turing" };
  for (guint i = 0; i < G_N_ELEMENTS(names); i++) {
    BmAvatar *avatar = BM_AVATAR(bm_avatar_new(names[i]));
    bm_avatar_set_theme_scope(avatar, state->scope);
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
  bm_empty_state_set_action(empty, make_button(
    state->scope, "Create project", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT));
  bm_empty_state_set_theme_scope(empty, state->scope);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(empty));
  return content;
}
static GtkWidget *build_metrics_catalog(DemoState*state){GtkWidget*c=gtk_box_new(GTK_ORIENTATION_VERTICAL,16);BmSkeleton*sk=BM_SKELETON(bm_skeleton_new(BM_SKELETON_RECTANGLE,"Loading metrics"));bm_skeleton_set_theme_scope(sk,state->scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(sk));BmKpi*k=BM_KPI(bm_kpi_new("Monthly recurring revenue","$128,400"));double v[]={72,74,73,79,82,86,91};bm_kpi_set_change(k,"+8.2%",BM_KPI_CHANGE_FAVORABLE);bm_kpi_set_comparison(k,"versus last month");bm_kpi_set_sparkline(k,v,G_N_ELEMENTS(v));bm_kpi_set_theme_scope(k,state->scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(k));BmProgress*p=BM_PROGRESS(bm_progress_new("Storage used"));bm_progress_set_value(p,37);bm_progress_set_value_label(p,"37 GB of 100 GB");bm_progress_set_theme_scope(p,state->scope);gtk_box_append(GTK_BOX(c),GTK_WIDGET(p));return c;}

static GtkWidget *
build_chart_catalog(DemoState *state)
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
  bm_chart_set_theme_scope(chart, state->scope);

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
  return GTK_WIDGET(chart);
}

static void
push_catalog_toast(GtkButton *button, gpointer data)
{
  (void) button;
  BmToast *toast = bm_toast_region_push(
    BM_TOAST_REGION(data), "Saved", "Workspace settings updated.",
    BM_TOAST_TONE_SUCCESS, 5);
  g_object_unref(toast);
}

static void
validate_catalog_summary(GtkButton *button, gpointer data)
{
  (void) button;
  bm_validation_summary_validate(BM_VALIDATION_SUMMARY(data),
                                 BM_VALIDATION_FOCUS_FIRST);
}

static GtkWidget *
build_feedback_catalog(DemoState *state)
{
  BmToastRegion *region = BM_TOAST_REGION(
    bm_toast_region_new("Notifications"));
  bm_toast_region_set_theme_scope(region, state->scope);
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  GtkWidget *notify = make_button(
    state->scope, "Show success toast", BM_BUTTON_VARIANT_OUTLINE,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(notify, "clicked", G_CALLBACK(push_catalog_toast), region);
  gtk_widget_set_halign(notify, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), notify);

  BmValidationSummary *summary = BM_VALIDATION_SUMMARY(
    bm_validation_summary_new("Correct the following errors"));
  bm_validation_summary_set_theme_scope(summary, state->scope);
  BmInput *name = BM_INPUT(bm_input_new("Workspace name"));
  bm_input_set_required(name, TRUE);
  bm_input_set_theme_scope(name, state->scope);
  BmCheckbox *terms = BM_CHECKBOX(bm_checkbox_new("Accept terms"));
  bm_checkbox_set_required(terms, TRUE);
  bm_checkbox_set_theme_scope(terms, state->scope);
  bm_validation_summary_add(summary, GTK_WIDGET(name), NULL, NULL);
  bm_validation_summary_add(summary, GTK_WIDGET(terms), NULL, NULL);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(summary));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(name));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(terms));
  GtkWidget *validate = make_button(
    state->scope, "Validate form", BM_BUTTON_VARIANT_PRIMARY,
    BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(validate, "clicked", G_CALLBACK(validate_catalog_summary), summary);
  gtk_widget_set_halign(validate, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(content), validate);
  bm_validation_summary_validate(summary, BM_VALIDATION_FOCUS_NONE);

  bm_toast_region_set_child(region, content);
  BmToast *initial = bm_toast_region_push(
    region, "Native feedback", "Toast queue is owned by this region.",
    BM_TOAST_TONE_NEUTRAL, 0);
  g_object_unref(initial);
  return GTK_WIDGET(region);
}

static GtkWidget *
build_compact_catalog(DemoState *state)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  BmSelect *select = BM_SELECT(bm_select_new("Region"));
  bm_select_append(select, "Europe", "eu");
  bm_select_append(select, "United States", "us");
  bm_select_append(select, "Asia Pacific", "apac");
  bm_select_set_value(select, "eu");
  bm_select_set_required(select, TRUE);
  bm_select_set_theme_scope(select, state->scope);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(select));

  GtkWidget *icons = flow_new();
  GtkWidget *add = bm_icon_button_new("Add", gtk_image_new_from_icon_name("list-add-symbolic"));
  bm_icon_button_set_theme_scope(BM_ICON_BUTTON(add), state->scope);
  flow_append(icons, add);
  GtkWidget *save = bm_icon_button_new("Save", gtk_image_new_from_icon_name("document-save-symbolic"));
  bm_icon_button_set_variant(BM_ICON_BUTTON(save), BM_BUTTON_VARIANT_OUTLINE);
  bm_icon_button_set_theme_scope(BM_ICON_BUTTON(save), state->scope);
  flow_append(icons, save);
  GtkWidget *remove = bm_icon_button_new("Delete", gtk_image_new_from_icon_name("edit-delete-symbolic"));
  bm_icon_button_set_variant(BM_ICON_BUTTON(remove), BM_BUTTON_VARIANT_OUTLINE);
  bm_icon_button_set_treatment(BM_ICON_BUTTON(remove), BM_BUTTON_TREATMENT_DESTRUCTIVE);
  bm_icon_button_set_theme_scope(BM_ICON_BUTTON(remove), state->scope);
  flow_append(icons, remove);
  gtk_box_append(GTK_BOX(content), icons);

  GtkWidget *labels = flow_new();
  GtkWidget *tag = bm_tag_new("Featured");
  bm_tag_set_prefix(BM_TAG(tag), gtk_image_new_from_icon_name("starred-symbolic"));
  bm_tag_set_theme_scope(BM_TAG(tag), state->scope);
  flow_append(labels, tag);
  static const char *texts[] = { "Neutral", "Active", "Operational", "Delayed", "Failed" };
  for (guint tone = BM_STATUS_TONE_NEUTRAL; tone <= BM_STATUS_TONE_ERROR; tone++) {
    GtkWidget *status = bm_status_new(texts[tone], tone);
    bm_status_set_theme_scope(BM_STATUS(status), state->scope);
    flow_append(labels, status);
  }
  gtk_box_append(GTK_BOX(content), labels);
  return content;
}

static GtkWidget *
build_selection_catalog(DemoState *state)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 24);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
  gtk_widget_set_hexpand(grid, TRUE);

  GtkWidget *checks = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *unchecked = bm_checkbox_new("Unchecked");
  bm_checkbox_set_theme_scope(BM_CHECKBOX(unchecked), state->scope);
  gtk_box_append(GTK_BOX(checks), unchecked);
  GtkWidget *checked = bm_checkbox_new("Checked");
  bm_checkbox_set_checked(BM_CHECKBOX(checked), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(checked), state->scope);
  gtk_box_append(GTK_BOX(checks), checked);
  GtkWidget *mixed = bm_checkbox_new("Indeterminate");
  bm_checkbox_set_indeterminate(BM_CHECKBOX(mixed), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(mixed), state->scope);
  gtk_box_append(GTK_BOX(checks), mixed);
  GtkWidget *required = bm_checkbox_new("Required and invalid");
  bm_checkbox_set_required(BM_CHECKBOX(required), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(required), state->scope);
  gtk_box_append(GTK_BOX(checks), required);
  gtk_grid_attach(GTK_GRID(grid), checks, 0, 0, 1, 1);

  BmRadioGroup *group = BM_RADIO_GROUP(bm_radio_group_new("Deployment channel"));
  bm_radio_group_set_required(group, TRUE);
  bm_radio_group_set_theme_scope(group, state->scope);
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Stable", "stable")));
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Preview", "preview")));
  bm_radio_group_append(group, BM_RADIO(bm_radio_new("Nightly", "nightly")));
  bm_radio_group_set_value(group, "stable");
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(group), 1, 0, 1, 1);

  GtkWidget *switches = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *switch_on = bm_switch_new("Email notifications");
  bm_switch_set_active(BM_SWITCH(switch_on), TRUE);
  bm_switch_set_theme_scope(BM_SWITCH(switch_on), state->scope);
  gtk_box_append(GTK_BOX(switches), switch_on);
  GtkWidget *switch_off = bm_switch_new("Automatic updates");
  bm_switch_set_theme_scope(BM_SWITCH(switch_off), state->scope);
  gtk_box_append(GTK_BOX(switches), switch_off);
  GtkWidget *switch_disabled = bm_switch_new("Unavailable setting");
  bm_switch_set_theme_scope(BM_SWITCH(switch_disabled), state->scope);
  gtk_widget_set_sensitive(switch_disabled, FALSE);
  gtk_box_append(GTK_BOX(switches), switch_disabled);
  gtk_grid_attach(GTK_GRID(grid), switches, 2, 0, 1, 1);

  return grid;
}

static GtkWidget *
build_workspace_card(DemoState *state)
{
  GtkWidget *card_widget = bm_card_new();
  BmCard *card = BM_CARD(card_widget);
  bm_card_set_theme_scope(card, state->scope);
  bm_card_set_accessible_label(card, "Workspace validation example");

  GtkWidget *header = gtk_label_new("Create workspace");
  gtk_widget_add_css_class(header, "title-3");
  gtk_label_set_xalign(GTK_LABEL(header), 0.0f);
  bm_card_set_header(card, header);

  GtkWidget *help = make_button(state->scope, "Help", BM_BUTTON_VARIANT_GHOST, BM_BUTTON_TREATMENT_DEFAULT);
  bm_card_set_actions(card, help);

  GtkWidget *form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  GtkWidget *input = make_input(state->scope, "Workspace name");
  state->validation_input = BM_INPUT(input);
  bm_input_set_required(state->validation_input, TRUE);
  bm_input_set_required_message(state->validation_input, "Workspace name is required.");
  g_signal_connect(input, "activate", G_CALLBACK(validate_workspace), state);
  gtk_box_append(GTK_BOX(form), input);

  GtkWidget *actions = flow_new();
  GtkWidget *create = make_button(state->scope, "Create workspace", BM_BUTTON_VARIANT_PRIMARY, BM_BUTTON_TREATMENT_DEFAULT);
  GtkWidget *cancel = make_button(state->scope, "Cancel", BM_BUTTON_VARIANT_OUTLINE, BM_BUTTON_TREATMENT_DEFAULT);
  g_signal_connect(create, "clicked", G_CALLBACK(validate_workspace), state);
  flow_append(actions, create);
  flow_append(actions, cancel);
  gtk_box_append(GTK_BOX(form), actions);
  bm_card_set_child(card, form);

  state->validation_status = GTK_LABEL(gtk_label_new("Validation uses the native BmInput contract."));
  gtk_label_set_xalign(state->validation_status, 0.0f);
  gtk_widget_add_css_class(GTK_WIDGET(state->validation_status), "dim-label");
  bm_card_set_footer(card, GTK_WIDGET(state->validation_status));
  return card_widget;
}

static GtkWidget *
build_nested_card(DemoState *state)
{
  GtkWidget *card_widget = bm_card_new();
  BmCard *card = BM_CARD(card_widget);
  bm_card_set_theme_scope(card, state->nested_scope);
  bm_card_set_accessible_label(card, "Nested theme example");

  GtkWidget *header = gtk_label_new("Local Dark / Compact / Violet override");
  gtk_widget_add_css_class(header, "title-3");
  gtk_label_set_xalign(GTK_LABEL(header), 0.0f);
  bm_card_set_header(card, header);

  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  GtkWidget *copy = gtk_label_new("This scope remains explicit and independent of window size and root controls.");
  gtk_label_set_wrap(GTK_LABEL(copy), TRUE);
  gtk_label_set_xalign(GTK_LABEL(copy), 0.0f);
  gtk_box_append(GTK_BOX(content), copy);
  GtkWidget *input = make_input(state->nested_scope, "Nested input");
  bm_input_set_text(BM_INPUT(input), "Compact value");
  gtk_box_append(GTK_BOX(content), input);
  gtk_box_append(GTK_BOX(content), make_button(state->nested_scope, "Nested action", BM_BUTTON_VARIANT_PRIMARY, BM_BUTTON_TREATMENT_DEFAULT));
  bm_card_set_child(card, content);
  return card_widget;
}

static void
state_free(DemoState *state)
{
  g_clear_object(&state->nested_scope);
  g_clear_object(&state->scope);
  g_free(state);
}

static void
activate(GtkApplication *application, gpointer user_data)
{
  (void) user_data;
  g_autoptr(GError) error = NULL;
  if (!bm_init_for_display(gdk_display_get_default(), &error)) {
    g_critical("BoldMerge initialization failed: %s", error->message);
    return;
  }

  DemoState *state = g_new0(DemoState, 1);
  state->scope = bm_theme_scope_new(NULL);
  state->nested_scope = bm_theme_scope_new(state->scope);
  bm_theme_scope_set_theme(state->scope, BM_THEME_LIGHT);
  bm_theme_scope_set_density(state->scope, BM_DENSITY_BALANCED);
  bm_theme_scope_set_accent(state->scope, BM_ACCENT_CYAN);
  bm_theme_scope_set_theme(state->nested_scope, BM_THEME_DARK);
  bm_theme_scope_set_density(state->nested_scope, BM_DENSITY_COMPACT);
  bm_theme_scope_set_accent(state->nested_scope, BM_ACCENT_VIOLET);

  GtkWidget *window = adw_application_window_new(GTK_APPLICATION(application));
  gtk_window_set_title(GTK_WINDOW(window), "BoldMerge Ubuntu Native Catalog");
  gtk_window_set_default_size(GTK_WINDOW(window), 1040, 760);

  GtkWidget *scroller = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 28);
  gtk_widget_set_margin_top(page, 32);
  gtk_widget_set_margin_bottom(page, 48);
  gtk_widget_set_margin_start(page, 32);
  gtk_widget_set_margin_end(page, 32);
  gtk_widget_set_size_request(page, 720, -1);
  bm_theme_scope_apply(state->scope, page);

  GtkWidget *title = gtk_label_new("BoldMerge Ubuntu Native");
  gtk_widget_add_css_class(title, "title-1");
  gtk_label_set_xalign(GTK_LABEL(title), 0.0f);
  gtk_box_append(GTK_BOX(page), title);
  GtkWidget *intro = gtk_label_new("Current Native scope: thirty-four control families through identity, native data views, charts, metrics, command filtering, and feedback.");
  gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
  gtk_label_set_xalign(GTK_LABEL(intro), 0.0f);
  gtk_widget_add_css_class(intro, "dim-label");
  gtk_box_append(GTK_BOX(page), intro);

  state->status = GTK_LABEL(gtk_label_new(NULL));
  gtk_label_set_xalign(state->status, 0.0f);
  gtk_box_append(GTK_BOX(page), GTK_WIDGET(state->status));

  GtkWidget *controls = flow_new();
  GtkWidget *theme = gtk_button_new_with_label("Toggle theme");
  GtkWidget *density = gtk_button_new_with_label("Toggle density");
  GtkWidget *accent = gtk_button_new_with_label("Next accent");
  g_signal_connect(theme, "clicked", G_CALLBACK(toggle_theme), state);
  g_signal_connect(density, "clicked", G_CALLBACK(toggle_density), state);
  g_signal_connect(accent, "clicked", G_CALLBACK(next_accent), state);
  flow_append(controls, theme);
  flow_append(controls, density);
  flow_append(controls, accent);
  gtk_box_append(GTK_BOX(page), controls);

  gtk_box_append(GTK_BOX(page), section_title("Accent presets", "Five canonical presets; root controls additionally cycle through deterministic custom magenta."));
  gtk_box_append(GTK_BOX(page), build_accent_gallery(state->scope));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Button", "All Native variants and destructive combinations, plus loading, disabled, and custom-content states."));
  gtk_box_append(GTK_BOX(page), build_button_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Input", "Empty, filled, read-only, explicit error, and leading/trailing composition states."));
  gtk_box_append(GTK_BOX(page), build_input_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Selection controls", "Native Checkbox, Radio Group/Radio, and Switch states with GTK keyboard and accessibility behavior."));
  gtk_box_append(GTK_BOX(page), build_selection_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Select and compact display", "Native Select and Icon Button plus static Tag and Status components."));
  gtk_box_append(GTK_BOX(page), build_compact_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Layout and navigation", "Page Layout, Section, Breadcrumbs, Tabs, and Segmented Control use native GTK composition and selection."));
  gtk_box_append(GTK_BOX(page), build_navigation_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Application shell", "Sidebar, Topbar, and App Shell with explicit desktop/mobile policy."));
  gtk_box_append(GTK_BOX(page), build_shell_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Tooltip and Dropdown Menu", "GTK-native tooltip timing/placement and a non-modal popover menu."));
  gtk_box_append(GTK_BOX(page), build_overlay_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Dialog and Drawer", "Modal GtkWindow dialog plus explicit Overlay/Push drawer modes."));
  gtk_box_append(GTK_BOX(page), build_modal_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Combobox and Command Palette", "Searchable native selection and grouped command filtering."));
  gtk_box_append(GTK_BOX(page), build_search_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Table", "Consumer-owned GTK model, native selection, row activation, and application-controlled sorting."));
  gtk_box_append(GTK_BOX(page), build_table_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Pagination", "Application-owned paging with a bounded native page range."));
  gtk_box_append(GTK_BOX(page), build_pagination_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Avatar and Empty State", "Deterministic identity fallbacks and compositional absence feedback."));
  gtk_box_append(GTK_BOX(page), build_avatar_empty_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Skeleton, KPI, and Segmented Progress", "Loading, textual metrics, and exact progress semantics."));
  gtk_box_append(GTK_BOX(page), build_metrics_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Chart and Chart Tooltip", "Programmatic multi-series data, pointer and keyboard inspection, plus summary and durable table alternative."));
  gtk_box_append(GTK_BOX(page), build_chart_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Toast and Validation Summary", "Local AdwToast queue and native control validity aggregation."));
  gtk_box_append(GTK_BOX(page), build_feedback_catalog(state));
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(page), section_title("Card and validation", "Open GTK composition with header, actions, content, footer, and required-value validation."));

  GtkWidget *cards = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(cards), 16);
  gtk_grid_set_row_spacing(GTK_GRID(cards), 16);
  gtk_grid_attach(GTK_GRID(cards), build_workspace_card(state), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(cards), build_nested_card(state), 1, 0, 1, 1);
  gtk_box_append(GTK_BOX(page), cards);

  set_status(state);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), page);
  adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), scroller);
  g_object_set_data_full(G_OBJECT(window), "demo-state", state, (GDestroyNotify) state_free);
  gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char **argv)
{
  g_autoptr(AdwApplication) application = adw_application_new("com.boldmerge.NativeDemoApp",
                                                               G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
  return g_application_run(G_APPLICATION(application), argc, argv);
}
