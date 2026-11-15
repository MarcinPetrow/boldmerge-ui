#include "bm-private.h"

struct _BmSelect {
  GtkBox parent_instance;
  GtkLabel *label_widget;
  GtkDropDown *control;
  GtkStringList *labels;
  GtkLabel *error_widget;
  GPtrArray *values;
  char *label;
  char *value;
  char *error;
  gboolean required;
  gboolean valid;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmSelect, bm_select, GTK_TYPE_BOX)

enum {
  PROP_0, PROP_LABEL, PROP_VALUE, PROP_REQUIRED, PROP_ERROR, PROP_VALID,
  PROP_THEME_SCOPE, N_PROPERTIES,
};
enum { SIGNAL_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
apply_theme(BmSelect *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
update_valid(BmSelect *self)
{
  gboolean valid = (self->error == NULL || self->error[0] == '\0') &&
                   (!self->required || (self->value != NULL && self->value[0] != '\0'));
  if (self->valid != valid) {
    self->valid = valid;
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALID]);
  }
  if (valid) gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-invalid");
  else gtk_widget_add_css_class(GTK_WIDGET(self), "bm-invalid");
  gtk_accessible_update_state(GTK_ACCESSIBLE(self->control),
                              GTK_ACCESSIBLE_STATE_INVALID,
                              valid ? GTK_ACCESSIBLE_INVALID_FALSE : GTK_ACCESSIBLE_INVALID_TRUE,
                              -1);
}

static void
on_selected_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  BmSelect *self = BM_SELECT(user_data);
  guint selected = gtk_drop_down_get_selected(self->control);
  const char *value = selected < self->values->len
    ? g_ptr_array_index(self->values, selected) : NULL;
  if (g_strcmp0(self->value, value) == 0) return;
  g_free(self->value);
  self->value = g_strdup(value);
  update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALUE]);
  g_signal_emit(self, signals[SIGNAL_CHANGED], 0, self->value);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_SELECT(user_data));
}

void
bm_select_append(BmSelect *self, const char *text, const char *value)
{
  g_return_if_fail(BM_IS_SELECT(self));
  g_return_if_fail(text != NULL);
  gtk_string_list_append(self->labels, text);
  g_ptr_array_add(self->values, g_strdup(value != NULL ? value : text));
  if (self->values->len == 1)
    gtk_drop_down_set_selected(self->control, 0);
}

void
bm_select_remove_all(BmSelect *self)
{
  g_return_if_fail(BM_IS_SELECT(self));
  gtk_string_list_splice(self->labels, 0, self->values->len, NULL);
  g_ptr_array_set_size(self->values, 0);
  g_clear_pointer(&self->value, g_free);
  gtk_drop_down_set_selected(self->control, GTK_INVALID_LIST_POSITION);
  update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALUE]);
}

const char *bm_select_get_label(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), NULL); return self->label; }
void
bm_select_set_label(BmSelect *self, const char *label)
{
  g_return_if_fail(BM_IS_SELECT(self));
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label); self->label = g_strdup(label);
  g_autofree char *display = self->required && self->label != NULL
    ? g_strdup_printf("%s *", self->label) : g_strdup(self->label);
  gtk_label_set_text(self->label_widget, display != NULL ? display : "");
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->control),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

const char *bm_select_get_value(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), NULL); return self->value; }
void
bm_select_set_value(BmSelect *self, const char *value)
{
  g_return_if_fail(BM_IS_SELECT(self));
  for (guint index = 0; index < self->values->len; index++)
    if (g_strcmp0(g_ptr_array_index(self->values, index), value) == 0) {
      gtk_drop_down_set_selected(self->control, index);
      return;
    }
  gtk_drop_down_set_selected(self->control, GTK_INVALID_LIST_POSITION);
}

gboolean bm_select_get_required(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), FALSE); return self->required; }
void
bm_select_set_required(BmSelect *self, gboolean required)
{
  g_return_if_fail(BM_IS_SELECT(self));
  required = !!required;
  if (self->required == required) return;
  self->required = required;
  const char *label = self->label;
  self->label = NULL;
  bm_select_set_label(self, label);
  g_free((char *) label);
  update_valid(self);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->control),
                                 GTK_ACCESSIBLE_PROPERTY_REQUIRED, required, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_REQUIRED]);
}

const char *bm_select_get_error(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), NULL); return self->error; }
void
bm_select_set_error(BmSelect *self, const char *error)
{
  g_return_if_fail(BM_IS_SELECT(self));
  if (g_strcmp0(self->error, error) == 0) return;
  g_free(self->error); self->error = g_strdup(error);
  gtk_label_set_text(self->error_widget, self->error != NULL ? self->error : "");
  gtk_widget_set_visible(GTK_WIDGET(self->error_widget), self->error != NULL && self->error[0] != '\0');
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->control),
                                 GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, self->error, -1);
  update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ERROR]);
}

gboolean bm_select_get_valid(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), FALSE); return self->valid; }
gboolean bm_select_validate(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), FALSE); update_valid(self); return self->valid; }
GtkDropDown *bm_select_get_control(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), NULL); return self->control; }
BmThemeScope *bm_select_get_theme_scope(BmSelect *self) { g_return_val_if_fail(BM_IS_SELECT(self), NULL); return self->theme_scope; }

void
bm_select_set_theme_scope(BmSelect *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_SELECT(self));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0 : g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
bm_select_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmSelect *self = BM_SELECT(object);
  switch (id) {
  case PROP_LABEL: g_value_set_string(value, self->label); break;
  case PROP_VALUE: g_value_set_string(value, self->value); break;
  case PROP_REQUIRED: g_value_set_boolean(value, self->required); break;
  case PROP_ERROR: g_value_set_string(value, self->error); break;
  case PROP_VALID: g_value_set_boolean(value, self->valid); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_select_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmSelect *self = BM_SELECT(object);
  switch (id) {
  case PROP_LABEL: bm_select_set_label(self, g_value_get_string(value)); break;
  case PROP_VALUE: bm_select_set_value(self, g_value_get_string(value)); break;
  case PROP_REQUIRED: bm_select_set_required(self, g_value_get_boolean(value)); break;
  case PROP_ERROR: bm_select_set_error(self, g_value_get_string(value)); break;
  case PROP_THEME_SCOPE: bm_select_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_select_dispose(GObject *object)
{
  BmSelect *self = BM_SELECT(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0) g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_select_parent_class)->dispose(object);
  self->labels = NULL;
}

static void
bm_select_finalize(GObject *object)
{
  BmSelect *self = BM_SELECT(object);
  g_ptr_array_unref(self->values);
  g_free(self->label); g_free(self->value); g_free(self->error); g_free(self->custom_class);
  G_OBJECT_CLASS(bm_select_parent_class)->finalize(object);
}

static void
bm_select_class_init(BmSelectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_select_get_property; object_class->set_property = bm_select_set_property;
  object_class->dispose = bm_select_dispose; object_class->finalize = bm_select_finalize;
  properties[PROP_LABEL] = g_param_spec_string("label", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VALUE] = g_param_spec_string("value", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_REQUIRED] = g_param_spec_boolean("required", NULL, NULL, FALSE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ERROR] = g_param_spec_string("error", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VALID] = g_param_spec_boolean("valid", NULL, NULL, TRUE, G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_CHANGED] = g_signal_new("changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
bm_select_init(BmSelect *self)
{
  self->valid = TRUE;
  self->values = g_ptr_array_new_with_free_func(g_free);
  self->labels = gtk_string_list_new(NULL);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 6);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-select");
  self->label_widget = GTK_LABEL(gtk_label_new(NULL));
  gtk_label_set_xalign(self->label_widget, 0.0f);
  self->control = GTK_DROP_DOWN(gtk_drop_down_new(G_LIST_MODEL(self->labels), NULL));
  gtk_drop_down_set_selected(self->control, GTK_INVALID_LIST_POSITION);
  self->error_widget = GTK_LABEL(gtk_label_new(NULL));
  gtk_label_set_xalign(self->error_widget, 0.0f);
  gtk_widget_add_css_class(GTK_WIDGET(self->error_widget), "bm-select-error");
  gtk_widget_set_visible(GTK_WIDGET(self->error_widget), FALSE);
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->label_widget));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->control));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->error_widget));
  g_signal_connect(self->control, "notify::selected", G_CALLBACK(on_selected_notify), self);
}

GtkWidget *bm_select_new(const char *label) { return g_object_new(BM_TYPE_SELECT, "label", label, NULL); }
