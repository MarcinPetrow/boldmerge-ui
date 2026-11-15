#include <boldmerge/boldmerge.h>

static void
test_button_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_button_new("Save"));
  BmButton *button = BM_BUTTON(widget);
  g_assert_cmpstr(bm_button_get_text(button), ==, "Save");
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(button)), ==,
                  GTK_ACCESSIBLE_ROLE_BUTTON);
  bm_button_set_variant(button, BM_BUTTON_VARIANT_OUTLINE);
  bm_button_set_treatment(button, BM_BUTTON_TREATMENT_DESTRUCTIVE);
  g_assert_true(gtk_widget_has_css_class(widget, "bm-outline"));
  g_assert_true(gtk_widget_has_css_class(widget, "bm-destructive"));
  bm_button_set_loading(button, TRUE);
  g_assert_true(bm_button_get_loading(button));
  g_assert_false(gtk_widget_get_sensitive(widget));
  bm_button_set_loading(button, FALSE);
  g_assert_true(gtk_widget_get_sensitive(widget));
}

static void
on_changed(BmInput *input, guint *count)
{
  (void) input;
  (*count)++;
}

static void
test_input_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_input_new("Email"));
  BmInput *input = BM_INPUT(widget);
  guint changes = 0;
  g_signal_connect(input, "changed", G_CALLBACK(on_changed), &changes);
  bm_input_set_required(input, TRUE);
  g_assert_false(bm_input_validate(input));
  g_assert_nonnull(bm_input_get_error(input));
  bm_input_set_text(input, "person@example.com");
  g_assert_cmpuint(changes, ==, 1);
  g_assert_true(bm_input_validate(input));
  g_assert_cmpstr(bm_input_get_text(input), ==, "person@example.com");
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(bm_input_get_entry(input))), ==,
                  GTK_ACCESSIBLE_ROLE_TEXT_BOX);
}

static void
test_card_composition(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_card_new());
  BmCard *card = BM_CARD(widget);
  GtkWidget *header = gtk_label_new("Header");
  GtkWidget *content = gtk_label_new("Content");
  GtkWidget *footer = gtk_label_new("Footer");
  bm_card_set_header(card, header);
  bm_card_set_child(card, content);
  bm_card_set_footer(card, footer);
  bm_card_set_accessible_label(card, "Account summary");
  g_assert_true(bm_card_get_header(card) == header);
  g_assert_true(bm_card_get_child(card) == content);
  g_assert_true(bm_card_get_footer(card) == footer);
  g_assert_cmpstr(bm_card_get_accessible_label(card), ==, "Account summary");
}

static void
test_checkbox_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_checkbox_new("Accept terms"));
  BmCheckbox *checkbox = BM_CHECKBOX(widget);
  g_assert_cmpstr(bm_checkbox_get_text(checkbox), ==, "Accept terms");
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(checkbox)), ==,
                  GTK_ACCESSIBLE_ROLE_CHECKBOX);
  bm_checkbox_set_required(checkbox, TRUE);
  g_assert_false(bm_checkbox_validate(checkbox));
  g_assert_true(gtk_widget_has_css_class(widget, "bm-invalid"));
  bm_checkbox_set_checked(checkbox, TRUE);
  g_assert_true(bm_checkbox_validate(checkbox));
  bm_checkbox_set_indeterminate(checkbox, TRUE);
  g_assert_true(bm_checkbox_get_indeterminate(checkbox));
}

static void
test_radio_group_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_radio_group_new("Plan"));
  BmRadioGroup *group = BM_RADIO_GROUP(widget);
  BmRadio *starter = BM_RADIO(bm_radio_new("Starter", "starter"));
  BmRadio *team = BM_RADIO(bm_radio_new("Team", "team"));
  bm_radio_group_append(group, starter);
  bm_radio_group_append(group, team);
  bm_radio_group_set_required(group, TRUE);
  g_assert_false(bm_radio_group_validate(group));
  bm_radio_group_set_value(group, "team");
  g_assert_cmpstr(bm_radio_group_get_value(group), ==, "team");
  g_assert_true(bm_radio_get_checked(team));
  g_assert_false(bm_radio_get_checked(starter));
  g_assert_true(bm_radio_group_validate(group));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(group)), ==,
                  GTK_ACCESSIBLE_ROLE_RADIO_GROUP);
}

static void
on_switch_changed(BmSwitch *widget, gboolean active, guint *count)
{
  (void) widget;
  g_assert_true(active);
  (*count)++;
}

static void
test_switch_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_switch_new("Notifications"));
  BmSwitch *control = BM_SWITCH(widget);
  guint changes = 0;
  g_signal_connect(control, "changed", G_CALLBACK(on_switch_changed), &changes);
  bm_switch_set_active(control, TRUE);
  g_assert_true(bm_switch_get_active(control));
  g_assert_cmpuint(changes, ==, 1);
  g_assert_cmpstr(bm_switch_get_text(control), ==, "Notifications");
  g_assert_cmpint(gtk_accessible_get_accessible_role(
                    GTK_ACCESSIBLE(bm_switch_get_control(control))), ==,
                  GTK_ACCESSIBLE_ROLE_SWITCH);
}

static void
test_select_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_select_new("Region"));
  BmSelect *select = BM_SELECT(widget);
  bm_select_set_required(select, TRUE);
  g_assert_false(bm_select_validate(select));
  bm_select_append(select, "Europe", "eu");
  bm_select_append(select, "United States", "us");
  bm_select_set_value(select, "us");
  g_assert_cmpstr(bm_select_get_value(select), ==, "us");
  g_assert_true(bm_select_validate(select));
  bm_select_set_error(select, "Region is unavailable.");
  g_assert_false(bm_select_get_valid(select));
  bm_select_set_error(select, NULL);
  g_assert_true(bm_select_get_valid(select));
}

typedef struct {
  char *value;
  char *label;
  guint count;
} SelectionResult;

static void
on_combobox_changed(BmCombobox *combobox, const char *value,
                    const char *label, SelectionResult *result)
{
  (void) combobox;
  g_free(result->value);
  g_free(result->label);
  result->value = g_strdup(value);
  result->label = g_strdup(label);
  result->count++;
}

static void
test_combobox_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_combobox_new("Region"));
  BmCombobox *combobox = BM_COMBOBOX(widget);
  SelectionResult result = { 0 };
  g_signal_connect(combobox, "changed", G_CALLBACK(on_combobox_changed),
                   &result);
  bm_combobox_append(combobox, "Europe", "eu");
  bm_combobox_append(combobox, "United States", "us");
  bm_combobox_set_required(combobox, TRUE);
  g_assert_false(bm_combobox_validate(combobox));
  g_assert_true(gtk_drop_down_get_enable_search(
    bm_combobox_get_control(combobox)));
  bm_combobox_set_value(combobox, "eu");
  g_assert_true(bm_combobox_validate(combobox));
  g_assert_cmpstr(bm_combobox_get_value(combobox), ==, "eu");
  g_assert_cmpstr(result.value, ==, "eu");
  g_assert_cmpstr(result.label, ==, "Europe");
  g_assert_cmpuint(result.count, ==, 1);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(
                    bm_combobox_get_control(combobox))), ==,
                  GTK_ACCESSIBLE_ROLE_COMBO_BOX);
  g_free(result.value);
  g_free(result.label);
}

static void
on_command_selected(BmCommandPalette *palette, const char *value,
                    const char *label, SelectionResult *result)
{
  (void) palette;
  g_free(result->value);
  g_free(result->label);
  result->value = g_strdup(value);
  result->label = g_strdup(label);
  result->count++;
}

static void
test_command_palette_contract(void)
{
  GtkWidget *widget = g_object_ref_sink(bm_command_palette_new("Commands"));
  BmCommandPalette *palette = BM_COMMAND_PALETTE(widget);
  g_autoptr(BmCommand) project = bm_command_new(
    "New project", "project.new", "create workspace", "Projects");
  g_autoptr(BmCommand) settings = bm_command_new(
    "Open settings", "settings.open", "preferences", "Navigation");
  g_autoptr(BmCommand) archive = bm_command_new(
    "Archive project", "project.archive", "remove", "Projects");
  SelectionResult result = { 0 };
  bm_command_set_disabled(archive, TRUE);
  bm_command_palette_append(palette, project);
  bm_command_palette_append(palette, settings);
  bm_command_palette_append(palette, archive);
  g_signal_connect(palette, "selected", G_CALLBACK(on_command_selected),
                   &result);
  bm_command_palette_set_query(palette, "workspace");
  g_assert_cmpuint(bm_command_palette_get_visible_count(palette), ==, 1);
  g_assert_true(bm_command_palette_activate_first(palette));
  g_assert_cmpstr(result.value, ==, "project.new");
  g_assert_cmpstr(result.label, ==, "New project");
  bm_command_palette_set_query(palette, "archive");
  g_assert_false(bm_command_palette_activate_first(palette));
  g_assert_cmpuint(result.count, ==, 1);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(palette)), ==,
                  GTK_ACCESSIBLE_ROLE_DIALOG);
  g_free(result.value);
  g_free(result.label);
  gtk_window_destroy(GTK_WINDOW(palette));
  g_object_unref(palette);
}

typedef struct {
  guint page;
  guint page_size;
  guint total_items;
  guint count;
} PageResult;

static void
on_page_changed(BmPagination *pagination, guint page, guint page_size,
                guint total_items, PageResult *result)
{
  (void) pagination;
  result->page = page;
  result->page_size = page_size;
  result->total_items = total_items;
  result->count++;
}

static void
test_pagination_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_pagination_new("Project pages"));
  BmPagination *pagination = BM_PAGINATION(widget);
  PageResult result = { 0 };
  g_signal_connect(pagination, "page-changed",
                   G_CALLBACK(on_page_changed), &result);
  bm_pagination_set_total_items(pagination, 236);
  bm_pagination_set_page(pagination, 4);
  g_assert_cmpuint(bm_pagination_get_page_count(pagination), ==, 10);
  g_assert_true(bm_pagination_next(pagination));
  g_assert_cmpuint(result.page, ==, 5);
  g_assert_cmpuint(result.page_size, ==, 25);
  g_assert_cmpuint(result.total_items, ==, 236);
  g_assert_true(bm_pagination_previous(pagination));
  g_assert_cmpuint(bm_pagination_get_page(pagination), ==, 4);
  bm_pagination_set_disabled(pagination, TRUE);
  g_assert_false(bm_pagination_go_to_page(pagination, 8));
  g_assert_cmpuint(result.count, ==, 2);
  bm_pagination_set_disabled(pagination, FALSE);
  bm_pagination_set_page_size(pagination, 50);
  g_assert_cmpuint(bm_pagination_get_page_count(pagination), ==, 5);
  bm_pagination_set_total_items(pagination, 0);
  g_assert_cmpuint(bm_pagination_get_page_count(pagination), ==, 1);
  g_assert_cmpuint(bm_pagination_get_page(pagination), ==, 1);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(widget)), ==,
                  GTK_ACCESSIBLE_ROLE_NAVIGATION);
}

static void
table_factory_setup(GtkSignalListItemFactory *factory, GtkListItem *item,
                    gpointer data)
{
  (void) factory; (void) data;
  gtk_list_item_set_child(item, gtk_label_new(NULL));
}

static void
table_factory_bind(GtkSignalListItemFactory *factory, GtkListItem *item,
                   gpointer data)
{
  (void) factory; (void) data;
  GtkStringObject *value = GTK_STRING_OBJECT(gtk_list_item_get_item(item));
  gtk_label_set_text(GTK_LABEL(gtk_list_item_get_child(item)),
                     gtk_string_object_get_string(value));
}

typedef struct {
  char *sort_key;
  BmTableSortDirection direction;
  guint selected_count;
  gboolean all_selected;
  guint activated_position;
  GObject *activated_item;
} TableResult;

static void
on_table_sort(BmTable *table, const char *key,
              BmTableSortDirection direction, TableResult *result)
{
  (void) table;
  g_free(result->sort_key);
  result->sort_key = g_strdup(key);
  result->direction = direction;
}

static void
on_table_selection(BmTable *table, guint count, gboolean all,
                   TableResult *result)
{
  (void) table;
  result->selected_count = count;
  result->all_selected = all;
}

static void
on_table_activate(BmTable *table, guint position, GObject *item,
                  TableResult *result)
{
  (void) table;
  result->activated_position = position;
  g_set_object(&result->activated_item, item);
}

static void
test_table_contract(void)
{
  GtkStringList *items = gtk_string_list_new(NULL);
  gtk_string_list_append(items, "Atlas");
  gtk_string_list_append(items, "Northstar");
  GtkMultiSelection *selection = gtk_multi_selection_new(G_LIST_MODEL(items));
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_table_new("Projects", GTK_SELECTION_MODEL(selection)));
  g_object_unref(selection);
  BmTable *table = BM_TABLE(widget);
  GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
  g_signal_connect(factory, "setup", G_CALLBACK(table_factory_setup), NULL);
  g_signal_connect(factory, "bind", G_CALLBACK(table_factory_bind), NULL);
  GtkColumnViewColumn *column = bm_table_append_column(
    table, "Project", "name", factory, TRUE);
  g_assert_nonnull(column);
  TableResult result = { 0 };
  g_signal_connect(table, "sort-request", G_CALLBACK(on_table_sort), &result);
  g_signal_connect(table, "selection-changed",
                   G_CALLBACK(on_table_selection), &result);
  g_signal_connect(table, "row-activated",
                   G_CALLBACK(on_table_activate), &result);
  g_assert_true(bm_table_request_sort(table, "name"));
  g_assert_cmpstr(result.sort_key, ==, "name");
  g_assert_cmpint(result.direction, ==, BM_TABLE_SORT_ASCENDING);
  g_assert_true(bm_table_request_sort(table, "name"));
  g_assert_cmpint(result.direction, ==, BM_TABLE_SORT_DESCENDING);
  gtk_selection_model_select_item(bm_table_get_model(table), 0, FALSE);
  g_assert_cmpuint(bm_table_get_selected_count(table), ==, 1);
  g_assert_cmpuint(result.selected_count, ==, 1);
  g_assert_false(result.all_selected);
  bm_table_set_row_activation(table, TRUE);
  g_signal_emit_by_name(bm_table_get_view(table), "activate", 1);
  g_assert_cmpuint(result.activated_position, ==, 1);
  g_assert_true(GTK_IS_STRING_OBJECT(result.activated_item));
  bm_table_set_state(table, BM_TABLE_STATE_LOADING);
  g_assert_cmpint(bm_table_get_state(table), ==, BM_TABLE_STATE_LOADING);
  bm_table_set_state(table, BM_TABLE_STATE_EMPTY);
  g_assert_cmpint(bm_table_get_state(table), ==, BM_TABLE_STATE_EMPTY);
  g_free(result.sort_key);
  g_clear_object(&result.activated_item);
}

static void
on_avatar_error(BmAvatar *avatar, const char *src, guint *count)
{
  (void) avatar;
  g_assert_cmpstr(src, ==, "/definitely/missing/avatar.png");
  (*count)++;
}

static void
test_avatar_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_avatar_new("Ada Lovelace"));
  BmAvatar *avatar = BM_AVATAR(widget);
  g_assert_cmpstr(bm_avatar_get_fallback(avatar), ==, "AL");
  bm_avatar_set_name(avatar, "BoldMerge");
  g_assert_cmpstr(bm_avatar_get_fallback(avatar), ==, "B");
  bm_avatar_set_kind(avatar, BM_AVATAR_ORGANIZATION);
  g_assert_true(gtk_widget_has_css_class(widget, "bm-organization"));
  bm_avatar_set_size(avatar, BM_AVATAR_XLARGE);
  g_assert_cmpint(bm_avatar_get_size(avatar), ==, BM_AVATAR_XLARGE);
  bm_avatar_set_availability(avatar, BM_AVATAR_AVAILABILITY_ONLINE);
  bm_avatar_set_availability_label(avatar, "Online");
  guint errors = 0;
  g_signal_connect(avatar, "image-error", G_CALLBACK(on_avatar_error), &errors);
  bm_avatar_set_src(avatar, "/definitely/missing/avatar.png");
  g_assert_cmpuint(errors, ==, 1);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(avatar)), ==,
                  GTK_ACCESSIBLE_ROLE_IMG);

  g_autoptr(GtkWidget) group_widget = g_object_ref_sink(
    bm_avatar_group_new("Project team"));
  BmAvatarGroup *group = BM_AVATAR_GROUP(group_widget);
  BmAvatar *first = BM_AVATAR(bm_avatar_new("Grace Hopper"));
  BmAvatar *second = BM_AVATAR(bm_avatar_new("Alan Turing"));
  bm_avatar_group_append(group, first);
  bm_avatar_group_append(group, second);
  bm_avatar_group_set_size(group, BM_AVATAR_LARGE);
  bm_avatar_group_set_overflow(group, 3, "3 more teammates");
  g_assert_cmpint(bm_avatar_get_size(first), ==, BM_AVATAR_LARGE);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(group)), ==,
                  GTK_ACCESSIBLE_ROLE_GROUP);
}

static void
test_empty_state_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_empty_state_new("No projects yet"));
  BmEmptyState *state = BM_EMPTY_STATE(widget);
  bm_empty_state_set_description(
    state, "Create a project to start tracking deployments.");
  bm_empty_state_set_heading_level(state, 9);
  bm_empty_state_set_icon(
    state, gtk_image_new_from_icon_name("folder-new-symbolic"));
  bm_empty_state_set_action(state, gtk_button_new_with_label("Create project"));
  g_assert_cmpstr(bm_empty_state_get_heading(state), ==, "No projects yet");
  g_assert_cmpuint(bm_empty_state_get_heading_level(state), ==, 6);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(state)), ==,
                  GTK_ACCESSIBLE_ROLE_REGION);
}

static void test_skeleton_contract(void){g_autoptr(GtkWidget)w=g_object_ref_sink(bm_skeleton_new(BM_SKELETON_CIRCLE,"Loading profile"));g_assert_cmpint(bm_skeleton_get_shape(BM_SKELETON(w)),==,BM_SKELETON_CIRCLE);bm_skeleton_set_shape(BM_SKELETON(w),BM_SKELETON_RECTANGLE);g_assert_cmpint(bm_skeleton_get_shape(BM_SKELETON(w)),==,BM_SKELETON_RECTANGLE);g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(w)),==,GTK_ACCESSIBLE_ROLE_STATUS);}
static void test_kpi_contract(void){g_autoptr(GtkWidget)w=g_object_ref_sink(bm_kpi_new("Monthly recurring revenue","$128,400"));double values[]={72,74,73,79,82,86,91};bm_kpi_set_change(BM_KPI(w),"+8.2%",BM_KPI_CHANGE_FAVORABLE);bm_kpi_set_comparison(BM_KPI(w),"versus last month");bm_kpi_set_sparkline(BM_KPI(w),values,G_N_ELEMENTS(values));g_assert_true(gtk_widget_has_css_class(w,"bm-favorable"));g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(w)),==,GTK_ACCESSIBLE_ROLE_ARTICLE);}
static void test_progress_contract(void){g_autoptr(GtkWidget)w=g_object_ref_sink(bm_progress_new("Storage used"));BmProgress*p=BM_PROGRESS(w);bm_progress_set_range(p,0,100);bm_progress_set_value(p,37);bm_progress_set_value_label(p,"37 GB of 100 GB");g_assert_cmpfloat(bm_progress_get_value(p),==,37);g_assert_cmpuint(bm_progress_get_segments(p),==,20);g_assert_cmpuint(bm_progress_get_filled_segments(p),==,7);bm_progress_set_segments(p,200);g_assert_cmpuint(bm_progress_get_segments(p),==,100);bm_progress_set_value(p,200);g_assert_cmpfloat(bm_progress_get_value(p),==,100);g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(w)),==,GTK_ACCESSIBLE_ROLE_PROGRESS_BAR);}

typedef struct {
  int index;
  char *category;
  guint count;
} ChartChangeResult;

static void
on_chart_active_changed(BmChart *chart,
                        int index,
                        const char *category,
                        ChartChangeResult *result)
{
  (void) chart;
  result->index = index;
  g_free(result->category);
  result->category = g_strdup(category);
  result->count++;
}

static GtkWidget *
find_focusable_drawing_area(GtkWidget *root)
{
  if (GTK_IS_DRAWING_AREA(root) && gtk_widget_get_focusable(root))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root);
       child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find_focusable_drawing_area(child);
    if (match)
      return match;
  }
  return NULL;
}

static gboolean
send_key_to_widget(GtkWidget *widget, guint keyval)
{
  g_autoptr(GListModel) controllers = gtk_widget_observe_controllers(widget);
  for (guint index = 0; index < g_list_model_get_n_items(controllers); index++) {
    g_autoptr(GtkEventController) controller =
      g_list_model_get_item(controllers, index);
    if (!GTK_IS_EVENT_CONTROLLER_KEY(controller))
      continue;
    gboolean handled = FALSE;
    g_signal_emit_by_name(controller, "key-pressed",
                          keyval, 0, GDK_NO_MODIFIER_MASK, &handled);
    return handled;
  }
  return FALSE;
}

static void
test_chart_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_chart_new("Weekly revenue",
                 "Revenue rose through the middle of the week."));
  BmChart *chart = BM_CHART(widget);
  g_autoptr(BmChartSeries) revenue = bm_chart_series_new("Revenue");
  g_autoptr(BmChartSeries) forecast = bm_chart_series_new("Forecast");
  const double actual[] = { 42, 56, 61 };
  const double expected[] = { 45, 52, 60 };
  const char *categories[] = { "Monday", "Tuesday", "Wednesday" };
  for (guint index = 0; index < G_N_ELEMENTS(categories); index++) {
    g_autofree char *actual_label = g_strdup_printf("$%.0fK", actual[index]);
    g_autofree char *expected_label = g_strdup_printf("$%.0fK", expected[index]);
    bm_chart_append_category(chart, categories[index]);
    bm_chart_series_append(revenue, actual[index], actual_label);
    bm_chart_series_append(forecast, expected[index], expected_label);
  }
  bm_chart_append_series(chart, revenue);
  bm_chart_append_series(chart, forecast);
  GtkWidget *table = gtk_label_new("Accessible weekly revenue table");
  bm_chart_set_table(chart, table);
  ChartChangeResult result = { .index = -1 };
  g_signal_connect(chart, "active-category-changed",
                   G_CALLBACK(on_chart_active_changed), &result);
  bm_chart_set_active_index(chart, 1);
  g_assert_cmpuint(bm_chart_get_category_count(chart), ==, 3);
  g_assert_cmpuint(bm_chart_get_series_count(chart), ==, 2);
  g_assert_cmpuint(bm_chart_series_get_length(revenue), ==, 3);
  g_assert_cmpfloat(bm_chart_series_get_value(revenue, 1), ==, 56);
  g_assert_cmpstr(bm_chart_series_get_formatted_value(revenue, 1), ==, "$56K");
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 1);
  g_assert_true(bm_chart_get_table(chart) == table);
  g_assert_true(BM_IS_CHART_TOOLTIP(bm_chart_get_tooltip(chart)));
  g_assert_cmpuint(result.count, ==, 1);
  g_assert_cmpint(result.index, ==, 1);
  g_assert_cmpstr(result.category, ==, "Tuesday");
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(chart)), ==,
                  GTK_ACCESSIBLE_ROLE_GROUP);
  g_free(result.category);
}

static void
test_chart_keyboard_rtl_localization(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_chart_new("الإيراد الأسبوعي",
                 "Przychód wzrósł w całym analizowanym okresie rozliczeniowym."));
  BmChart *chart = BM_CHART(widget);
  g_autoptr(BmChartSeries) series = bm_chart_series_new("الإيراد");
  const char *categories[] = { "الاثنين", "الثلاثاء", "الأربعاء" };
  const char *values[] = { "٤٢ ألف zł", "٥٦ ألف zł", "٦١ ألف zł" };
  for (guint index = 0; index < G_N_ELEMENTS(categories); index++) {
    bm_chart_append_category(chart, categories[index]);
    bm_chart_series_append(series, 42 + index * 7, values[index]);
  }
  bm_chart_append_series(chart, series);
  bm_chart_set_table(chart, gtk_label_new(
    "جدول دائم بالقيم الكاملة — trwała tabela pełnych wartości"));
  GtkWidget *plot = find_focusable_drawing_area(widget);
  g_assert_nonnull(plot);

  gtk_widget_set_direction(widget, GTK_TEXT_DIR_LTR);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_Right));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 0);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_End));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 2);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_Escape));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, -1);

  gtk_widget_set_direction(widget, GTK_TEXT_DIR_RTL);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_Right));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 2);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_Right));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 1);
  g_assert_true(send_key_to_widget(plot, GDK_KEY_Left));
  g_assert_cmpint(bm_chart_get_active_index(chart), ==, 2);
  g_assert_cmpstr(bm_chart_series_get_formatted_value(series, 1), ==,
                  "٥٦ ألف zł");
}

static void
test_icon_foundation(void)
{
  g_assert_true(bm_icon_name_is_supported("search"));
  g_assert_false(bm_icon_name_is_supported("not-reviewed"));
  g_autoptr(GtkWidget) icon = g_object_ref_sink(
    bm_icon_new("search", BM_ICON_SIZE_SMALL));
  g_assert_true(GTK_IS_IMAGE(icon));
  g_assert_cmpint(gtk_image_get_pixel_size(GTK_IMAGE(icon)), ==, 16);
}

static void
test_icon_button_contract(void)
{
  GtkWidget *icon = gtk_image_new_from_icon_name("document-save-symbolic");
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_icon_button_new("Save", icon));
  BmIconButton *button = BM_ICON_BUTTON(widget);
  g_assert_cmpstr(bm_icon_button_get_label(button), ==, "Save");
  g_assert_true(bm_icon_button_get_icon(button) == icon);
  bm_icon_button_set_variant(button, BM_BUTTON_VARIANT_OUTLINE);
  bm_icon_button_set_treatment(button, BM_BUTTON_TREATMENT_DESTRUCTIVE);
  g_assert_true(gtk_widget_has_css_class(widget, "bm-outline"));
  g_assert_true(gtk_widget_has_css_class(widget, "bm-destructive"));
  bm_icon_button_set_loading(button, TRUE);
  g_assert_true(bm_icon_button_get_loading(button));
  g_assert_false(gtk_widget_get_sensitive(widget));
}

static void
test_tag_composition(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_tag_new("Featured"));
  BmTag *tag = BM_TAG(widget);
  GtkWidget *prefix = gtk_image_new_from_icon_name("starred-symbolic");
  GtkWidget *suffix = gtk_label_new("1");
  bm_tag_set_prefix(tag, prefix);
  bm_tag_set_suffix(tag, suffix);
  g_assert_cmpstr(bm_tag_get_text(tag), ==, "Featured");
  g_assert_true(bm_tag_get_prefix(tag) == prefix);
  g_assert_true(bm_tag_get_suffix(tag) == suffix);
}

static void
test_status_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_status_new("Operational", BM_STATUS_TONE_SUCCESS));
  BmStatus *status = BM_STATUS(widget);
  g_assert_cmpstr(bm_status_get_text(status), ==, "Operational");
  g_assert_cmpint(bm_status_get_tone(status), ==, BM_STATUS_TONE_SUCCESS);
  g_assert_true(gtk_widget_has_css_class(widget, "bm-success"));
  bm_status_set_tone(status, BM_STATUS_TONE_ERROR);
  g_assert_true(gtk_widget_has_css_class(widget, "bm-error"));
  g_assert_false(gtk_widget_has_css_class(widget, "bm-success"));
}

static void
test_page_layout_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_page_layout_new());
  BmPageLayout *layout = BM_PAGE_LAYOUT(widget);
  GtkWidget *header = gtk_label_new("Header");
  GtkWidget *content = gtk_label_new("Content");
  GtkWidget *aside = gtk_label_new("Aside");
  bm_page_layout_set_header(layout, header);
  bm_page_layout_set_child(layout, content);
  bm_page_layout_set_aside(layout, aside);
  bm_page_layout_set_mode(layout, BM_PAGE_LAYOUT_SPLIT);
  g_assert_true(bm_page_layout_get_header(layout) == header);
  g_assert_true(bm_page_layout_get_child(layout) == content);
  g_assert_true(bm_page_layout_get_aside(layout) == aside);
  g_assert_cmpint(bm_page_layout_get_mode(layout), ==, BM_PAGE_LAYOUT_SPLIT);
}

static void
test_section_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_section_new("Deployment"));
  BmSection *section = BM_SECTION(widget);
  bm_section_set_header(section, gtk_label_new("Header"));
  bm_section_set_child(section, gtk_label_new("Content"));
  bm_section_set_footer(section, gtk_label_new("Footer"));
  bm_section_set_separator(section, TRUE);
  g_assert_cmpstr(bm_section_get_label(section), ==, "Deployment");
  g_assert_true(bm_section_get_separator(section));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(section)), ==,
                  GTK_ACCESSIBLE_ROLE_REGION);
}

static void
test_breadcrumbs_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_breadcrumbs_new("Project breadcrumbs"));
  BmBreadcrumbs *breadcrumbs = BM_BREADCRUMBS(widget);
  BmBreadcrumbItem *projects = BM_BREADCRUMB_ITEM(
    bm_breadcrumb_item_new("Projects", "https://example.test/projects", FALSE));
  BmBreadcrumbItem *current = BM_BREADCRUMB_ITEM(
    bm_breadcrumb_item_new("Overview", NULL, TRUE));
  bm_breadcrumbs_append(breadcrumbs, projects);
  bm_breadcrumbs_append(breadcrumbs, current);
  g_assert_cmpstr(bm_breadcrumb_item_get_text(projects), ==, "Projects");
  g_assert_true(bm_breadcrumb_item_get_current(current));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(breadcrumbs)), ==,
                  GTK_ACCESSIBLE_ROLE_NAVIGATION);
}

static void
test_tabs_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_tabs_new("Project views"));
  BmTabs *tabs = BM_TABS(widget);
  bm_tabs_append(tabs, "overview", "Overview", gtk_label_new("Overview panel"));
  bm_tabs_append(tabs, "activity", "Activity", gtk_label_new("Activity panel"));
  bm_tabs_set_value(tabs, "activity");
  g_assert_cmpstr(bm_tabs_get_value(tabs), ==, "activity");
  g_assert_cmpstr(gtk_stack_get_visible_child_name(bm_tabs_get_stack(tabs)), ==, "activity");
}

static void
test_segmented_control_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_segmented_control_new("View mode"));
  BmSegmentedControl *control = BM_SEGMENTED_CONTROL(widget);
  bm_segmented_control_append(control, "List", "list");
  GtkToggleButton *board = bm_segmented_control_append(control, "Board", "board");
  bm_segmented_control_set_value(control, "board");
  g_assert_cmpstr(bm_segmented_control_get_value(control), ==, "board");
  g_assert_true(gtk_toggle_button_get_active(board));
}

static void
on_sidebar_navigate(BmSidebar *sidebar, const char *uri, char **result)
{
  (void) sidebar;
  g_free(*result);
  *result = g_strdup(uri);
}

static void
test_sidebar_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_sidebar_new("Workspace"));
  BmSidebar *sidebar = BM_SIDEBAR(widget);
  BmSidebarItem *overview = BM_SIDEBAR_ITEM(
    bm_sidebar_item_new("Overview", "/overview",
                        gtk_image_new_from_icon_name("view-grid-symbolic")));
  BmSidebarItem *projects = BM_SIDEBAR_ITEM(
    bm_sidebar_item_new("Projects", "/projects", NULL));
  g_autofree char *navigated_uri = NULL;
  g_signal_connect(sidebar, "navigate", G_CALLBACK(on_sidebar_navigate),
                   &navigated_uri);
  bm_sidebar_append(sidebar, overview);
  bm_sidebar_append(sidebar, projects);
  bm_sidebar_item_set_active(overview, TRUE);
  g_assert_true(bm_sidebar_item_get_active(overview));
  g_assert_cmpstr(bm_sidebar_item_get_uri(projects), ==, "/projects");
  g_signal_emit_by_name(projects, "clicked");
  g_assert_cmpstr(navigated_uri, ==, "/projects");
  bm_sidebar_set_collapsed(sidebar, TRUE);
  g_assert_true(bm_sidebar_get_collapsed(sidebar));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(sidebar)), ==,
                  GTK_ACCESSIBLE_ROLE_NAVIGATION);
}

static void
test_topbar_composition(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_topbar_new());
  BmTopbar *topbar = BM_TOPBAR(widget);
  bm_topbar_set_start(topbar, gtk_label_new("Menu"));
  bm_topbar_set_child(topbar, gtk_label_new("Workspace"));
  bm_topbar_set_search(topbar, gtk_search_entry_new());
  bm_topbar_set_notifications(topbar, gtk_button_new_with_label("Alerts"));
  bm_topbar_set_account(topbar, gtk_button_new_with_label("Account"));
  g_assert_true(gtk_widget_has_css_class(widget, "bm-topbar"));
}

static void
test_app_shell_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_app_shell_new());
  BmAppShell *shell = BM_APP_SHELL(widget);
  BmSidebar *sidebar = BM_SIDEBAR(bm_sidebar_new("Workspace"));
  BmTopbar *topbar = BM_TOPBAR(bm_topbar_new());
  GtkWidget *trigger = gtk_button_new_with_label("Menu");
  bm_app_shell_set_sidebar(shell, sidebar);
  bm_app_shell_set_topbar(shell, topbar);
  bm_app_shell_set_child(shell, gtk_label_new("Content"));
  bm_app_shell_set_sidebar_collapsed(shell, TRUE);
  g_assert_true(bm_app_shell_get_sidebar_collapsed(shell));
  g_assert_true(bm_sidebar_get_collapsed(sidebar));
  bm_app_shell_set_mobile(shell, TRUE);
  g_assert_true(bm_app_shell_get_mobile(shell));
  bm_app_shell_open_sidebar(shell, trigger);
  g_assert_true(bm_app_shell_get_sidebar_open(shell));
  bm_app_shell_close_sidebar(shell);
  g_assert_false(bm_app_shell_get_sidebar_open(shell));
}

static void
test_tooltip_contract(void)
{
  GtkWidget *trigger = gtk_button_new_with_label("Create project");
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_tooltip_new(trigger, "Create a new project"));
  BmTooltip *tooltip = BM_TOOLTIP(widget);
  g_assert_true(bm_tooltip_get_child(tooltip) == trigger);
  g_assert_cmpstr(bm_tooltip_get_content(tooltip), ==, "Create a new project");
  g_assert_true(gtk_widget_get_has_tooltip(trigger));
  g_assert_cmpstr(gtk_widget_get_tooltip_text(trigger), ==,
                  "Create a new project");
  bm_tooltip_set_content(tooltip, "Add project");
  g_assert_cmpstr(gtk_widget_get_tooltip_text(trigger), ==, "Add project");
}

static void
on_dropdown_select(BmDropdown *dropdown, const char *value, char **result)
{
  (void) dropdown;
  g_free(*result);
  *result = g_strdup(value);
}

static void
test_dropdown_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_dropdown_new("Project actions"));
  BmDropdown *dropdown = BM_DROPDOWN(widget);
  BmDropdownItem *rename = BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Rename", "rename"));
  BmDropdownGroup *danger = BM_DROPDOWN_GROUP(
    bm_dropdown_group_new("Destructive actions"));
  BmDropdownItem *remove = BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Delete", "delete"));
  BmDropdownItem *archive = BM_DROPDOWN_ITEM(
    bm_dropdown_item_new("Archive unavailable", "archive"));
  g_autofree char *selected = NULL;
  g_signal_connect(dropdown, "select", G_CALLBACK(on_dropdown_select), &selected);
  g_assert_false(bm_dropdown_get_open(dropdown));
  bm_dropdown_append_item(dropdown, rename);
  bm_dropdown_group_set_destructive(danger, TRUE);
  bm_dropdown_group_append(danger, remove);
  bm_dropdown_group_append(danger, archive);
  bm_dropdown_append_group(dropdown, danger);
  bm_dropdown_item_set_disabled(archive, TRUE);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(rename)), ==,
                  GTK_ACCESSIBLE_ROLE_MENU_ITEM);
  g_assert_true(bm_dropdown_item_get_destructive(remove));
  g_assert_false(gtk_widget_get_sensitive(GTK_WIDGET(archive)));
  g_signal_emit_by_name(rename, "clicked");
  g_assert_cmpstr(selected, ==, "rename");
  g_signal_emit_by_name(remove, "clicked");
  g_assert_cmpstr(selected, ==, "delete");
  bm_dropdown_set_disabled(dropdown, TRUE);
  g_assert_true(bm_dropdown_get_disabled(dropdown));
  g_assert_false(gtk_widget_get_sensitive(
    GTK_WIDGET(bm_dropdown_get_trigger(dropdown))));
}

typedef struct {
  BmCloseReason reason;
  char *value;
  guint count;
} CloseResult;

static void
on_component_closed(GObject *object, BmCloseReason reason, const char *value,
                    CloseResult *result)
{
  (void) object;
  result->reason = reason;
  g_free(result->value);
  result->value = g_strdup(value);
  result->count++;
}

static void
test_dialog_contract(void)
{
  GtkWindow *parent = GTK_WINDOW(gtk_window_new());
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_dialog_new("Delete project"));
  BmDialog *dialog = BM_DIALOG(widget);
  GtkWidget *trigger = gtk_button_new_with_label("Open dialog");
  CloseResult result = { 0 };
  bm_dialog_set_header(dialog, gtk_label_new("Delete project?"));
  bm_dialog_set_child(dialog, gtk_label_new("This cannot be undone."));
  GtkButton *confirm = bm_dialog_add_action(dialog, "Delete", "delete",
                                            FALSE, TRUE);
  g_signal_connect(dialog, "closed", G_CALLBACK(on_component_closed), &result);
  bm_dialog_present(dialog, parent, trigger, GTK_WIDGET(confirm));
  g_assert_true(bm_dialog_get_open(dialog));
  g_assert_true(gtk_window_get_modal(GTK_WINDOW(dialog)));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(dialog)), ==,
                  GTK_ACCESSIBLE_ROLE_DIALOG);
  g_signal_emit_by_name(confirm, "clicked");
  g_assert_false(bm_dialog_get_open(dialog));
  g_assert_cmpuint(result.count, ==, 1);
  g_assert_cmpint(result.reason, ==, BM_CLOSE_REASON_ACTION);
  g_assert_cmpstr(result.value, ==, "delete");
  g_free(result.value);
  gtk_window_destroy(GTK_WINDOW(dialog));
  gtk_window_destroy(parent);
}

static void
test_drawer_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_drawer_new("Project details"));
  BmDrawer *drawer = BM_DRAWER(widget);
  GtkWidget *trigger = gtk_button_new_with_label("Open details");
  GtkWidget *first = gtk_button_new_with_label("Close");
  CloseResult result = { 0 };
  bm_drawer_set_content(drawer, gtk_label_new("Workspace"));
  bm_drawer_set_header(drawer, gtk_label_new("Project details"));
  bm_drawer_set_child(drawer, first);
  g_signal_connect(drawer, "closed", G_CALLBACK(on_component_closed), &result);
  bm_drawer_set_mode(drawer, BM_DRAWER_MODE_PUSH);
  bm_drawer_set_position(drawer, BM_DRAWER_POSITION_START);
  g_assert_cmpint(bm_drawer_get_mode(drawer), ==, BM_DRAWER_MODE_PUSH);
  g_assert_cmpint(bm_drawer_get_position(drawer), ==,
                  BM_DRAWER_POSITION_START);
  bm_drawer_set_mobile(drawer, TRUE);
  g_assert_true(bm_drawer_get_mobile(drawer));
  bm_drawer_open(drawer, trigger, first);
  g_assert_true(bm_drawer_get_open(drawer));
  g_assert_true(bm_drawer_close(drawer, BM_CLOSE_REASON_ACTION, "done"));
  g_assert_false(bm_drawer_get_open(drawer));
  g_assert_cmpuint(result.count, ==, 1);
  g_assert_cmpint(result.reason, ==, BM_CLOSE_REASON_ACTION);
  g_assert_cmpstr(result.value, ==, "done");
  g_free(result.value);
}

static void
on_toast_closed(BmToast *toast, BmToastCloseReason reason,
                BmToastCloseReason *result)
{
  (void) toast;
  *result = reason;
}

static void
test_toast_region_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_toast_region_new("Notifications"));
  BmToastRegion *region = BM_TOAST_REGION(widget);
  bm_toast_region_set_child(region, gtk_label_new("Workspace"));
  g_autoptr(BmToast) saved = bm_toast_region_push(
    region, "Saved", "Workspace settings updated.",
    BM_TOAST_TONE_SUCCESS, 0);
  g_autoptr(BmToast) failed = bm_toast_region_push(
    region, "Connection interrupted", "Retrying in the background.",
    BM_TOAST_TONE_ERROR, 0);
  BmToastCloseReason reason = BM_TOAST_CLOSE_REASON_TIMEOUT;
  g_signal_connect(saved, "closed", G_CALLBACK(on_toast_closed), &reason);
  g_assert_cmpuint(bm_toast_region_get_count(region), ==, 2);
  g_assert_cmpstr(bm_toast_get_title(saved), ==, "Saved");
  g_assert_cmpint(bm_toast_get_tone(failed), ==, BM_TOAST_TONE_ERROR);
  bm_toast_dismiss(saved, BM_TOAST_CLOSE_REASON_API);
  while (g_main_context_iteration(NULL, FALSE));
  g_assert_cmpint(reason, ==, BM_TOAST_CLOSE_REASON_API);
  g_assert_cmpuint(bm_toast_region_get_count(region), ==, 1);
  bm_toast_region_clear(region);
  g_assert_cmpuint(bm_toast_region_get_count(region), ==, 0);
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(region)), ==,
                  GTK_ACCESSIBLE_ROLE_REGION);
}

static void
test_validation_summary_contract(void)
{
  g_autoptr(GtkWidget) widget = g_object_ref_sink(
    bm_validation_summary_new("Correct the following errors"));
  BmValidationSummary *summary = BM_VALIDATION_SUMMARY(widget);
  g_autoptr(GtkWidget) input_widget = g_object_ref_sink(
    bm_input_new("Workspace name"));
  g_autoptr(GtkWidget) checkbox_widget = g_object_ref_sink(
    bm_checkbox_new("Accept terms"));
  g_autoptr(GtkWidget) combobox_widget = g_object_ref_sink(
    bm_combobox_new("Region"));
  BmInput *input = BM_INPUT(input_widget);
  BmCheckbox *checkbox = BM_CHECKBOX(checkbox_widget);
  BmCombobox *combobox = BM_COMBOBOX(combobox_widget);
  bm_input_set_required(input, TRUE);
  bm_checkbox_set_required(checkbox, TRUE);
  bm_combobox_append(combobox, "Europe", "eu");
  bm_combobox_set_required(combobox, TRUE);
  bm_validation_summary_add(summary, input_widget, NULL, NULL);
  bm_validation_summary_add(summary, checkbox_widget, NULL, NULL);
  bm_validation_summary_add(summary, combobox_widget, NULL, NULL);
  g_assert_false(bm_validation_summary_validate(
    summary, BM_VALIDATION_FOCUS_NONE));
  g_assert_cmpuint(bm_validation_summary_get_error_count(summary), ==, 3);
  g_assert_true(gtk_widget_get_visible(widget));
  g_assert_cmpint(gtk_accessible_get_accessible_role(GTK_ACCESSIBLE(summary)), ==,
                  GTK_ACCESSIBLE_ROLE_ALERT);
  bm_input_set_text(input, "Northstar");
  bm_checkbox_set_checked(checkbox, TRUE);
  bm_combobox_set_value(combobox, "eu");
  bm_validation_summary_refresh(summary);
  g_assert_cmpuint(bm_validation_summary_get_error_count(summary), ==, 0);
  g_assert_false(gtk_widget_get_visible(widget));
}

static void
test_theme_density_matrix(void)
{
  g_autoptr(BmThemeScope) scope = bm_theme_scope_new(NULL);
  g_autoptr(GtkWidget) button_widget = g_object_ref_sink(bm_button_new("Matrix"));
  BmButton *button = BM_BUTTON(button_widget);
  bm_button_set_theme_scope(button, scope);
  for (BmTheme theme = BM_THEME_LIGHT; theme <= BM_THEME_DARK; theme++) {
    bm_theme_scope_set_theme(scope, theme);
    for (BmDensity density = BM_DENSITY_BALANCED; density <= BM_DENSITY_COMPACT; density++) {
      bm_theme_scope_set_density(scope, density);
      g_assert_true(gtk_widget_has_css_class(button_widget,
                    theme == BM_THEME_LIGHT ? "bm-theme-light" : "bm-theme-dark"));
      g_assert_true(gtk_widget_has_css_class(button_widget,
                    density == BM_DENSITY_BALANCED ? "bm-density-balanced" : "bm-density-compact"));
    }
  }
}

static void
test_reduced_motion_setting(void)
{
  GtkSettings *settings = gtk_settings_get_for_display(gdk_display_get_default());
  gboolean original = TRUE;
  g_object_get(settings, "gtk-enable-animations", &original, NULL);
  g_object_set(settings, "gtk-enable-animations", FALSE, NULL);
  g_autoptr(GtkWidget) widget = g_object_ref_sink(bm_button_new("Loading"));
  bm_button_set_loading(BM_BUTTON(widget), TRUE);
  g_assert_true(bm_button_get_loading(BM_BUTTON(widget)));
  g_object_set(settings, "gtk-enable-animations", original, NULL);
}

int
main(int argc, char **argv)
{
  g_test_init(&argc, &argv, NULL);
  if (!gtk_init_check()) {
    g_test_message("No GTK display is available; component tests skipped");
    return 77;
  }
  g_autoptr(GError) error = NULL;
  if (!bm_init_for_display(gdk_display_get_default(), &error)) {
    g_printerr("Could not initialize component CSS: %s\n", error->message);
    return 1;
  }
  g_test_add_func("/boldmerge/button/contract", test_button_contract);
  g_test_add_func("/boldmerge/input/contract", test_input_contract);
  g_test_add_func("/boldmerge/card/composition", test_card_composition);
  g_test_add_func("/boldmerge/checkbox/contract", test_checkbox_contract);
  g_test_add_func("/boldmerge/radio-group/contract", test_radio_group_contract);
  g_test_add_func("/boldmerge/switch/contract", test_switch_contract);
  g_test_add_func("/boldmerge/select/contract", test_select_contract);
  g_test_add_func("/boldmerge/combobox/contract", test_combobox_contract);
  g_test_add_func("/boldmerge/icon/contract", test_icon_foundation);
  g_test_add_func("/boldmerge/icon-button/contract", test_icon_button_contract);
  g_test_add_func("/boldmerge/tag/composition", test_tag_composition);
  g_test_add_func("/boldmerge/status/contract", test_status_contract);
  g_test_add_func("/boldmerge/page-layout/contract", test_page_layout_contract);
  g_test_add_func("/boldmerge/section/contract", test_section_contract);
  g_test_add_func("/boldmerge/breadcrumbs/contract", test_breadcrumbs_contract);
  g_test_add_func("/boldmerge/tabs/contract", test_tabs_contract);
  g_test_add_func("/boldmerge/segmented-control/contract", test_segmented_control_contract);
  g_test_add_func("/boldmerge/sidebar/contract", test_sidebar_contract);
  g_test_add_func("/boldmerge/topbar/composition", test_topbar_composition);
  g_test_add_func("/boldmerge/app-shell/contract", test_app_shell_contract);
  g_test_add_func("/boldmerge/tooltip/contract", test_tooltip_contract);
  g_test_add_func("/boldmerge/dropdown/contract", test_dropdown_contract);
  g_test_add_func("/boldmerge/dialog/contract", test_dialog_contract);
  g_test_add_func("/boldmerge/drawer/contract", test_drawer_contract);
  g_test_add_func("/boldmerge/toast-region/contract", test_toast_region_contract);
  g_test_add_func("/boldmerge/validation-summary/contract", test_validation_summary_contract);
  g_test_add_func("/boldmerge/command-palette/contract", test_command_palette_contract);
  g_test_add_func("/boldmerge/pagination/contract", test_pagination_contract);
  g_test_add_func("/boldmerge/table/contract", test_table_contract);
  g_test_add_func("/boldmerge/avatar/contract", test_avatar_contract);
  g_test_add_func("/boldmerge/empty-state/contract", test_empty_state_contract);
  g_test_add_func("/boldmerge/skeleton/contract", test_skeleton_contract);
  g_test_add_func("/boldmerge/kpi/contract", test_kpi_contract);
  g_test_add_func("/boldmerge/progress/contract", test_progress_contract);
  g_test_add_func("/boldmerge/chart/contract", test_chart_contract);
  g_test_add_func("/boldmerge/chart/keyboard-rtl-localization",
                  test_chart_keyboard_rtl_localization);
  g_test_add_func("/boldmerge/components/theme-density-matrix", test_theme_density_matrix);
  g_test_add_func("/boldmerge/components/reduced-motion", test_reduced_motion_setting);
  return g_test_run();
}
