#include <boldmerge/boldmerge.h>

#include <glib/gstdio.h>

typedef struct {
  GtkWidget *window;
  GtkWidget *page;
  GtkLabel *context;
  BmThemeScope *scope;
} Fixture;

static void
spin_main_context(void)
{
  gint64 deadline = g_get_monotonic_time() + 150 * G_TIME_SPAN_MILLISECOND;
  while (g_get_monotonic_time() < deadline) {
    while (g_main_context_iteration(NULL, FALSE));
    g_usleep(1000);
  }
  while (g_main_context_iteration(NULL, FALSE));
}

static GtkWidget *
heading(const char *text)
{
  GtkWidget *label = gtk_label_new(text);
  gtk_widget_add_css_class(label, "title-2");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  return label;
}

static GtkWidget *
build_core(Fixture *fixture)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_box_append(GTK_BOX(box), heading("Core controls"));

  GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  GtkWidget *primary = bm_button_new("Create project");
  GtkWidget *outline = bm_button_new("Review");
  GtkWidget *destructive = bm_button_new("Delete");
  bm_button_set_variant(BM_BUTTON(outline), BM_BUTTON_VARIANT_OUTLINE);
  bm_button_set_treatment(BM_BUTTON(destructive),
                          BM_BUTTON_TREATMENT_DESTRUCTIVE);
  bm_button_set_theme_scope(BM_BUTTON(primary), fixture->scope);
  bm_button_set_theme_scope(BM_BUTTON(outline), fixture->scope);
  bm_button_set_theme_scope(BM_BUTTON(destructive), fixture->scope);
  gtk_box_append(GTK_BOX(buttons), primary);
  gtk_box_append(GTK_BOX(buttons), outline);
  gtk_box_append(GTK_BOX(buttons), destructive);
  gtk_box_append(GTK_BOX(box), buttons);

  GtkWidget *input = bm_input_new("Workspace name");
  bm_input_set_text(BM_INPUT(input), "Northstar operations");
  bm_input_set_theme_scope(BM_INPUT(input), fixture->scope);
  gtk_box_append(GTK_BOX(box), input);

  GtkWidget *error = bm_input_new("Server address");
  bm_input_set_text(BM_INPUT(error), "invalid address");
  bm_input_set_error(BM_INPUT(error), "Enter a valid server address.");
  bm_input_set_theme_scope(BM_INPUT(error), fixture->scope);
  gtk_box_append(GTK_BOX(box), error);

  GtkWidget *selection = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  GtkWidget *checkbox = bm_checkbox_new("Include archived projects");
  GtkWidget *toggle = bm_switch_new("Automatic updates");
  bm_checkbox_set_checked(BM_CHECKBOX(checkbox), TRUE);
  bm_switch_set_active(BM_SWITCH(toggle), TRUE);
  bm_checkbox_set_theme_scope(BM_CHECKBOX(checkbox), fixture->scope);
  bm_switch_set_theme_scope(BM_SWITCH(toggle), fixture->scope);
  gtk_box_append(GTK_BOX(selection), checkbox);
  gtk_box_append(GTK_BOX(selection), toggle);
  gtk_box_append(GTK_BOX(box), selection);

  GtkWidget *card_widget = bm_card_new();
  BmCard *card = BM_CARD(card_widget);
  bm_card_set_accessible_label(card, "Deployment summary");
  bm_card_set_theme_scope(card, fixture->scope);
  GtkWidget *card_title = gtk_label_new("Deployment summary");
  gtk_widget_add_css_class(card_title, "title-3");
  gtk_label_set_xalign(GTK_LABEL(card_title), 0.0f);
  bm_card_set_header(card, card_title);
  GtkWidget *card_copy = gtk_label_new(
    "Production is healthy. Two changes await approval.");
  gtk_label_set_wrap(GTK_LABEL(card_copy), TRUE);
  gtk_label_set_xalign(GTK_LABEL(card_copy), 0.0f);
  bm_card_set_child(card, card_copy);
  bm_card_set_footer(card, bm_status_new("Operational",
                                         BM_STATUS_TONE_SUCCESS));
  gtk_box_append(GTK_BOX(box), card_widget);
  return box;
}

static GtkWidget *
build_data(Fixture *fixture)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_box_append(GTK_BOX(box), heading("Data and visualization"));

  BmKpi *kpi = BM_KPI(bm_kpi_new("Monthly recurring revenue", "$128,400"));
  const double sparkline[] = { 72, 74, 73, 79, 82, 86, 91 };
  bm_kpi_set_change(kpi, "+8.2%", BM_KPI_CHANGE_FAVORABLE);
  bm_kpi_set_comparison(kpi, "versus last month");
  bm_kpi_set_sparkline(kpi, sparkline, G_N_ELEMENTS(sparkline));
  bm_kpi_set_theme_scope(kpi, fixture->scope);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(kpi));

  BmProgress *progress = BM_PROGRESS(bm_progress_new("Storage used"));
  bm_progress_set_value(progress, 37);
  bm_progress_set_value_label(progress, "37 GB of 100 GB");
  bm_progress_set_theme_scope(progress, fixture->scope);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(progress));

  BmChart *chart = BM_CHART(bm_chart_new(
    "Weekly recurring revenue",
    "Actual revenue remains above forecast after Tuesday."));
  g_autoptr(BmChartSeries) actual = bm_chart_series_new("Actual");
  g_autoptr(BmChartSeries) forecast = bm_chart_series_new("Forecast");
  const char *days[] = { "Mon", "Tue", "Wed", "Thu", "Fri" };
  const double actual_values[] = { 42, 56, 61, 58, 68 };
  const double forecast_values[] = { 45, 52, 57, 61, 64 };
  for (guint index = 0; index < G_N_ELEMENTS(days); index++) {
    g_autofree char *actual_label = g_strdup_printf("$%.0fK", actual_values[index]);
    g_autofree char *forecast_label = g_strdup_printf("$%.0fK", forecast_values[index]);
    bm_chart_append_category(chart, days[index]);
    bm_chart_series_append(actual, actual_values[index], actual_label);
    bm_chart_series_append(forecast, forecast_values[index], forecast_label);
  }
  bm_chart_append_series(chart, actual);
  bm_chart_append_series(chart, forecast);
  bm_chart_set_table(chart, gtk_label_new(
    "Table alternative: Mon–Fri actual and forecast values"));
  bm_chart_set_theme_scope(chart, fixture->scope);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(chart));
  return box;
}

static GtkWidget *
build_selection_and_display(Fixture *fixture)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_box_append(GTK_BOX(box), heading("Selection and compact display"));

  GtkWidget *controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  BmRadioGroup *radios = BM_RADIO_GROUP(
    bm_radio_group_new("Deployment channel"));
  bm_radio_group_append(radios, BM_RADIO(bm_radio_new("Stable", "stable")));
  bm_radio_group_append(radios, BM_RADIO(bm_radio_new("Preview", "preview")));
  bm_radio_group_append(radios, BM_RADIO(bm_radio_new("Nightly", "nightly")));
  bm_radio_group_set_value(radios, "stable");
  bm_radio_group_set_theme_scope(radios, fixture->scope);
  gtk_box_append(GTK_BOX(controls), GTK_WIDGET(radios));

  BmSelect *select = BM_SELECT(bm_select_new("Region"));
  bm_select_append(select, "Europe", "eu");
  bm_select_append(select, "United States", "us");
  bm_select_append(select, "Asia Pacific", "apac");
  bm_select_set_value(select, "eu");
  bm_select_set_theme_scope(select, fixture->scope);
  gtk_widget_set_hexpand(GTK_WIDGET(select), TRUE);
  gtk_box_append(GTK_BOX(controls), GTK_WIDGET(select));
  gtk_box_append(GTK_BOX(box), controls);

  GtkWidget *compact = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  BmIconButton *icon_button = BM_ICON_BUTTON(bm_icon_button_new(
    "Save", gtk_image_new_from_icon_name("document-save-symbolic")));
  bm_icon_button_set_variant(icon_button, BM_BUTTON_VARIANT_OUTLINE);
  bm_icon_button_set_theme_scope(icon_button, fixture->scope);
  gtk_box_append(GTK_BOX(compact), GTK_WIDGET(icon_button));
  BmTag *featured = BM_TAG(bm_tag_new("Featured"));
  bm_tag_set_prefix(featured, gtk_image_new_from_icon_name("starred-symbolic"));
  bm_tag_set_theme_scope(featured, fixture->scope);
  gtk_box_append(GTK_BOX(compact), GTK_WIDGET(featured));
  BmTag *review = BM_TAG(bm_tag_new("Needs review"));
  bm_tag_set_theme_scope(review, fixture->scope);
  gtk_box_append(GTK_BOX(compact), GTK_WIDGET(review));
  gtk_box_append(GTK_BOX(box), compact);
  return box;
}

static GtkWidget *
build_identity_and_states(Fixture *fixture)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_box_append(GTK_BOX(box), heading("Identity and content states"));

  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  BmAvatarGroup *group = BM_AVATAR_GROUP(
    bm_avatar_group_new("Release operations team"));
  BmAvatar *marcin = BM_AVATAR(bm_avatar_new("Marcin Kowalski"));
  BmAvatar *amina = BM_AVATAR(bm_avatar_new("Amina Rahman"));
  BmAvatar *northstar = BM_AVATAR(bm_avatar_new("Northstar Labs"));
  bm_avatar_set_availability(marcin, BM_AVATAR_AVAILABILITY_ONLINE);
  bm_avatar_set_availability(amina, BM_AVATAR_AVAILABILITY_AWAY);
  bm_avatar_set_kind(northstar, BM_AVATAR_ORGANIZATION);
  bm_avatar_group_append(group, marcin);
  bm_avatar_group_append(group, amina);
  bm_avatar_group_append(group, northstar);
  bm_avatar_group_set_overflow(group, 4, "4 more team members");
  bm_avatar_group_set_theme_scope(group, fixture->scope);
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(group));

  BmSkeleton *text = BM_SKELETON(
    bm_skeleton_new(BM_SKELETON_TEXT, "Loading deployment details"));
  BmSkeleton *circle = BM_SKELETON(
    bm_skeleton_new(BM_SKELETON_CIRCLE, "Loading owner"));
  bm_skeleton_set_size(text, 180, 16);
  bm_skeleton_set_size(circle, 44, 44);
  bm_skeleton_set_theme_scope(text, fixture->scope);
  bm_skeleton_set_theme_scope(circle, fixture->scope);
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(text));
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(circle));
  gtk_box_append(GTK_BOX(box), row);

  BmEmptyState *empty = BM_EMPTY_STATE(bm_empty_state_new("No failed runs"));
  bm_empty_state_set_description(
    empty, "All production workflows completed successfully.");
  bm_empty_state_set_icon(
    empty, gtk_image_new_from_icon_name("emblem-ok-symbolic"));
  bm_empty_state_set_action(empty, gtk_button_new_with_label("View history"));
  bm_empty_state_set_theme_scope(empty, fixture->scope);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(empty));
  return box;
}

static Fixture *
fixture_new(void)
{
  Fixture *fixture = g_new0(Fixture, 1);
  fixture->scope = bm_theme_scope_new(NULL);
  bm_theme_scope_set_theme(fixture->scope, BM_THEME_LIGHT);
  bm_theme_scope_set_density(fixture->scope, BM_DENSITY_BALANCED);
  bm_theme_scope_set_accent(fixture->scope, BM_ACCENT_CYAN);
  fixture->window = gtk_window_new();
  gtk_window_set_decorated(GTK_WINDOW(fixture->window), FALSE);
  gtk_window_set_default_size(GTK_WINDOW(fixture->window), 1000, 980);

  fixture->page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
  gtk_widget_add_css_class(fixture->page, "bm-visual-fixture");
  gtk_widget_set_margin_top(fixture->page, 24);
  gtk_widget_set_margin_bottom(fixture->page, 24);
  gtk_widget_set_margin_start(fixture->page, 24);
  gtk_widget_set_margin_end(fixture->page, 24);
  bm_theme_scope_apply(fixture->scope, fixture->page);

  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  GtkWidget *title = gtk_label_new("BoldMerge Native visual fixture");
  fixture->context = GTK_LABEL(gtk_label_new(NULL));
  gtk_widget_add_css_class(title, "title-1");
  gtk_widget_add_css_class(GTK_WIDGET(fixture->context), "dim-label");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_label_set_xalign(GTK_LABEL(title), 0.0f);
  gtk_label_set_xalign(fixture->context, 1.0f);
  gtk_box_append(GTK_BOX(header), title);
  gtk_box_append(GTK_BOX(header), GTK_WIDGET(fixture->context));
  gtk_box_append(GTK_BOX(fixture->page), header);

  GtkWidget *columns = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(columns), 20);
  gtk_grid_set_column_homogeneous(GTK_GRID(columns), TRUE);
  gtk_widget_set_vexpand(columns, TRUE);
  gtk_grid_attach(GTK_GRID(columns), build_core(fixture), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(columns), build_data(fixture), 1, 0, 1, 1);
  gtk_box_append(GTK_BOX(fixture->page), columns);

  GtkWidget *extended = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(extended), 20);
  gtk_grid_set_column_homogeneous(GTK_GRID(extended), TRUE);
  gtk_grid_attach(GTK_GRID(extended),
                  build_selection_and_display(fixture), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(extended),
                  build_identity_and_states(fixture), 1, 0, 1, 1);
  gtk_box_append(GTK_BOX(fixture->page),
                 gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_box_append(GTK_BOX(fixture->page), extended);
  gtk_window_set_child(GTK_WINDOW(fixture->window), fixture->page);
  return fixture;
}

static gboolean
capture_widget(GtkWidget *widget, const char *path, GError **error)
{
  int width = gtk_widget_get_width(widget);
  int height = gtk_widget_get_height(widget);
  g_autoptr(GdkPaintable) paintable = gtk_widget_paintable_new(widget);
  g_autoptr(GtkSnapshot) snapshot = gtk_snapshot_new();
  gdk_paintable_snapshot(paintable, snapshot, width, height);
  g_autoptr(GskRenderNode) node = gtk_snapshot_free_to_node(
    g_steal_pointer(&snapshot));
  if (!node) {
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "widget snapshot produced no render node");
    return FALSE;
  }
  GdkSurface *surface = gtk_native_get_surface(gtk_widget_get_native(widget));
  g_autoptr(GskRenderer) renderer = gsk_renderer_new_for_surface(surface);
  graphene_rect_t viewport;
  graphene_rect_init(&viewport, 0, 0, width, height);
  g_autoptr(GdkTexture) texture = gsk_renderer_render_texture(
    renderer, node, &viewport);
  gboolean saved = gdk_texture_save_to_png(texture, path);
  gsk_renderer_unrealize(renderer);
  if (!saved) {
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                "could not save PNG to %s", path);
    return FALSE;
  }
  return TRUE;
}

int
main(int argc, char **argv)
{
  if (argc != 2) {
    g_printerr("usage: capture-visual OUTPUT_DIRECTORY\n");
    return 2;
  }
  gtk_init();
  g_autoptr(GError) error = NULL;
  if (!bm_init_for_display(gdk_display_get_default(), &error)) {
    g_printerr("BoldMerge initialization failed: %s\n", error->message);
    return 1;
  }
  GtkSettings *settings = gtk_settings_get_for_display(gdk_display_get_default());
  g_object_set(settings, "gtk-enable-animations", FALSE, NULL);
  GtkCssProvider *font_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_string(font_provider,
    "* { font-family: 'Noto Sans'; }"
    ".bm-visual-fixture.bm-theme-light {"
    "  color: @bm_light_cyan_color_text_primary;"
    "  background-color: @bm_light_cyan_color_canvas;"
    "}"
    ".bm-visual-fixture.bm-theme-dark {"
    "  color: @bm_dark_cyan_color_text_primary;"
    "  background-color: @bm_dark_cyan_color_canvas;"
    "}");
  gtk_style_context_add_provider_for_display(
    gdk_display_get_default(), GTK_STYLE_PROVIDER(font_provider),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
  g_object_unref(font_provider);

  if (g_mkdir_with_parents(argv[1], 0755) != 0) {
    g_printerr("could not create output directory %s\n", argv[1]);
    return 1;
  }
  Fixture *fixture = fixture_new();
  gtk_window_present(GTK_WINDOW(fixture->window));
  spin_main_context();
  g_autofree char *warmup_path = g_build_filename(argv[1], ".warmup.png", NULL);
  if (!capture_widget(fixture->page, warmup_path, &error)) {
    g_printerr("warmup capture failed: %s\n", error->message);
    gtk_window_destroy(GTK_WINDOW(fixture->window));
    g_object_unref(fixture->scope);
    g_free(fixture);
    return 1;
  }
  g_remove(warmup_path);
  gtk_widget_queue_draw(fixture->page);
  spin_main_context();

  for (BmTheme theme = BM_THEME_LIGHT; theme <= BM_THEME_DARK; theme++) {
    for (BmDensity density = BM_DENSITY_BALANCED;
         density <= BM_DENSITY_COMPACT;
         density++) {
      const char *theme_name = theme == BM_THEME_LIGHT ? "light" : "dark";
      const char *density_name = density == BM_DENSITY_BALANCED
        ? "balanced" : "compact";
      g_autofree char *label = g_strdup_printf("%s · %s",
                                               theme_name, density_name);
      g_autofree char *filename = g_strdup_printf(
        "native-tier-a-%s-%s.png", theme_name, density_name);
      g_autofree char *path = g_build_filename(argv[1], filename, NULL);
      bm_theme_scope_set_theme(fixture->scope, theme);
      bm_theme_scope_set_density(fixture->scope, density);
      gtk_label_set_text(fixture->context, label);
      spin_main_context();
      if (!capture_widget(fixture->page, path, &error)) {
        g_printerr("capture failed: %s\n", error->message);
        gtk_window_destroy(GTK_WINDOW(fixture->window));
        g_object_unref(fixture->scope);
        g_free(fixture);
        return 1;
      }
    }
  }

  gtk_window_destroy(GTK_WINDOW(fixture->window));
  g_object_unref(fixture->scope);
  g_free(fixture);
  return 0;
}
