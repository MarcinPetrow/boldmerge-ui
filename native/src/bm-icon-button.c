#include "bm-private.h"

struct _BmIconButton {
  GtkButton parent_instance;
  GtkBox *layout;
  GtkSpinner *spinner;
  GtkWidget *icon;
  char *label;
  BmButtonVariant variant;
  BmButtonTreatment treatment;
  gboolean loading;
  gboolean restore_sensitive;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmIconButton, bm_icon_button, GTK_TYPE_BUTTON)

enum { PROP_0, PROP_LABEL, PROP_ICON, PROP_VARIANT, PROP_TREATMENT, PROP_LOADING, PROP_THEME_SCOPE, N_PROPERTIES };
static GParamSpec *properties[N_PROPERTIES];

static void
apply_appearance(BmIconButton *self)
{
  static const char *variants[] = { "bm-primary", "bm-secondary", "bm-outline", "bm-ghost" };
  for (guint i = 0; i < G_N_ELEMENTS(variants); i++) gtk_widget_remove_css_class(GTK_WIDGET(self), variants[i]);
  gtk_widget_add_css_class(GTK_WIDGET(self), variants[self->variant]);
  if (self->treatment == BM_BUTTON_TREATMENT_DESTRUCTIVE) gtk_widget_add_css_class(GTK_WIDGET(self), "bm-destructive");
  else gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-destructive");
}

static void apply_theme(BmIconButton *self) { if (self->theme_scope != NULL) bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self), &self->custom_class); }
static void on_theme_notify(GObject *object, GParamSpec *pspec, gpointer data) { (void) object; (void) pspec; apply_theme(BM_ICON_BUTTON(data)); }

const char *bm_icon_button_get_label(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), NULL); return self->label; }
void
bm_icon_button_set_label(BmIconButton *self, const char *label)
{
  g_return_if_fail(BM_IS_ICON_BUTTON(self));
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label); self->label = g_strdup(label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self), GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

GtkWidget *bm_icon_button_get_icon(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), NULL); return self->icon; }
void
bm_icon_button_set_icon(BmIconButton *self, GtkWidget *icon)
{
  g_return_if_fail(BM_IS_ICON_BUTTON(self));
  if (self->icon == icon) return;
  if (self->icon != NULL) gtk_box_remove(self->layout, self->icon);
  self->icon = icon;
  if (icon != NULL) gtk_box_append(self->layout, icon);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ICON]);
}

BmButtonVariant bm_icon_button_get_variant(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), BM_BUTTON_VARIANT_PRIMARY); return self->variant; }
void bm_icon_button_set_variant(BmIconButton *self, BmButtonVariant variant) { g_return_if_fail(BM_IS_ICON_BUTTON(self)); if (self->variant == variant) return; self->variant = variant; apply_appearance(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VARIANT]); }
BmButtonTreatment bm_icon_button_get_treatment(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), BM_BUTTON_TREATMENT_DEFAULT); return self->treatment; }
void bm_icon_button_set_treatment(BmIconButton *self, BmButtonTreatment treatment) { g_return_if_fail(BM_IS_ICON_BUTTON(self)); if (self->treatment == treatment) return; self->treatment = treatment; apply_appearance(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TREATMENT]); }
gboolean bm_icon_button_get_loading(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), FALSE); return self->loading; }
void
bm_icon_button_set_loading(BmIconButton *self, gboolean loading)
{
  g_return_if_fail(BM_IS_ICON_BUTTON(self)); loading = !!loading;
  if (self->loading == loading) return;
  if (loading) { self->restore_sensitive = gtk_widget_get_sensitive(GTK_WIDGET(self)); gtk_widget_set_sensitive(GTK_WIDGET(self), FALSE); gtk_spinner_start(self->spinner); }
  else { gtk_spinner_stop(self->spinner); gtk_widget_set_sensitive(GTK_WIDGET(self), self->restore_sensitive); }
  self->loading = loading; gtk_widget_set_visible(GTK_WIDGET(self->spinner), loading);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LOADING]);
}

BmThemeScope *bm_icon_button_get_theme_scope(BmIconButton *self) { g_return_val_if_fail(BM_IS_ICON_BUTTON(self), NULL); return self->theme_scope; }
void
bm_icon_button_set_theme_scope(BmIconButton *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_ICON_BUTTON(self)); if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0) g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0 : g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void bm_icon_button_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec) { BmIconButton *self = BM_ICON_BUTTON(object); switch (id) { case PROP_LABEL: g_value_set_string(value, self->label); break; case PROP_ICON: g_value_set_object(value, self->icon); break; case PROP_VARIANT: g_value_set_enum(value, self->variant); break; case PROP_TREATMENT: g_value_set_enum(value, self->treatment); break; case PROP_LOADING: g_value_set_boolean(value, self->loading); break; case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break; default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec); } }
static void bm_icon_button_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec) { BmIconButton *self = BM_ICON_BUTTON(object); switch (id) { case PROP_LABEL: bm_icon_button_set_label(self, g_value_get_string(value)); break; case PROP_ICON: bm_icon_button_set_icon(self, g_value_get_object(value)); break; case PROP_VARIANT: bm_icon_button_set_variant(self, g_value_get_enum(value)); break; case PROP_TREATMENT: bm_icon_button_set_treatment(self, g_value_get_enum(value)); break; case PROP_LOADING: bm_icon_button_set_loading(self, g_value_get_boolean(value)); break; case PROP_THEME_SCOPE: bm_icon_button_set_theme_scope(self, g_value_get_object(value)); break; default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec); } }
static void bm_icon_button_dispose(GObject *object) { BmIconButton *self = BM_ICON_BUTTON(object); if (self->theme_scope != NULL && self->theme_notify_id != 0) g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id); g_clear_object(&self->theme_scope); G_OBJECT_CLASS(bm_icon_button_parent_class)->dispose(object); }
static void bm_icon_button_finalize(GObject *object) { BmIconButton *self = BM_ICON_BUTTON(object); g_free(self->label); g_free(self->custom_class); G_OBJECT_CLASS(bm_icon_button_parent_class)->finalize(object); }

static void
bm_icon_button_class_init(BmIconButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass); object_class->get_property = bm_icon_button_get_property; object_class->set_property = bm_icon_button_set_property; object_class->dispose = bm_icon_button_dispose; object_class->finalize = bm_icon_button_finalize;
  properties[PROP_LABEL] = g_param_spec_string("label", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ICON] = g_param_spec_object("icon", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VARIANT] = g_param_spec_enum("variant", NULL, NULL, BM_TYPE_BUTTON_VARIANT, BM_BUTTON_VARIANT_PRIMARY, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_TREATMENT] = g_param_spec_enum("treatment", NULL, NULL, BM_TYPE_BUTTON_TREATMENT, BM_BUTTON_TREATMENT_DEFAULT, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_LOADING] = g_param_spec_boolean("loading", NULL, NULL, FALSE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_icon_button_init(BmIconButton *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-button"); gtk_widget_add_css_class(GTK_WIDGET(self), "bm-icon-button");
  self->layout = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0)); self->spinner = GTK_SPINNER(gtk_spinner_new());
  gtk_widget_set_visible(GTK_WIDGET(self->spinner), FALSE); gtk_box_append(self->layout, GTK_WIDGET(self->spinner));
  gtk_button_set_child(GTK_BUTTON(self), GTK_WIDGET(self->layout)); apply_appearance(self);
}

GtkWidget *bm_icon_button_new(const char *label, GtkWidget *icon) { return g_object_new(BM_TYPE_ICON_BUTTON, "label", label, "icon", icon, NULL); }
