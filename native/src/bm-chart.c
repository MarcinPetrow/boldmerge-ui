#include "bm-private.h"

#include <math.h>

struct _BmChartSeries {
  GObject parent_instance;
  char *label;
  GArray *values;
  GPtrArray *formatted_values;
};

G_DEFINE_FINAL_TYPE(BmChartSeries, bm_chart_series, G_TYPE_OBJECT)

static void
bm_chart_series_finalize(GObject *object)
{
  BmChartSeries *self = BM_CHART_SERIES(object);
  g_free(self->label);
  g_array_unref(self->values);
  g_ptr_array_unref(self->formatted_values);
  G_OBJECT_CLASS(bm_chart_series_parent_class)->finalize(object);
}

static void
bm_chart_series_class_init(BmChartSeriesClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = bm_chart_series_finalize;
}

static void
bm_chart_series_init(BmChartSeries *self)
{
  self->values = g_array_new(FALSE, FALSE, sizeof(double));
  self->formatted_values = g_ptr_array_new_with_free_func(g_free);
}

BmChartSeries *
bm_chart_series_new(const char *label)
{
  BmChartSeries *self = g_object_new(BM_TYPE_CHART_SERIES, NULL);
  self->label = g_strdup(label ? label : "");
  return self;
}

void
bm_chart_series_append(BmChartSeries *self,
                       double value,
                       const char *formatted_value)
{
  g_return_if_fail(BM_IS_CHART_SERIES(self));
  g_return_if_fail(isfinite(value));
  g_array_append_val(self->values, value);
  if (formatted_value) {
    g_ptr_array_add(self->formatted_values, g_strdup(formatted_value));
  } else {
    char buffer[G_ASCII_DTOSTR_BUF_SIZE];
    g_ascii_dtostr(buffer, sizeof(buffer), value);
    g_ptr_array_add(self->formatted_values, g_strdup(buffer));
  }
}

const char *
bm_chart_series_get_label(BmChartSeries *self)
{
  g_return_val_if_fail(BM_IS_CHART_SERIES(self), NULL);
  return self->label;
}

guint
bm_chart_series_get_length(BmChartSeries *self)
{
  g_return_val_if_fail(BM_IS_CHART_SERIES(self), 0);
  return self->values->len;
}

double
bm_chart_series_get_value(BmChartSeries *self, guint index)
{
  g_return_val_if_fail(BM_IS_CHART_SERIES(self), 0.0);
  g_return_val_if_fail(index < self->values->len, 0.0);
  return g_array_index(self->values, double, index);
}

const char *
bm_chart_series_get_formatted_value(BmChartSeries *self, guint index)
{
  g_return_val_if_fail(BM_IS_CHART_SERIES(self), NULL);
  g_return_val_if_fail(index < self->formatted_values->len, NULL);
  return g_ptr_array_index(self->formatted_values, index);
}

struct _BmChartTooltip {
  GtkPopover parent_instance;
  GtkLabel *heading;
  GtkBox *rows;
};

G_DEFINE_FINAL_TYPE(BmChartTooltip, bm_chart_tooltip, GTK_TYPE_POPOVER)

static void
bm_chart_tooltip_class_init(BmChartTooltipClass *klass)
{
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_TOOLTIP);
}

static void
bm_chart_tooltip_init(BmChartTooltip *self)
{
  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  self->heading = GTK_LABEL(gtk_label_new(NULL));
  self->rows = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4));
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-chart-tooltip");
  gtk_widget_add_css_class(GTK_WIDGET(self->heading),
                           "bm-chart-tooltip-heading");
  gtk_label_set_xalign(self->heading, 0.0f);
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(self->heading));
  gtk_box_append(GTK_BOX(content), GTK_WIDGET(self->rows));
  gtk_popover_set_child(GTK_POPOVER(self), content);
  gtk_popover_set_autohide(GTK_POPOVER(self), FALSE);
  gtk_popover_set_has_arrow(GTK_POPOVER(self), TRUE);
}

GtkWidget *
bm_chart_tooltip_new(void)
{
  return g_object_new(BM_TYPE_CHART_TOOLTIP, NULL);
}

void
bm_chart_tooltip_set_heading(BmChartTooltip *self, const char *heading)
{
  g_return_if_fail(BM_IS_CHART_TOOLTIP(self));
  gtk_label_set_text(self->heading, heading ? heading : "");
}

void
bm_chart_tooltip_clear(BmChartTooltip *self)
{
  g_return_if_fail(BM_IS_CHART_TOOLTIP(self));
  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(self->rows));
  while (child) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(self->rows, child);
    child = next;
  }
}

void
bm_chart_tooltip_append(BmChartTooltip *self,
                        const char *label,
                        const char *value)
{
  g_return_if_fail(BM_IS_CHART_TOOLTIP(self));
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  GtkWidget *name = gtk_label_new(label ? label : "");
  GtkWidget *reading = gtk_label_new(value ? value : "");
  gtk_widget_set_hexpand(name, TRUE);
  gtk_label_set_xalign(GTK_LABEL(name), 0.0f);
  gtk_label_set_xalign(GTK_LABEL(reading), 1.0f);
  gtk_box_append(GTK_BOX(row), name);
  gtk_box_append(GTK_BOX(row), reading);
  gtk_box_append(self->rows, row);
}

struct _BmChart {
  GtkBox parent_instance;
  GtkLabel *title;
  GtkDrawingArea *plot;
  GtkBox *legend;
  GtkLabel *summary;
  GtkBox *table_box;
  GtkWidget *table;
  BmChartTooltip *tooltip;
  GPtrArray *categories;
  GPtrArray *series;
  int active_index;
  BmThemeScope *scope;
  gulong theme_id;
  char *custom_class;
};

enum {
  ACTIVE_CATEGORY_CHANGED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE(BmChart, bm_chart, GTK_TYPE_BOX)

static const double series_colors[][3] = {
  { 0.04, 0.55, 0.72 },
  { 0.15, 0.65, 0.40 },
  { 0.93, 0.49, 0.10 },
  { 0.85, 0.22, 0.26 },
  { 0.48, 0.37, 0.85 },
};

static guint
plottable_count(BmChart *self, BmChartSeries *series)
{
  return MIN(self->categories->len, series->values->len);
}

static double
x_for_index(BmChart *self, guint index, int width)
{
  double left = 24.0;
  double usable = MAX(1.0, width - 48.0);
  if (self->categories->len < 2)
    return left + usable / 2.0;
  return left + usable * index / (self->categories->len - 1);
}

static void
value_range(BmChart *self, double *minimum, double *maximum)
{
  double low = 0.0;
  double high = 1.0;
  for (guint series_index = 0; series_index < self->series->len;
       series_index++) {
    BmChartSeries *series = g_ptr_array_index(self->series, series_index);
    for (guint point = 0; point < plottable_count(self, series); point++) {
      double value = g_array_index(series->values, double, point);
      low = MIN(low, value);
      high = MAX(high, value);
    }
  }
  if (high == low)
    high = low + 1.0;
  *minimum = low;
  *maximum = high;
}

static double
y_for_value(double value, double minimum, double maximum, int height)
{
  double top = 20.0;
  double usable = MAX(1.0, height - 40.0);
  return top + usable * (maximum - value) / (maximum - minimum);
}

static void
draw_plot(GtkDrawingArea *area,
          cairo_t *cr,
          int width,
          int height,
          gpointer user_data)
{
  (void) area;
  BmChart *self = BM_CHART(user_data);
  double minimum;
  double maximum;
  value_range(self, &minimum, &maximum);

  cairo_set_line_width(cr, 1.0);
  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.22);
  for (guint grid = 0; grid < 5; grid++) {
    double y = 20.0 + (height - 40.0) * grid / 4.0;
    cairo_move_to(cr, 24.0, y);
    cairo_line_to(cr, width - 24.0, y);
  }
  cairo_stroke(cr);

  for (guint series_index = 0; series_index < self->series->len;
       series_index++) {
    BmChartSeries *series = g_ptr_array_index(self->series, series_index);
    guint count = plottable_count(self, series);
    const double *color = series_colors[series_index % G_N_ELEMENTS(series_colors)];
    cairo_set_source_rgb(cr, color[0], color[1], color[2]);
    cairo_set_line_width(cr, 1.5);
    for (guint point = 0; point < count; point++) {
      double x = x_for_index(self, point, width);
      double value = g_array_index(series->values, double, point);
      double y = y_for_value(value, minimum, maximum, height);
      if (point == 0)
        cairo_move_to(cr, x, y);
      else
        cairo_line_to(cr, x, y);
    }
    cairo_stroke(cr);
    for (guint point = 0; point < count; point++) {
      double value = g_array_index(series->values, double, point);
      cairo_arc(cr, x_for_index(self, point, width),
                y_for_value(value, minimum, maximum, height),
                point == (guint) self->active_index ? 3.5 : 2.5,
                0, 2 * G_PI);
      cairo_fill(cr);
    }
  }
}

static void
rebuild_legend(BmChart *self)
{
  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(self->legend));
  while (child) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(self->legend, child);
    child = next;
  }
  for (guint index = 0; index < self->series->len; index++) {
    BmChartSeries *series = g_ptr_array_index(self->series, index);
    GtkWidget *item = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *swatch = gtk_drawing_area_new();
    GtkWidget *label = gtk_label_new(series->label);
    g_autofree char *color_class = g_strdup_printf("bm-series-%u", index % 5);
    gtk_widget_add_css_class(swatch, "bm-chart-legend-swatch");
    gtk_widget_add_css_class(swatch, color_class);
    gtk_widget_set_size_request(swatch, 10, 10);
    gtk_box_append(GTK_BOX(item), swatch);
    gtk_box_append(GTK_BOX(item), label);
    gtk_box_append(self->legend, item);
  }
}

static void
update_active(BmChart *self)
{
  GString *accessible = g_string_new(NULL);
  if (self->active_index >= 0 &&
      self->active_index < (int) self->categories->len) {
    const char *category = g_ptr_array_index(self->categories,
                                              self->active_index);
    bm_chart_tooltip_set_heading(self->tooltip, category);
    bm_chart_tooltip_clear(self->tooltip);
    g_string_append(accessible, category);
    for (guint index = 0; index < self->series->len; index++) {
      BmChartSeries *series = g_ptr_array_index(self->series, index);
      if ((guint) self->active_index >= series->values->len)
        continue;
      const char *value = g_ptr_array_index(series->formatted_values,
                                             self->active_index);
      bm_chart_tooltip_append(self->tooltip, series->label, value);
      g_string_append_printf(accessible, ", %s: %s", series->label, value);
    }
    gtk_accessible_update_property(GTK_ACCESSIBLE(self->plot),
                                   GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   accessible->str, -1);
    if (gtk_widget_get_mapped(GTK_WIDGET(self->plot))) {
      int width = gtk_widget_get_width(GTK_WIDGET(self->plot));
      GdkRectangle rectangle = {
        (int) round(x_for_index(self, self->active_index, width)),
        20,
        1,
        MAX(1, gtk_widget_get_height(GTK_WIDGET(self->plot)) - 40),
      };
      gtk_popover_set_pointing_to(GTK_POPOVER(self->tooltip), &rectangle);
      gtk_popover_popup(GTK_POPOVER(self->tooltip));
    }
  } else {
    gtk_accessible_update_property(GTK_ACCESSIBLE(self->plot),
                                   GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Chart plot. Use arrow keys to inspect categories.",
                                   -1);
    gtk_popover_popdown(GTK_POPOVER(self->tooltip));
  }
  g_string_free(accessible, TRUE);
  gtk_widget_queue_draw(GTK_WIDGET(self->plot));
}

void
bm_chart_set_active_index(BmChart *self, int index)
{
  g_return_if_fail(BM_IS_CHART(self));
  int next = index;
  if (next < -1 || next >= (int) self->categories->len)
    next = -1;
  if (self->active_index == next)
    return;
  self->active_index = next;
  update_active(self);
  g_signal_emit(self, signals[ACTIVE_CATEGORY_CHANGED], 0,
                next,
                next >= 0 ? g_ptr_array_index(self->categories, next) : NULL);
}

static gboolean
on_key_pressed(GtkEventControllerKey *controller,
               guint keyval,
               guint keycode,
               GdkModifierType state,
               gpointer user_data)
{
  (void) controller;
  (void) keycode;
  (void) state;
  BmChart *self = BM_CHART(user_data);
  if (self->categories->len == 0)
    return FALSE;
  gboolean rtl = gtk_widget_get_direction(GTK_WIDGET(self)) == GTK_TEXT_DIR_RTL;
  if (keyval == GDK_KEY_Right || keyval == GDK_KEY_Left) {
    int delta = keyval == GDK_KEY_Right ? 1 : -1;
    if (rtl)
      delta = -delta;
    int next = self->active_index < 0
      ? (delta > 0 ? 0 : (int) self->categories->len - 1)
      : CLAMP(self->active_index + delta, 0,
              (int) self->categories->len - 1);
    bm_chart_set_active_index(self, next);
    return TRUE;
  }
  if (keyval == GDK_KEY_Home) {
    bm_chart_set_active_index(self, 0);
    return TRUE;
  }
  if (keyval == GDK_KEY_End) {
    bm_chart_set_active_index(self, self->categories->len - 1);
    return TRUE;
  }
  if (keyval == GDK_KEY_Escape) {
    bm_chart_set_active_index(self, -1);
    return TRUE;
  }
  return FALSE;
}

static int
index_for_x(BmChart *self, double x)
{
  if (self->categories->len == 0)
    return -1;
  int width = gtk_widget_get_width(GTK_WIDGET(self->plot));
  double usable = MAX(1.0, width - 48.0);
  double position = CLAMP((x - 24.0) / usable, 0.0, 1.0);
  return (int) round(position * (self->categories->len - 1));
}

static void
on_motion(GtkEventControllerMotion *controller,
          double x,
          double y,
          gpointer user_data)
{
  (void) controller;
  (void) y;
  bm_chart_set_active_index(BM_CHART(user_data),
                            index_for_x(BM_CHART(user_data), x));
}

static void
on_leave(GtkEventControllerMotion *controller, gpointer user_data)
{
  (void) controller;
  BmChart *self = BM_CHART(user_data);
  if (!gtk_widget_has_focus(GTK_WIDGET(self->plot)))
    bm_chart_set_active_index(self, -1);
}

static void
on_click(GtkGestureClick *gesture,
         int press_count,
         double x,
         double y,
         gpointer user_data)
{
  (void) gesture;
  (void) press_count;
  (void) y;
  BmChart *self = BM_CHART(user_data);
  gtk_widget_grab_focus(GTK_WIDGET(self->plot));
  bm_chart_set_active_index(self, index_for_x(self, x));
}

static void
apply_theme(BmChart *self)
{
  if (self->scope)
    bm_theme_scope_apply_context_to_widget(self->scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_theme_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  (void) object;
  (void) param;
  apply_theme(BM_CHART(user_data));
}

static void
bm_chart_dispose(GObject *object)
{
  BmChart *self = BM_CHART(object);
  if (self->scope && self->theme_id)
    g_signal_handler_disconnect(self->scope, self->theme_id);
  g_clear_object(&self->scope);
  if (self->tooltip && gtk_widget_get_parent(GTK_WIDGET(self->tooltip)))
    gtk_widget_unparent(GTK_WIDGET(self->tooltip));
  self->tooltip = NULL;
  G_OBJECT_CLASS(bm_chart_parent_class)->dispose(object);
}

static void
bm_chart_finalize(GObject *object)
{
  BmChart *self = BM_CHART(object);
  g_ptr_array_unref(self->categories);
  g_ptr_array_unref(self->series);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_chart_parent_class)->finalize(object);
}

static void
bm_chart_class_init(BmChartClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->dispose = bm_chart_dispose;
  object_class->finalize = bm_chart_finalize;
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_GROUP);
  signals[ACTIVE_CATEGORY_CHANGED] = g_signal_new(
    "active-category-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_STRING);
}

static void
bm_chart_init(BmChart *self)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self),
                                 GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 12);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-chart");
  self->categories = g_ptr_array_new_with_free_func(g_free);
  self->series = g_ptr_array_new_with_free_func(g_object_unref);
  self->active_index = -1;

  self->title = GTK_LABEL(gtk_label_new(NULL));
  gtk_widget_add_css_class(GTK_WIDGET(self->title), "bm-chart-title");
  gtk_label_set_xalign(self->title, 0.0f);

  self->plot = GTK_DRAWING_AREA(gtk_drawing_area_new());
  gtk_widget_add_css_class(GTK_WIDGET(self->plot), "bm-chart-plot");
  gtk_widget_set_size_request(GTK_WIDGET(self->plot), 420, 220);
  gtk_widget_set_hexpand(GTK_WIDGET(self->plot), TRUE);
  gtk_widget_set_focusable(GTK_WIDGET(self->plot), TRUE);
  gtk_drawing_area_set_draw_func(self->plot, draw_plot, self, NULL);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->plot),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "Chart plot. Use arrow keys to inspect categories.",
                                 -1);

  GtkEventController *keys = gtk_event_controller_key_new();
  g_signal_connect(keys, "key-pressed", G_CALLBACK(on_key_pressed), self);
  gtk_widget_add_controller(GTK_WIDGET(self->plot), keys);
  GtkEventController *motion = gtk_event_controller_motion_new();
  g_signal_connect(motion, "motion", G_CALLBACK(on_motion), self);
  g_signal_connect(motion, "leave", G_CALLBACK(on_leave), self);
  gtk_widget_add_controller(GTK_WIDGET(self->plot), motion);
  GtkGesture *click = gtk_gesture_click_new();
  g_signal_connect(click, "pressed", G_CALLBACK(on_click), self);
  gtk_widget_add_controller(GTK_WIDGET(self->plot), GTK_EVENT_CONTROLLER(click));

  self->tooltip = BM_CHART_TOOLTIP(bm_chart_tooltip_new());
  gtk_widget_set_parent(GTK_WIDGET(self->tooltip), GTK_WIDGET(self->plot));

  self->legend = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));
  gtk_widget_add_css_class(GTK_WIDGET(self->legend), "bm-chart-legend");
  self->summary = GTK_LABEL(gtk_label_new(NULL));
  gtk_widget_add_css_class(GTK_WIDGET(self->summary), "bm-chart-summary");
  gtk_label_set_xalign(self->summary, 0.0f);
  gtk_label_set_wrap(self->summary, TRUE);
  self->table_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(self->table_box),
                           "bm-chart-alternative");

  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->title));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->plot));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->legend));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->summary));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->table_box));
  gtk_widget_set_visible(GTK_WIDGET(self->legend), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->table_box), FALSE);
}

GtkWidget *
bm_chart_new(const char *label, const char *summary)
{
  BmChart *self = g_object_new(BM_TYPE_CHART, NULL);
  gtk_label_set_text(self->title, label ? label : "");
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
  bm_chart_set_summary(self, summary);
  return GTK_WIDGET(self);
}

void
bm_chart_append_category(BmChart *self, const char *category)
{
  g_return_if_fail(BM_IS_CHART(self));
  g_ptr_array_add(self->categories, g_strdup(category ? category : ""));
  gtk_widget_queue_draw(GTK_WIDGET(self->plot));
}

void
bm_chart_append_series(BmChart *self, BmChartSeries *series)
{
  g_return_if_fail(BM_IS_CHART(self));
  g_return_if_fail(BM_IS_CHART_SERIES(series));
  g_ptr_array_add(self->series, g_object_ref(series));
  rebuild_legend(self);
  gtk_widget_set_visible(GTK_WIDGET(self->legend), self->series->len > 0);
  gtk_widget_queue_draw(GTK_WIDGET(self->plot));
}

void
bm_chart_clear(BmChart *self)
{
  g_return_if_fail(BM_IS_CHART(self));
  bm_chart_set_active_index(self, -1);
  g_ptr_array_set_size(self->categories, 0);
  g_ptr_array_set_size(self->series, 0);
  rebuild_legend(self);
  gtk_widget_set_visible(GTK_WIDGET(self->legend), FALSE);
  gtk_widget_queue_draw(GTK_WIDGET(self->plot));
}

guint
bm_chart_get_category_count(BmChart *self)
{
  g_return_val_if_fail(BM_IS_CHART(self), 0);
  return self->categories->len;
}

guint
bm_chart_get_series_count(BmChart *self)
{
  g_return_val_if_fail(BM_IS_CHART(self), 0);
  return self->series->len;
}

int
bm_chart_get_active_index(BmChart *self)
{
  g_return_val_if_fail(BM_IS_CHART(self), -1);
  return self->active_index;
}

void
bm_chart_set_summary(BmChart *self, const char *summary)
{
  g_return_if_fail(BM_IS_CHART(self));
  gtk_label_set_text(self->summary, summary ? summary : "");
  gtk_widget_set_visible(GTK_WIDGET(self->summary), summary && *summary);
}

void
bm_chart_set_table(BmChart *self, GtkWidget *table)
{
  g_return_if_fail(BM_IS_CHART(self));
  if (self->table)
    gtk_box_remove(self->table_box, self->table);
  self->table = table;
  if (table)
    gtk_box_append(self->table_box, table);
  gtk_widget_set_visible(GTK_WIDGET(self->table_box), table != NULL);
}

GtkWidget *
bm_chart_get_table(BmChart *self)
{
  g_return_val_if_fail(BM_IS_CHART(self), NULL);
  return self->table;
}

BmChartTooltip *
bm_chart_get_tooltip(BmChart *self)
{
  g_return_val_if_fail(BM_IS_CHART(self), NULL);
  return self->tooltip;
}

void
bm_chart_set_theme_scope(BmChart *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_CHART(self));
  if (self->scope == scope)
    return;
  if (self->scope && self->theme_id)
    g_signal_handler_disconnect(self->scope, self->theme_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self),
                                           &self->custom_class);
  g_set_object(&self->scope, scope);
  self->theme_id = scope
    ? g_signal_connect(scope, "notify", G_CALLBACK(on_theme_changed), self)
    : 0;
  apply_theme(self);
}
