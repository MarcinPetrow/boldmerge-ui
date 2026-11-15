#include "bm-private.h"

struct _BmSwitch {
  GtkBox parent_instance;
  GtkSwitch *control;
  GtkLabel *label;
  char *text;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmSwitch, bm_switch, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_TEXT,
  PROP_ACTIVE,
  PROP_THEME_SCOPE,
  N_PROPERTIES,
};

enum { SIGNAL_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
apply_theme(BmSwitch *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                           GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_SWITCH(user_data));
}

static void
on_active_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  BmSwitch *self = BM_SWITCH(user_data);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ACTIVE]);
  g_signal_emit(self, signals[SIGNAL_CHANGED], 0, bm_switch_get_active(self));
}

const char *
bm_switch_get_text(BmSwitch *self)
{
  g_return_val_if_fail(BM_IS_SWITCH(self), NULL);
  return self->text;
}

void
bm_switch_set_text(BmSwitch *self, const char *text)
{
  g_return_if_fail(BM_IS_SWITCH(self));
  if (g_strcmp0(self->text, text) == 0) return;
  g_free(self->text);
  self->text = g_strdup(text);
  gtk_label_set_text(self->label, self->text != NULL ? self->text : "");
  gtk_widget_set_visible(GTK_WIDGET(self->label), self->text != NULL && self->text[0] != '\0');
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->control),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->text,
                                 -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TEXT]);
}

gboolean
bm_switch_get_active(BmSwitch *self)
{
  g_return_val_if_fail(BM_IS_SWITCH(self), FALSE);
  return gtk_switch_get_active(self->control);
}

void
bm_switch_set_active(BmSwitch *self, gboolean active)
{
  g_return_if_fail(BM_IS_SWITCH(self));
  gtk_switch_set_active(self->control, !!active);
}

GtkSwitch *
bm_switch_get_control(BmSwitch *self)
{
  g_return_val_if_fail(BM_IS_SWITCH(self), NULL);
  return self->control;
}

BmThemeScope *
bm_switch_get_theme_scope(BmSwitch *self)
{
  g_return_val_if_fail(BM_IS_SWITCH(self), NULL);
  return self->theme_scope;
}

void
bm_switch_set_theme_scope(BmSwitch *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_SWITCH(self));
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
bm_switch_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmSwitch *self = BM_SWITCH(object);
  switch (id) {
  case PROP_TEXT: g_value_set_string(value, self->text); break;
  case PROP_ACTIVE: g_value_set_boolean(value, bm_switch_get_active(self)); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_switch_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmSwitch *self = BM_SWITCH(object);
  switch (id) {
  case PROP_TEXT: bm_switch_set_text(self, g_value_get_string(value)); break;
  case PROP_ACTIVE: bm_switch_set_active(self, g_value_get_boolean(value)); break;
  case PROP_THEME_SCOPE: bm_switch_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_switch_dispose(GObject *object)
{
  BmSwitch *self = BM_SWITCH(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_switch_parent_class)->dispose(object);
}

static void
bm_switch_finalize(GObject *object)
{
  BmSwitch *self = BM_SWITCH(object);
  g_free(self->text);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_switch_parent_class)->finalize(object);
}

static void
bm_switch_class_init(BmSwitchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_switch_get_property;
  object_class->set_property = bm_switch_set_property;
  object_class->dispose = bm_switch_dispose;
  object_class->finalize = bm_switch_finalize;
  properties[PROP_TEXT] = g_param_spec_string("text", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ACTIVE] = g_param_spec_boolean("active", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL,
    BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_CHANGED] = g_signal_new("changed",
                                         G_TYPE_FROM_CLASS(klass),
                                         G_SIGNAL_RUN_LAST,
                                         0, NULL, NULL, NULL,
                                         G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

static void
bm_switch_init(BmSwitch *self)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_HORIZONTAL);
  gtk_box_set_spacing(GTK_BOX(self), 8);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-switch");
  self->control = GTK_SWITCH(gtk_switch_new());
  self->label = GTK_LABEL(gtk_label_new(NULL));
  gtk_widget_set_valign(GTK_WIDGET(self->control), GTK_ALIGN_CENTER);
  gtk_widget_set_hexpand(GTK_WIDGET(self->label), TRUE);
  gtk_label_set_xalign(self->label, 0.0f);
  gtk_label_set_wrap(self->label, TRUE);
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->control));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->label));
  g_signal_connect(self->control, "notify::active",
                   G_CALLBACK(on_active_notify), self);
}

GtkWidget *
bm_switch_new(const char *text)
{
  return g_object_new(BM_TYPE_SWITCH, "text", text, NULL);
}
