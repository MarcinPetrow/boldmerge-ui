#include "bm-private.h"

GType
bm_validation_focus_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_VALIDATION_FOCUS_FIRST, "BM_VALIDATION_FOCUS_FIRST", "first" },
      { BM_VALIDATION_FOCUS_SUMMARY, "BM_VALIDATION_FOCUS_SUMMARY", "summary" },
      { BM_VALIDATION_FOCUS_NONE, "BM_VALIDATION_FOCUS_NONE", "none" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmValidationFocus", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

typedef struct {
  GtkWidget *control;
  char *label;
  char *message;
  gulong notify_id;
} ValidationEntry;

struct _BmValidationSummary {
  GtkWidget parent_instance;
  GtkBox *frame;
  GtkLabel *heading_label;
  GtkBox *list;
  GPtrArray *entries;
  char *heading;
  guint error_count;
  gboolean refreshing;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmValidationSummary, bm_validation_summary, GTK_TYPE_WIDGET)

enum { PROP_0, PROP_HEADING, PROP_ERROR_COUNT, PROP_THEME_SCOPE, N_PROPERTIES };
enum { SIGNAL_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
validation_entry_free(gpointer data)
{
  ValidationEntry *entry = data;
  if (entry->control != NULL && entry->notify_id != 0)
    g_signal_handler_disconnect(entry->control, entry->notify_id);
  g_clear_object(&entry->control);
  g_free(entry->label);
  g_free(entry->message);
  g_free(entry);
}

static gboolean
control_is_valid(GtkWidget *control)
{
  if (BM_IS_INPUT(control)) return bm_input_get_valid(BM_INPUT(control));
  if (BM_IS_SELECT(control)) return bm_select_get_valid(BM_SELECT(control));
  if (BM_IS_COMBOBOX(control))
    return bm_combobox_get_valid(BM_COMBOBOX(control));
  if (BM_IS_CHECKBOX(control)) return bm_checkbox_get_valid(BM_CHECKBOX(control));
  if (BM_IS_RADIO_GROUP(control))
    return bm_radio_group_get_valid(BM_RADIO_GROUP(control));
  return TRUE;
}

static gboolean
control_validate(GtkWidget *control)
{
  if (BM_IS_INPUT(control)) return bm_input_validate(BM_INPUT(control));
  if (BM_IS_SELECT(control)) return bm_select_validate(BM_SELECT(control));
  if (BM_IS_COMBOBOX(control))
    return bm_combobox_validate(BM_COMBOBOX(control));
  if (BM_IS_CHECKBOX(control))
    return bm_checkbox_validate(BM_CHECKBOX(control));
  if (BM_IS_RADIO_GROUP(control))
    return bm_radio_group_validate(BM_RADIO_GROUP(control));
  return TRUE;
}

static const char *
control_label(ValidationEntry *entry)
{
  if (entry->label != NULL && *entry->label != '\0') return entry->label;
  if (BM_IS_INPUT(entry->control)) return bm_input_get_label(BM_INPUT(entry->control));
  if (BM_IS_SELECT(entry->control)) return bm_select_get_label(BM_SELECT(entry->control));
  if (BM_IS_COMBOBOX(entry->control))
    return bm_combobox_get_label(BM_COMBOBOX(entry->control));
  if (BM_IS_CHECKBOX(entry->control))
    return bm_checkbox_get_text(BM_CHECKBOX(entry->control));
  if (BM_IS_RADIO_GROUP(entry->control))
    return bm_radio_group_get_label(BM_RADIO_GROUP(entry->control));
  return "Field";
}

static const char *
control_message(ValidationEntry *entry)
{
  if (entry->message != NULL && *entry->message != '\0') return entry->message;
  if (BM_IS_INPUT(entry->control)) {
    const char *error = bm_input_get_error(BM_INPUT(entry->control));
    return error != NULL && *error != '\0' ? error : "Enter a value.";
  }
  if (BM_IS_SELECT(entry->control)) {
    const char *error = bm_select_get_error(BM_SELECT(entry->control));
    return error != NULL && *error != '\0' ? error : "Select a value.";
  }
  if (BM_IS_COMBOBOX(entry->control)) {
    const char *error = bm_combobox_get_error(BM_COMBOBOX(entry->control));
    return error != NULL && *error != '\0' ? error : "Select a value.";
  }
  if (BM_IS_CHECKBOX(entry->control)) return "This option is required.";
  if (BM_IS_RADIO_GROUP(entry->control)) return "Select an option.";
  return "Check this field.";
}

static void
focus_control(GtkWidget *control)
{
  if (BM_IS_INPUT(control))
    gtk_widget_grab_focus(GTK_WIDGET(bm_input_get_entry(BM_INPUT(control))));
  else if (BM_IS_SELECT(control))
    gtk_widget_grab_focus(GTK_WIDGET(bm_select_get_control(BM_SELECT(control))));
  else if (BM_IS_COMBOBOX(control))
    gtk_widget_grab_focus(GTK_WIDGET(
      bm_combobox_get_control(BM_COMBOBOX(control))));
  else if (!gtk_widget_grab_focus(control))
    gtk_widget_child_focus(control, GTK_DIR_TAB_FORWARD);
}

static void
on_error_clicked(GtkButton *button, gpointer data)
{
  (void) button;
  focus_control(((ValidationEntry *) data)->control);
}

static void
clear_list(BmValidationSummary *self)
{
  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(self->list));
  while (child != NULL) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(self->list, child);
    child = next;
  }
}

void
bm_validation_summary_refresh(BmValidationSummary *self)
{
  g_return_if_fail(BM_IS_VALIDATION_SUMMARY(self));
  if (self->refreshing) return;
  self->refreshing = TRUE;
  guint previous = self->error_count;
  self->error_count = 0;
  clear_list(self);
  for (guint i = 0; i < self->entries->len; i++) {
    ValidationEntry *entry = g_ptr_array_index(self->entries, i);
    if (control_is_valid(entry->control)) continue;
    self->error_count++;
    g_autofree char *text = g_strdup_printf("%s: %s",
      control_label(entry), control_message(entry));
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_widget_add_css_class(button, "bm-validation-link");
    gtk_widget_set_halign(button, GTK_ALIGN_START);
    g_signal_connect(button, "clicked", G_CALLBACK(on_error_clicked), entry);
    gtk_box_append(self->list, button);
  }
  gtk_widget_set_visible(GTK_WIDGET(self), self->error_count > 0);
  if (previous != self->error_count) {
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ERROR_COUNT]);
    g_signal_emit(self, signals[SIGNAL_CHANGED], 0, self->error_count == 0,
                  self->error_count);
  }
  self->refreshing = FALSE;
}

static void
on_control_valid_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  bm_validation_summary_refresh(BM_VALIDATION_SUMMARY(data));
}

void
bm_validation_summary_add(BmValidationSummary *self, GtkWidget *control,
                          const char *label, const char *message)
{
  g_return_if_fail(BM_IS_VALIDATION_SUMMARY(self));
  g_return_if_fail(BM_IS_INPUT(control) || BM_IS_SELECT(control) ||
                   BM_IS_COMBOBOX(control) ||
                   BM_IS_CHECKBOX(control) || BM_IS_RADIO_GROUP(control));
  ValidationEntry *entry = g_new0(ValidationEntry, 1);
  entry->control = g_object_ref(control);
  entry->label = g_strdup(label);
  entry->message = g_strdup(message);
  entry->notify_id = g_signal_connect(control, "notify::valid",
                                      G_CALLBACK(on_control_valid_notify), self);
  g_ptr_array_add(self->entries, entry);
  bm_validation_summary_refresh(self);
}

void
bm_validation_summary_remove_all(BmValidationSummary *self)
{
  g_return_if_fail(BM_IS_VALIDATION_SUMMARY(self));
  g_ptr_array_set_size(self->entries, 0);
  bm_validation_summary_refresh(self);
}

gboolean
bm_validation_summary_validate(BmValidationSummary *self,
                               BmValidationFocus focus)
{
  g_return_val_if_fail(BM_IS_VALIDATION_SUMMARY(self), TRUE);
  gboolean valid = TRUE;
  for (guint i = 0; i < self->entries->len; i++) {
    ValidationEntry *entry = g_ptr_array_index(self->entries, i);
    if (!control_validate(entry->control)) valid = FALSE;
  }
  bm_validation_summary_refresh(self);
  if (!valid) {
    if (focus == BM_VALIDATION_FOCUS_SUMMARY)
      gtk_widget_grab_focus(GTK_WIDGET(self));
    else if (focus == BM_VALIDATION_FOCUS_FIRST)
      bm_validation_summary_focus_first_invalid(self);
  }
  return valid;
}

guint
bm_validation_summary_get_error_count(BmValidationSummary *self)
{
  g_return_val_if_fail(BM_IS_VALIDATION_SUMMARY(self), 0);
  return self->error_count;
}

void
bm_validation_summary_focus_first_invalid(BmValidationSummary *self)
{
  g_return_if_fail(BM_IS_VALIDATION_SUMMARY(self));
  for (guint i = 0; i < self->entries->len; i++) {
    ValidationEntry *entry = g_ptr_array_index(self->entries, i);
    if (!control_is_valid(entry->control)) {
      focus_control(entry->control);
      return;
    }
  }
}

static void
apply_theme(BmValidationSummary *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_VALIDATION_SUMMARY(data));
}

void
bm_validation_summary_set_theme_scope(BmValidationSummary *self,
                                      BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_VALIDATION_SUMMARY(self));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
summary_set_heading(BmValidationSummary *self, const char *heading)
{
  if (g_strcmp0(self->heading, heading) == 0) return;
  g_free(self->heading);
  self->heading = g_strdup(heading);
  gtk_label_set_text(self->heading_label,
                     self->heading != NULL ? self->heading : "");
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_HEADING]);
}

static void
bm_validation_summary_get_property(GObject *object, guint id, GValue *value,
                                   GParamSpec *pspec)
{
  BmValidationSummary *self = BM_VALIDATION_SUMMARY(object);
  if (id == PROP_HEADING) g_value_set_string(value, self->heading);
  else if (id == PROP_ERROR_COUNT) g_value_set_uint(value, self->error_count);
  else if (id == PROP_THEME_SCOPE) g_value_set_object(value, self->theme_scope);
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_validation_summary_set_property(GObject *object, guint id,
                                   const GValue *value, GParamSpec *pspec)
{
  if (id == PROP_HEADING)
    summary_set_heading(BM_VALIDATION_SUMMARY(object),
                        g_value_get_string(value));
  else if (id == PROP_THEME_SCOPE)
    bm_validation_summary_set_theme_scope(BM_VALIDATION_SUMMARY(object),
                                          g_value_get_object(value));
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_validation_summary_dispose(GObject *object)
{
  BmValidationSummary *self = BM_VALIDATION_SUMMARY(object);
  g_ptr_array_set_size(self->entries, 0);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  if (self->frame != NULL) {
    gtk_widget_unparent(GTK_WIDGET(self->frame));
    self->frame = NULL;
  }
  G_OBJECT_CLASS(bm_validation_summary_parent_class)->dispose(object);
}

static void
bm_validation_summary_finalize(GObject *object)
{
  BmValidationSummary *self = BM_VALIDATION_SUMMARY(object);
  g_ptr_array_unref(self->entries);
  g_free(self->heading);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_validation_summary_parent_class)->finalize(object);
}

static void
bm_validation_summary_class_init(BmValidationSummaryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_validation_summary_get_property;
  object_class->set_property = bm_validation_summary_set_property;
  object_class->dispose = bm_validation_summary_dispose;
  object_class->finalize = bm_validation_summary_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_ALERT);
  properties[PROP_HEADING] = g_param_spec_string(
    "heading", NULL, NULL, "Correct the following errors",
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ERROR_COUNT] = g_param_spec_uint(
    "error-count", NULL, NULL, 0, G_MAXUINT, 0,
    G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object(
    "theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_CHANGED] = g_signal_new(
    "changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
    G_TYPE_NONE, 2, G_TYPE_BOOLEAN, G_TYPE_UINT);
}

static void
bm_validation_summary_init(BmValidationSummary *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-validation-summary");
  gtk_widget_set_focusable(GTK_WIDGET(self), TRUE);
  self->entries = g_ptr_array_new_with_free_func(validation_entry_free);
  self->frame = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
  self->heading_label = GTK_LABEL(gtk_label_new(NULL));
  gtk_widget_add_css_class(GTK_WIDGET(self->heading_label),
                           "bm-validation-heading");
  gtk_label_set_xalign(self->heading_label, 0.0f);
  self->list = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
  gtk_box_append(self->frame, GTK_WIDGET(self->heading_label));
  gtk_box_append(self->frame, GTK_WIDGET(self->list));
  gtk_widget_set_parent(GTK_WIDGET(self->frame), GTK_WIDGET(self));
  gtk_widget_set_visible(GTK_WIDGET(self), FALSE);
}

GtkWidget *
bm_validation_summary_new(const char *heading)
{
  return g_object_new(BM_TYPE_VALIDATION_SUMMARY, "heading",
                      heading != NULL ? heading : "Correct the following errors",
                      NULL);
}
