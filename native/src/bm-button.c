#include "bm-private.h"

struct _BmButton {
  GtkButton parent_instance;
  GtkBox *layout;
  GtkSpinner *spinner;
  GtkWidget *content;
  char *text;
  BmButtonVariant variant;
  BmButtonTreatment treatment;
  gboolean loading;
  gboolean restore_sensitive;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmButton, bm_button, GTK_TYPE_BUTTON)

enum {
  PROP_0,
  PROP_TEXT,
  PROP_VARIANT,
  PROP_TREATMENT,
  PROP_LOADING,
  PROP_CONTENT,
  PROP_THEME_SCOPE,
  N_PROPERTIES,
};

static GParamSpec *properties[N_PROPERTIES];

GType
bm_button_variant_get_type(void)
{
  static gsize type = 0;
  static const GEnumValue values[] = {
    { BM_BUTTON_VARIANT_PRIMARY, "BM_BUTTON_VARIANT_PRIMARY", "primary" },
    { BM_BUTTON_VARIANT_SECONDARY, "BM_BUTTON_VARIANT_SECONDARY", "secondary" },
    { BM_BUTTON_VARIANT_OUTLINE, "BM_BUTTON_VARIANT_OUTLINE", "outline" },
    { BM_BUTTON_VARIANT_GHOST, "BM_BUTTON_VARIANT_GHOST", "ghost" },
    { 0, NULL, NULL },
  };
  if (g_once_init_enter(&type)) {
    GType registered = g_enum_register_static("BmButtonVariant", values);
    g_once_init_leave(&type, registered);
  }
  return (GType) type;
}

GType
bm_button_treatment_get_type(void)
{
  static gsize type = 0;
  static const GEnumValue values[] = {
    { BM_BUTTON_TREATMENT_DEFAULT, "BM_BUTTON_TREATMENT_DEFAULT", "default" },
    { BM_BUTTON_TREATMENT_DESTRUCTIVE, "BM_BUTTON_TREATMENT_DESTRUCTIVE", "destructive" },
    { 0, NULL, NULL },
  };
  if (g_once_init_enter(&type)) {
    GType registered = g_enum_register_static("BmButtonTreatment", values);
    g_once_init_leave(&type, registered);
  }
  return (GType) type;
}

static void
apply_appearance(BmButton *self)
{
  static const char *variants[] = { "bm-primary", "bm-secondary", "bm-outline", "bm-ghost" };
  for (guint i = 0; i < G_N_ELEMENTS(variants); i++)
    gtk_widget_remove_css_class(GTK_WIDGET(self), variants[i]);
  gtk_widget_add_css_class(GTK_WIDGET(self), variants[self->variant]);
  if (self->treatment == BM_BUTTON_TREATMENT_DESTRUCTIVE)
    gtk_widget_add_css_class(GTK_WIDGET(self), "bm-destructive");
  else
    gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-destructive");
}

static void
apply_theme(BmButton *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self), &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_BUTTON(user_data));
}

const char *
bm_button_get_text(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), NULL);
  return self->text;
}

void
bm_button_set_content(BmButton *self, GtkWidget *content)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  g_return_if_fail(content == NULL || GTK_IS_WIDGET(content));
  if (self->content == content) return;
  if (self->content != NULL) gtk_box_remove(self->layout, self->content);
  self->content = content;
  if (content != NULL) gtk_box_append(self->layout, content);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CONTENT]);
}

GtkWidget *
bm_button_get_content(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), NULL);
  return self->content;
}

void
bm_button_set_text(BmButton *self, const char *text)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  if (g_strcmp0(self->text, text) == 0) return;
  g_free(self->text);
  self->text = g_strdup(text);
  GtkWidget *label = gtk_label_new(self->text != NULL ? self->text : "");
  gtk_label_set_xalign(GTK_LABEL(label), 0.5f);
  bm_button_set_content(self, label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->text,
                                 -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TEXT]);
}

BmButtonVariant
bm_button_get_variant(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), BM_BUTTON_VARIANT_PRIMARY);
  return self->variant;
}

void
bm_button_set_variant(BmButton *self, BmButtonVariant variant)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  g_return_if_fail(variant >= BM_BUTTON_VARIANT_PRIMARY && variant <= BM_BUTTON_VARIANT_GHOST);
  if (self->variant == variant) return;
  self->variant = variant;
  apply_appearance(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VARIANT]);
}

BmButtonTreatment
bm_button_get_treatment(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), BM_BUTTON_TREATMENT_DEFAULT);
  return self->treatment;
}

void
bm_button_set_treatment(BmButton *self, BmButtonTreatment treatment)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  g_return_if_fail(treatment >= BM_BUTTON_TREATMENT_DEFAULT && treatment <= BM_BUTTON_TREATMENT_DESTRUCTIVE);
  if (self->treatment == treatment) return;
  self->treatment = treatment;
  apply_appearance(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TREATMENT]);
}

gboolean
bm_button_get_loading(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), FALSE);
  return self->loading;
}

void
bm_button_set_loading(BmButton *self, gboolean loading)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  loading = !!loading;
  if (self->loading == loading) return;
  if (loading) {
    self->restore_sensitive = gtk_widget_get_sensitive(GTK_WIDGET(self));
    gtk_widget_set_sensitive(GTK_WIDGET(self), FALSE);
    gtk_spinner_start(self->spinner);
  } else {
    gtk_spinner_stop(self->spinner);
    gtk_widget_set_sensitive(GTK_WIDGET(self), self->restore_sensitive);
  }
  self->loading = loading;
  gtk_widget_set_visible(GTK_WIDGET(self->spinner), loading);
  if (loading) gtk_widget_add_css_class(GTK_WIDGET(self), "bm-loading");
  else gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-loading");
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LOADING]);
}

BmThemeScope *
bm_button_get_theme_scope(BmButton *self)
{
  g_return_val_if_fail(BM_IS_BUTTON(self), NULL);
  return self->theme_scope;
}

void
bm_button_set_theme_scope(BmButton *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_BUTTON(self));
  g_return_if_fail(scope == NULL || BM_IS_THEME_SCOPE(scope));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0 :
    g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
bm_button_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmButton *self = BM_BUTTON(object);
  switch (id) {
  case PROP_TEXT: g_value_set_string(value, self->text); break;
  case PROP_VARIANT: g_value_set_enum(value, self->variant); break;
  case PROP_TREATMENT: g_value_set_enum(value, self->treatment); break;
  case PROP_LOADING: g_value_set_boolean(value, self->loading); break;
  case PROP_CONTENT: g_value_set_object(value, self->content); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_button_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmButton *self = BM_BUTTON(object);
  switch (id) {
  case PROP_TEXT: bm_button_set_text(self, g_value_get_string(value)); break;
  case PROP_VARIANT: bm_button_set_variant(self, g_value_get_enum(value)); break;
  case PROP_TREATMENT: bm_button_set_treatment(self, g_value_get_enum(value)); break;
  case PROP_LOADING: bm_button_set_loading(self, g_value_get_boolean(value)); break;
  case PROP_CONTENT: bm_button_set_content(self, g_value_get_object(value)); break;
  case PROP_THEME_SCOPE: bm_button_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_button_dispose(GObject *object)
{
  BmButton *self = BM_BUTTON(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_button_parent_class)->dispose(object);
}

static void
bm_button_finalize(GObject *object)
{
  BmButton *self = BM_BUTTON(object);
  g_free(self->text);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_button_parent_class)->finalize(object);
}

static void
bm_button_class_init(BmButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_button_get_property;
  object_class->set_property = bm_button_set_property;
  object_class->dispose = bm_button_dispose;
  object_class->finalize = bm_button_finalize;
  properties[PROP_TEXT] = g_param_spec_string("text", NULL, NULL, NULL,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VARIANT] = g_param_spec_enum("variant", NULL, NULL, BM_TYPE_BUTTON_VARIANT,
                                               BM_BUTTON_VARIANT_PRIMARY,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_TREATMENT] = g_param_spec_enum("treatment", NULL, NULL, BM_TYPE_BUTTON_TREATMENT,
                                                 BM_BUTTON_TREATMENT_DEFAULT,
                                                 G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_LOADING] = g_param_spec_boolean("loading", NULL, NULL, FALSE,
                                                  G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_CONTENT] = g_param_spec_object("content", NULL, NULL, GTK_TYPE_WIDGET,
                                                 G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
                                                     G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_button_init(BmButton *self)
{
  self->variant = BM_BUTTON_VARIANT_PRIMARY;
  self->treatment = BM_BUTTON_TREATMENT_DEFAULT;
  self->restore_sensitive = TRUE;
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-button");
  self->layout = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
  gtk_widget_set_halign(GTK_WIDGET(self->layout), GTK_ALIGN_CENTER);
  self->spinner = GTK_SPINNER(gtk_spinner_new());
  gtk_widget_set_visible(GTK_WIDGET(self->spinner), FALSE);
  gtk_box_append(self->layout, GTK_WIDGET(self->spinner));
  gtk_button_set_child(GTK_BUTTON(self), GTK_WIDGET(self->layout));
  apply_appearance(self);
}

GtkWidget *
bm_button_new(const char *text)
{
  return g_object_new(BM_TYPE_BUTTON, "text", text, NULL);
}
