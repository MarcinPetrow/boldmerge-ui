#include "bm-private.h"

struct _BmRadio {
  GtkCheckButton parent_instance;
  char *text;
  char *value;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmRadio, bm_radio, GTK_TYPE_CHECK_BUTTON)

enum {
  RADIO_PROP_0,
  RADIO_PROP_TEXT,
  RADIO_PROP_VALUE,
  RADIO_PROP_CHECKED,
  RADIO_PROP_THEME_SCOPE,
  RADIO_N_PROPERTIES,
};

static GParamSpec *radio_properties[RADIO_N_PROPERTIES];

static void
radio_apply_theme(BmRadio *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                           GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_radio_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  radio_apply_theme(BM_RADIO(user_data));
}

static void
on_radio_toggled(GtkCheckButton *button, gpointer user_data)
{
  (void) button;
  g_object_notify_by_pspec(G_OBJECT(user_data), radio_properties[RADIO_PROP_CHECKED]);
}

const char *
bm_radio_get_text(BmRadio *self)
{
  g_return_val_if_fail(BM_IS_RADIO(self), NULL);
  return self->text;
}

void
bm_radio_set_text(BmRadio *self, const char *text)
{
  g_return_if_fail(BM_IS_RADIO(self));
  if (g_strcmp0(self->text, text) == 0) return;
  g_free(self->text);
  self->text = g_strdup(text);
  gtk_check_button_set_label(GTK_CHECK_BUTTON(self), self->text);
  g_object_notify_by_pspec(G_OBJECT(self), radio_properties[RADIO_PROP_TEXT]);
}

const char *
bm_radio_get_value(BmRadio *self)
{
  g_return_val_if_fail(BM_IS_RADIO(self), NULL);
  return self->value;
}

void
bm_radio_set_value(BmRadio *self, const char *value)
{
  g_return_if_fail(BM_IS_RADIO(self));
  if (g_strcmp0(self->value, value) == 0) return;
  g_free(self->value);
  self->value = g_strdup(value);
  g_object_notify_by_pspec(G_OBJECT(self), radio_properties[RADIO_PROP_VALUE]);
}

gboolean
bm_radio_get_checked(BmRadio *self)
{
  g_return_val_if_fail(BM_IS_RADIO(self), FALSE);
  return gtk_check_button_get_active(GTK_CHECK_BUTTON(self));
}

void
bm_radio_set_checked(BmRadio *self, gboolean checked)
{
  g_return_if_fail(BM_IS_RADIO(self));
  gtk_check_button_set_active(GTK_CHECK_BUTTON(self), !!checked);
}

BmThemeScope *
bm_radio_get_theme_scope(BmRadio *self)
{
  g_return_val_if_fail(BM_IS_RADIO(self), NULL);
  return self->theme_scope;
}

void
bm_radio_set_theme_scope(BmRadio *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_RADIO(self));
  g_return_if_fail(scope == NULL || BM_IS_THEME_SCOPE(scope));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_radio_theme_notify), self);
  radio_apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), radio_properties[RADIO_PROP_THEME_SCOPE]);
}

static void
bm_radio_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmRadio *self = BM_RADIO(object);
  switch (id) {
  case RADIO_PROP_TEXT: g_value_set_string(value, self->text); break;
  case RADIO_PROP_VALUE: g_value_set_string(value, self->value); break;
  case RADIO_PROP_CHECKED: g_value_set_boolean(value, bm_radio_get_checked(self)); break;
  case RADIO_PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_radio_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmRadio *self = BM_RADIO(object);
  switch (id) {
  case RADIO_PROP_TEXT: bm_radio_set_text(self, g_value_get_string(value)); break;
  case RADIO_PROP_VALUE: bm_radio_set_value(self, g_value_get_string(value)); break;
  case RADIO_PROP_CHECKED: bm_radio_set_checked(self, g_value_get_boolean(value)); break;
  case RADIO_PROP_THEME_SCOPE: bm_radio_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_radio_dispose(GObject *object)
{
  BmRadio *self = BM_RADIO(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_radio_parent_class)->dispose(object);
}

static void
bm_radio_finalize(GObject *object)
{
  BmRadio *self = BM_RADIO(object);
  g_free(self->text);
  g_free(self->value);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_radio_parent_class)->finalize(object);
}

static void
bm_radio_class_init(BmRadioClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_radio_get_property;
  object_class->set_property = bm_radio_set_property;
  object_class->dispose = bm_radio_dispose;
  object_class->finalize = bm_radio_finalize;
  radio_properties[RADIO_PROP_TEXT] = g_param_spec_string("text", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  radio_properties[RADIO_PROP_VALUE] = g_param_spec_string("value", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  radio_properties[RADIO_PROP_CHECKED] = g_param_spec_boolean("checked", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  radio_properties[RADIO_PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL,
    BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, RADIO_N_PROPERTIES, radio_properties);
}

static void
bm_radio_init(BmRadio *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-radio");
  g_signal_connect(self, "toggled", G_CALLBACK(on_radio_toggled), self);
}

GtkWidget *
bm_radio_new(const char *text, const char *value)
{
  return g_object_new(BM_TYPE_RADIO, "text", text, "value", value, NULL);
}

struct _BmRadioGroup {
  GtkBox parent_instance;
  GtkLabel *label_widget;
  GtkBox *options;
  GPtrArray *radios;
  char *label;
  char *value;
  gboolean required;
  gboolean valid;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmRadioGroup, bm_radio_group, GTK_TYPE_BOX)

enum {
  GROUP_PROP_0,
  GROUP_PROP_LABEL,
  GROUP_PROP_VALUE,
  GROUP_PROP_REQUIRED,
  GROUP_PROP_VALID,
  GROUP_PROP_THEME_SCOPE,
  GROUP_N_PROPERTIES,
};

enum { GROUP_SIGNAL_CHANGED, GROUP_N_SIGNALS };
static GParamSpec *group_properties[GROUP_N_PROPERTIES];
static guint group_signals[GROUP_N_SIGNALS];

static void
group_update_label(BmRadioGroup *self)
{
  g_autofree char *display = self->required && self->label != NULL
    ? g_strdup_printf("%s *", self->label)
    : g_strdup(self->label);
  gtk_label_set_text(self->label_widget, display != NULL ? display : "");
  gtk_widget_set_visible(GTK_WIDGET(self->label_widget), display != NULL && display[0] != '\0');
}

static void
group_update_valid(BmRadioGroup *self)
{
  gboolean valid = !self->required || (self->value != NULL && self->value[0] != '\0');
  if (self->valid != valid) {
    self->valid = valid;
    g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_VALID]);
  }
  if (valid)
    gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-invalid");
  else
    gtk_widget_add_css_class(GTK_WIDGET(self), "bm-invalid");
  gtk_accessible_update_state(GTK_ACCESSIBLE(self),
                              GTK_ACCESSIBLE_STATE_INVALID,
                              valid ? GTK_ACCESSIBLE_INVALID_FALSE
                                    : GTK_ACCESSIBLE_INVALID_TRUE,
                              -1);
}

static void
group_apply_theme(BmRadioGroup *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                           GTK_WIDGET(self),
                                           &self->custom_class);
  for (guint index = 0; index < self->radios->len; index++)
    bm_radio_set_theme_scope(g_ptr_array_index(self->radios, index), self->theme_scope);
}

static void
on_group_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  group_apply_theme(BM_RADIO_GROUP(user_data));
}

static void
on_group_radio_toggled(GtkCheckButton *button, gpointer user_data)
{
  if (!gtk_check_button_get_active(button)) return;
  BmRadioGroup *self = BM_RADIO_GROUP(user_data);
  BmRadio *radio = BM_RADIO(button);
  if (g_strcmp0(self->value, bm_radio_get_value(radio)) == 0) return;
  g_free(self->value);
  self->value = g_strdup(bm_radio_get_value(radio));
  group_update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_VALUE]);
  g_signal_emit(self, group_signals[GROUP_SIGNAL_CHANGED], 0, self->value);
}

void
bm_radio_group_append(BmRadioGroup *self, BmRadio *radio)
{
  g_return_if_fail(BM_IS_RADIO_GROUP(self));
  g_return_if_fail(BM_IS_RADIO(radio));
  if (self->radios->len > 0) {
    BmRadio *first = g_ptr_array_index(self->radios, 0);
    gtk_check_button_set_group(GTK_CHECK_BUTTON(radio), GTK_CHECK_BUTTON(first));
  }
  bm_radio_set_theme_scope(radio, self->theme_scope);
  g_ptr_array_add(self->radios, radio);
  g_signal_connect(radio, "toggled", G_CALLBACK(on_group_radio_toggled), self);
  gtk_box_append(self->options, GTK_WIDGET(radio));
  if (bm_radio_get_checked(radio))
    on_group_radio_toggled(GTK_CHECK_BUTTON(radio), self);
}

const char *
bm_radio_group_get_label(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), NULL);
  return self->label;
}

void
bm_radio_group_set_label(BmRadioGroup *self, const char *label)
{
  g_return_if_fail(BM_IS_RADIO_GROUP(self));
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  group_update_label(self);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label,
                                 -1);
  g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_LABEL]);
}

const char *
bm_radio_group_get_value(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), NULL);
  return self->value;
}

void
bm_radio_group_set_value(BmRadioGroup *self, const char *value)
{
  g_return_if_fail(BM_IS_RADIO_GROUP(self));
  for (guint index = 0; index < self->radios->len; index++) {
    BmRadio *radio = g_ptr_array_index(self->radios, index);
    if (g_strcmp0(bm_radio_get_value(radio), value) == 0) {
      bm_radio_set_checked(radio, TRUE);
      return;
    }
  }
}

gboolean
bm_radio_group_get_required(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), FALSE);
  return self->required;
}

void
bm_radio_group_set_required(BmRadioGroup *self, gboolean required)
{
  g_return_if_fail(BM_IS_RADIO_GROUP(self));
  required = !!required;
  if (self->required == required) return;
  self->required = required;
  group_update_label(self);
  group_update_valid(self);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_REQUIRED, required,
                                 -1);
  g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_REQUIRED]);
}

gboolean
bm_radio_group_get_valid(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), FALSE);
  return self->valid;
}

gboolean
bm_radio_group_validate(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), FALSE);
  group_update_valid(self);
  return self->valid;
}

BmThemeScope *
bm_radio_group_get_theme_scope(BmRadioGroup *self)
{
  g_return_val_if_fail(BM_IS_RADIO_GROUP(self), NULL);
  return self->theme_scope;
}

void
bm_radio_group_set_theme_scope(BmRadioGroup *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_RADIO_GROUP(self));
  g_return_if_fail(scope == NULL || BM_IS_THEME_SCOPE(scope));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_group_theme_notify), self);
  group_apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_THEME_SCOPE]);
}

static void
bm_radio_group_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmRadioGroup *self = BM_RADIO_GROUP(object);
  switch (id) {
  case GROUP_PROP_LABEL: g_value_set_string(value, self->label); break;
  case GROUP_PROP_VALUE: g_value_set_string(value, self->value); break;
  case GROUP_PROP_REQUIRED: g_value_set_boolean(value, self->required); break;
  case GROUP_PROP_VALID: g_value_set_boolean(value, self->valid); break;
  case GROUP_PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_radio_group_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmRadioGroup *self = BM_RADIO_GROUP(object);
  switch (id) {
  case GROUP_PROP_LABEL: bm_radio_group_set_label(self, g_value_get_string(value)); break;
  case GROUP_PROP_VALUE: bm_radio_group_set_value(self, g_value_get_string(value)); break;
  case GROUP_PROP_REQUIRED: bm_radio_group_set_required(self, g_value_get_boolean(value)); break;
  case GROUP_PROP_THEME_SCOPE: bm_radio_group_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_radio_group_dispose(GObject *object)
{
  BmRadioGroup *self = BM_RADIO_GROUP(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_radio_group_parent_class)->dispose(object);
}

static void
bm_radio_group_finalize(GObject *object)
{
  BmRadioGroup *self = BM_RADIO_GROUP(object);
  g_ptr_array_unref(self->radios);
  g_free(self->label);
  g_free(self->value);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_radio_group_parent_class)->finalize(object);
}

static void
bm_radio_group_class_init(BmRadioGroupClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_radio_group_get_property;
  object_class->set_property = bm_radio_group_set_property;
  object_class->dispose = bm_radio_group_dispose;
  object_class->finalize = bm_radio_group_finalize;
  gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_RADIO_GROUP);
  group_properties[GROUP_PROP_LABEL] = g_param_spec_string("label", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  group_properties[GROUP_PROP_VALUE] = g_param_spec_string("value", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  group_properties[GROUP_PROP_REQUIRED] = g_param_spec_boolean("required", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  group_properties[GROUP_PROP_VALID] = g_param_spec_boolean("valid", NULL, NULL, TRUE,
    G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  group_properties[GROUP_PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL,
    BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, GROUP_N_PROPERTIES, group_properties);
  group_signals[GROUP_SIGNAL_CHANGED] = g_signal_new("changed",
                                                     G_TYPE_FROM_CLASS(klass),
                                                     G_SIGNAL_RUN_LAST,
                                                     0, NULL, NULL, NULL,
                                                     G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
bm_radio_group_init(BmRadioGroup *self)
{
  self->valid = TRUE;
  self->radios = g_ptr_array_new();
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 8);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-radio-group");
  self->label_widget = GTK_LABEL(gtk_label_new(NULL));
  gtk_label_set_xalign(self->label_widget, 0.0f);
  gtk_widget_add_css_class(GTK_WIDGET(self->label_widget), "heading");
  self->options = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->label_widget));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->options));
}

GtkWidget *
bm_radio_group_new(const char *label)
{
  return g_object_new(BM_TYPE_RADIO_GROUP, "label", label, NULL);
}
