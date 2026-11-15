#include "bm-private.h"

struct _BmCheckbox {
  GtkCheckButton parent_instance;
  char *text;
  gboolean required;
  gboolean valid;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmCheckbox, bm_checkbox, GTK_TYPE_CHECK_BUTTON)

enum {
  PROP_0,
  PROP_TEXT,
  PROP_CHECKED,
  PROP_INDETERMINATE,
  PROP_REQUIRED,
  PROP_VALID,
  PROP_THEME_SCOPE,
  N_PROPERTIES,
};

static GParamSpec *properties[N_PROPERTIES];

static void
apply_theme(BmCheckbox *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                           GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
update_valid(BmCheckbox *self)
{
  gboolean valid = !self->required || gtk_check_button_get_active(GTK_CHECK_BUTTON(self));
  if (self->valid != valid) {
    self->valid = valid;
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALID]);
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
on_toggled(GtkCheckButton *button, gpointer user_data)
{
  (void) button;
  BmCheckbox *self = BM_CHECKBOX(user_data);
  update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CHECKED]);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_CHECKBOX(user_data));
}

const char *
bm_checkbox_get_text(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), NULL);
  return self->text;
}

void
bm_checkbox_set_text(BmCheckbox *self, const char *text)
{
  g_return_if_fail(BM_IS_CHECKBOX(self));
  if (g_strcmp0(self->text, text) == 0) return;
  g_free(self->text);
  self->text = g_strdup(text);
  gtk_check_button_set_label(GTK_CHECK_BUTTON(self), self->text);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TEXT]);
}

gboolean
bm_checkbox_get_checked(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), FALSE);
  return gtk_check_button_get_active(GTK_CHECK_BUTTON(self));
}

void
bm_checkbox_set_checked(BmCheckbox *self, gboolean checked)
{
  g_return_if_fail(BM_IS_CHECKBOX(self));
  gtk_check_button_set_active(GTK_CHECK_BUTTON(self), !!checked);
}

gboolean
bm_checkbox_get_indeterminate(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), FALSE);
  return gtk_check_button_get_inconsistent(GTK_CHECK_BUTTON(self));
}

void
bm_checkbox_set_indeterminate(BmCheckbox *self, gboolean indeterminate)
{
  g_return_if_fail(BM_IS_CHECKBOX(self));
  indeterminate = !!indeterminate;
  if (bm_checkbox_get_indeterminate(self) == indeterminate) return;
  gtk_check_button_set_inconsistent(GTK_CHECK_BUTTON(self), indeterminate);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_INDETERMINATE]);
}

gboolean
bm_checkbox_get_required(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), FALSE);
  return self->required;
}

void
bm_checkbox_set_required(BmCheckbox *self, gboolean required)
{
  g_return_if_fail(BM_IS_CHECKBOX(self));
  required = !!required;
  if (self->required == required) return;
  self->required = required;
  update_valid(self);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_REQUIRED, required,
                                 -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_REQUIRED]);
}

gboolean
bm_checkbox_get_valid(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), FALSE);
  return self->valid;
}

gboolean
bm_checkbox_validate(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), FALSE);
  update_valid(self);
  return self->valid;
}

BmThemeScope *
bm_checkbox_get_theme_scope(BmCheckbox *self)
{
  g_return_val_if_fail(BM_IS_CHECKBOX(self), NULL);
  return self->theme_scope;
}

void
bm_checkbox_set_theme_scope(BmCheckbox *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_CHECKBOX(self));
  g_return_if_fail(scope == NULL || BM_IS_THEME_SCOPE(scope));
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
bm_checkbox_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmCheckbox *self = BM_CHECKBOX(object);
  switch (id) {
  case PROP_TEXT: g_value_set_string(value, self->text); break;
  case PROP_CHECKED: g_value_set_boolean(value, bm_checkbox_get_checked(self)); break;
  case PROP_INDETERMINATE: g_value_set_boolean(value, bm_checkbox_get_indeterminate(self)); break;
  case PROP_REQUIRED: g_value_set_boolean(value, self->required); break;
  case PROP_VALID: g_value_set_boolean(value, self->valid); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_checkbox_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmCheckbox *self = BM_CHECKBOX(object);
  switch (id) {
  case PROP_TEXT: bm_checkbox_set_text(self, g_value_get_string(value)); break;
  case PROP_CHECKED: bm_checkbox_set_checked(self, g_value_get_boolean(value)); break;
  case PROP_INDETERMINATE: bm_checkbox_set_indeterminate(self, g_value_get_boolean(value)); break;
  case PROP_REQUIRED: bm_checkbox_set_required(self, g_value_get_boolean(value)); break;
  case PROP_THEME_SCOPE: bm_checkbox_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_checkbox_dispose(GObject *object)
{
  BmCheckbox *self = BM_CHECKBOX(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_checkbox_parent_class)->dispose(object);
}

static void
bm_checkbox_finalize(GObject *object)
{
  BmCheckbox *self = BM_CHECKBOX(object);
  g_free(self->text);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_checkbox_parent_class)->finalize(object);
}

static void
bm_checkbox_class_init(BmCheckboxClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_checkbox_get_property;
  object_class->set_property = bm_checkbox_set_property;
  object_class->dispose = bm_checkbox_dispose;
  object_class->finalize = bm_checkbox_finalize;
  properties[PROP_TEXT] = g_param_spec_string("text", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_CHECKED] = g_param_spec_boolean("checked", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_INDETERMINATE] = g_param_spec_boolean("indeterminate", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_REQUIRED] = g_param_spec_boolean("required", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VALID] = g_param_spec_boolean("valid", NULL, NULL, TRUE,
    G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL,
    BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_checkbox_init(BmCheckbox *self)
{
  self->valid = TRUE;
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-checkbox");
  g_signal_connect(self, "toggled", G_CALLBACK(on_toggled), self);
}

GtkWidget *
bm_checkbox_new(const char *text)
{
  return g_object_new(BM_TYPE_CHECKBOX, "text", text, NULL);
}
