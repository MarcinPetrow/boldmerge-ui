#include "bm-private.h"

#include <math.h>

struct _BmThemeScope {
  GObject parent_instance;
  BmThemeScope *parent;
  gulong parent_notify_id;
  BmTheme theme;
  BmDensity density;
  BmAccent accent;
  GdkRGBA custom_seed;
  gboolean has_custom_seed;
  BmAccentPalette custom_palette;
  guint scope_id;
  GWeakRef widget;
  GtkCssProvider *custom_provider;
  GdkDisplay *custom_display;
  char *applied_custom_class;
};

G_DEFINE_FINAL_TYPE(BmThemeScope, bm_theme_scope, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_PARENT,
  PROP_THEME,
  PROP_DENSITY,
  PROP_ACCENT,
  PROP_CUSTOM_ACCENT,
  PROP_EFFECTIVE_THEME,
  PROP_EFFECTIVE_DENSITY,
  PROP_EFFECTIVE_ACCENT,
  N_PROPERTIES,
};

static GParamSpec *properties[N_PROPERTIES];
static gint next_scope_id = 0;

static GType
register_enum_once(const char *name, const GEnumValue *values, gsize *storage)
{
  if (g_once_init_enter(storage)) {
    GType type = g_enum_register_static(name, values);
    g_once_init_leave(storage, type);
  }
  return (GType) *storage;
}

GType
bm_theme_get_type(void)
{
  static gsize type = 0;
  static const GEnumValue values[] = {
    { BM_THEME_INHERIT, "BM_THEME_INHERIT", "inherit" },
    { BM_THEME_LIGHT, "BM_THEME_LIGHT", "light" },
    { BM_THEME_DARK, "BM_THEME_DARK", "dark" },
    { 0, NULL, NULL },
  };
  return register_enum_once("BmTheme", values, &type);
}

GType
bm_density_get_type(void)
{
  static gsize type = 0;
  static const GEnumValue values[] = {
    { BM_DENSITY_INHERIT, "BM_DENSITY_INHERIT", "inherit" },
    { BM_DENSITY_BALANCED, "BM_DENSITY_BALANCED", "balanced" },
    { BM_DENSITY_COMPACT, "BM_DENSITY_COMPACT", "compact" },
    { 0, NULL, NULL },
  };
  return register_enum_once("BmDensity", values, &type);
}

GType
bm_accent_get_type(void)
{
  static gsize type = 0;
  static const GEnumValue values[] = {
    { BM_ACCENT_INHERIT, "BM_ACCENT_INHERIT", "inherit" },
    { BM_ACCENT_CYAN, "BM_ACCENT_CYAN", "cyan" },
    { BM_ACCENT_BLUE, "BM_ACCENT_BLUE", "blue" },
    { BM_ACCENT_VIOLET, "BM_ACCENT_VIOLET", "violet" },
    { BM_ACCENT_ORANGE, "BM_ACCENT_ORANGE", "orange" },
    { BM_ACCENT_GREEN, "BM_ACCENT_GREEN", "green" },
    { BM_ACCENT_CUSTOM, "BM_ACCENT_CUSTOM", "custom" },
    { 0, NULL, NULL },
  };
  return register_enum_once("BmAccent", values, &type);
}

static const char *
theme_class(BmTheme theme)
{
  return theme == BM_THEME_DARK ? "bm-theme-dark" : "bm-theme-light";
}

static const char *
density_class(BmDensity density)
{
  return density == BM_DENSITY_COMPACT ? "bm-density-compact" : "bm-density-balanced";
}

static const char *
accent_class(BmAccent accent)
{
  static const char *classes[] = {
    NULL, "bm-accent-cyan", "bm-accent-blue", "bm-accent-violet",
    "bm-accent-orange", "bm-accent-green", NULL,
  };
  return accent >= BM_ACCENT_INHERIT && accent <= BM_ACCENT_CUSTOM ? classes[accent] : NULL;
}

BmTheme
bm_theme_scope_get_effective_theme(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), BM_THEME_LIGHT);
  if (self->theme != BM_THEME_INHERIT) return self->theme;
  if (self->parent != NULL) return bm_theme_scope_get_effective_theme(self->parent);
  return adw_style_manager_get_dark(adw_style_manager_get_default()) ? BM_THEME_DARK : BM_THEME_LIGHT;
}

BmDensity
bm_theme_scope_get_effective_density(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), BM_DENSITY_BALANCED);
  if (self->density != BM_DENSITY_INHERIT) return self->density;
  return self->parent != NULL ? bm_theme_scope_get_effective_density(self->parent) : BM_DENSITY_BALANCED;
}

BmAccent
bm_theme_scope_get_effective_accent(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), BM_ACCENT_CYAN);
  if (self->accent != BM_ACCENT_INHERIT) return self->accent;
  return self->parent != NULL ? bm_theme_scope_get_effective_accent(self->parent) : BM_ACCENT_CYAN;
}

static BmThemeScope *
custom_owner(BmThemeScope *self)
{
  if (self->accent == BM_ACCENT_CUSTOM && self->has_custom_seed) return self;
  if (self->accent == BM_ACCENT_INHERIT && self->parent != NULL) return custom_owner(self->parent);
  return NULL;
}

static void update_widget(BmThemeScope *self);

void
bm_theme_scope_clear_context_from_widget(GtkWidget *widget,
                                         char **applied_custom_class)
{
  static const char *classes[] = {
    "bm-scope", "bm-theme-light", "bm-theme-dark",
    "bm-density-balanced", "bm-density-compact", "bm-accent-cyan",
    "bm-accent-blue", "bm-accent-violet", "bm-accent-orange", "bm-accent-green",
  };
  for (guint i = 0; i < G_N_ELEMENTS(classes); i++)
    gtk_widget_remove_css_class(widget, classes[i]);
  if (applied_custom_class != NULL && *applied_custom_class != NULL) {
    gtk_widget_remove_css_class(widget, *applied_custom_class);
    g_clear_pointer(applied_custom_class, g_free);
  }
}

static void
on_parent_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  BmThemeScope *self = BM_THEME_SCOPE(user_data);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EFFECTIVE_THEME]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EFFECTIVE_DENSITY]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EFFECTIVE_ACCENT]);
  update_widget(self);
}

static gboolean
would_create_cycle(BmThemeScope *self, BmThemeScope *parent)
{
  for (BmThemeScope *cursor = parent; cursor != NULL; cursor = cursor->parent)
    if (cursor == self) return TRUE;
  return FALSE;
}

void
bm_theme_scope_set_parent(BmThemeScope *self, BmThemeScope *parent)
{
  g_return_if_fail(BM_IS_THEME_SCOPE(self));
  g_return_if_fail(parent == NULL || BM_IS_THEME_SCOPE(parent));
  g_return_if_fail(!would_create_cycle(self, parent));
  if (self->parent == parent) return;
  if (self->parent != NULL && self->parent_notify_id != 0)
    g_signal_handler_disconnect(self->parent, self->parent_notify_id);
  g_set_object(&self->parent, parent);
  self->parent_notify_id = self->parent == NULL ? 0 :
    g_signal_connect(self->parent, "notify", G_CALLBACK(on_parent_notify), self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PARENT]);
  on_parent_notify(NULL, NULL, self);
}

BmThemeScope *
bm_theme_scope_get_parent(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), NULL);
  return self->parent;
}

#define DEFINE_ENUM_ACCESSORS(Name, name, field, Type, INHERIT_VALUE, property_id) \
Type bm_theme_scope_get_##name(BmThemeScope *self) { \
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), INHERIT_VALUE); return self->field; \
} \
void bm_theme_scope_set_##name(BmThemeScope *self, Type value) { \
  g_return_if_fail(BM_IS_THEME_SCOPE(self)); \
  if (self->field == value) return; \
  self->field = value; \
  g_object_notify_by_pspec(G_OBJECT(self), properties[property_id]); \
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EFFECTIVE_##Name]); \
  update_widget(self); \
}

DEFINE_ENUM_ACCESSORS(THEME, theme, theme, BmTheme, BM_THEME_INHERIT, PROP_THEME)
DEFINE_ENUM_ACCESSORS(DENSITY, density, density, BmDensity, BM_DENSITY_INHERIT, PROP_DENSITY)
BmAccent
bm_theme_scope_get_accent(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), BM_ACCENT_INHERIT);
  return self->accent;
}

void
bm_theme_scope_set_accent(BmThemeScope *self, BmAccent value)
{
  g_return_if_fail(BM_IS_THEME_SCOPE(self));
  g_return_if_fail(value != BM_ACCENT_CUSTOM || self->has_custom_seed);
  if (self->accent == value) return;
  self->accent = value;
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ACCENT]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EFFECTIVE_ACCENT]);
  update_widget(self);
}

static void
remove_custom_provider(BmThemeScope *self)
{
  if (self->custom_provider != NULL && self->custom_display != NULL)
    gtk_style_context_remove_provider_for_display(self->custom_display,
                                                  GTK_STYLE_PROVIDER(self->custom_provider));
  g_clear_object(&self->custom_provider);
  g_clear_object(&self->custom_display);
}

static void
ensure_custom_provider(BmThemeScope *owner, GdkDisplay *display)
{
  if (owner->custom_provider != NULL && owner->custom_display == display) return;
  remove_custom_provider(owner);
  char *class_name = g_strdup_printf("bm-custom-%u", owner->scope_id);
  char *css = g_strdup_printf(
    ".%s.bm-theme-light { color: %s; }\n"
    ".%s.bm-theme-dark { color: %s; }\n"
    ".%s.bm-theme-light.bm-accent-sample { background-color: %s; color: %s; }\n"
    ".%s.bm-theme-dark.bm-accent-sample { background-color: %s; color: %s; }\n"
    ".%s.bm-theme-light.bm-button.bm-primary { background-color: %s; border-color: %s; color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-dark.bm-button.bm-primary { background-color: %s; border-color: %s; color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-light.bm-button.bm-outline, .%s.bm-theme-light.bm-button.bm-ghost { color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-dark.bm-button.bm-outline, .%s.bm-theme-dark.bm-button.bm-ghost { color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-light.bm-input:focus-within { border-color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-dark.bm-input:focus-within { border-color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-light.bm-checkbox check:checked, .%s.bm-theme-light.bm-radio radio:checked, .%s.bm-theme-light.bm-switch switch:checked { background-color: %s; border-color: %s; color: %s; }\n"
    ".%s.bm-theme-dark.bm-checkbox check:checked, .%s.bm-theme-dark.bm-radio radio:checked, .%s.bm-theme-dark.bm-switch switch:checked { background-color: %s; border-color: %s; color: %s; }\n"
    ".%s.bm-theme-light.bm-tag { background-color: %s; color: %s; }\n"
    ".%s.bm-theme-dark.bm-tag { background-color: %s; color: %s; }\n"
    ".%s.bm-theme-light.bm-select:focus-within { border-color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-dark.bm-select:focus-within { border-color: %s; outline-color: %s; }\n"
    ".%s.bm-theme-light.bm-status.bm-accent .bm-status-dot { background-color: %s; }\n"
    ".%s.bm-theme-dark.bm-status.bm-accent .bm-status-dot { background-color: %s; }\n",
    class_name, owner->custom_palette.light_text,
    class_name, owner->custom_palette.dark_text,
    class_name, owner->custom_palette.light_solid, owner->custom_palette.light_on_solid,
    class_name, owner->custom_palette.dark_solid, owner->custom_palette.dark_on_solid,
    class_name, owner->custom_palette.light_solid, owner->custom_palette.light_solid,
      owner->custom_palette.light_on_solid, owner->custom_palette.light_focus,
    class_name, owner->custom_palette.dark_solid, owner->custom_palette.dark_solid,
      owner->custom_palette.dark_on_solid, owner->custom_palette.dark_focus,
    class_name, class_name, owner->custom_palette.light_text, owner->custom_palette.light_focus,
    class_name, class_name, owner->custom_palette.dark_text, owner->custom_palette.dark_focus,
    class_name, owner->custom_palette.light_focus, owner->custom_palette.light_focus,
    class_name, owner->custom_palette.dark_focus, owner->custom_palette.dark_focus,
    class_name, class_name, class_name,
      owner->custom_palette.light_solid, owner->custom_palette.light_solid,
      owner->custom_palette.light_on_solid,
    class_name, class_name, class_name,
      owner->custom_palette.dark_solid, owner->custom_palette.dark_solid,
      owner->custom_palette.dark_on_solid,
    class_name, owner->custom_palette.light_subtle, owner->custom_palette.light_on_subtle,
    class_name, owner->custom_palette.dark_subtle, owner->custom_palette.dark_on_subtle,
    class_name, owner->custom_palette.light_focus, owner->custom_palette.light_focus,
    class_name, owner->custom_palette.dark_focus, owner->custom_palette.dark_focus,
    class_name, owner->custom_palette.light_solid,
    class_name, owner->custom_palette.dark_solid);
  owner->custom_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(owner->custom_provider, css, -1);
  owner->custom_display = g_object_ref(display);
  gtk_style_context_add_provider_for_display(display,
                                              GTK_STYLE_PROVIDER(owner->custom_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 2);
  g_free(css);
  g_free(class_name);
}

void
bm_theme_scope_apply_context_to_widget(BmThemeScope *scope,
                                       GtkWidget *widget,
                                       char **applied_custom_class)
{
  g_return_if_fail(BM_IS_THEME_SCOPE(scope));
  g_return_if_fail(GTK_IS_WIDGET(widget));
  bm_theme_scope_clear_context_from_widget(widget, applied_custom_class);
  gtk_widget_add_css_class(widget, "bm-scope");
  gtk_widget_add_css_class(widget, theme_class(bm_theme_scope_get_effective_theme(scope)));
  gtk_widget_add_css_class(widget, density_class(bm_theme_scope_get_effective_density(scope)));
  BmAccent effective = bm_theme_scope_get_effective_accent(scope);
  if (effective == BM_ACCENT_CUSTOM) {
    BmThemeScope *owner = custom_owner(scope);
    if (owner != NULL) {
      char *class_name = g_strdup_printf("bm-custom-%u", owner->scope_id);
      gtk_widget_add_css_class(widget, class_name);
      if (applied_custom_class != NULL) *applied_custom_class = g_strdup(class_name);
      ensure_custom_provider(owner, gtk_widget_get_display(widget));
      g_free(class_name);
    }
  } else {
    const char *klass = accent_class(effective);
    if (klass != NULL) gtk_widget_add_css_class(widget, klass);
  }
}

static void
update_widget(BmThemeScope *self)
{
  g_autoptr(GtkWidget) widget = g_weak_ref_get(&self->widget);
  if (widget == NULL) return;
  GdkDisplay *display = gtk_widget_get_display(widget);
  if (self->parent == NULL) {
    AdwStyleManager *style_manager = adw_style_manager_get_for_display(display);
    AdwColorScheme scheme = ADW_COLOR_SCHEME_PREFER_LIGHT;
    if (self->theme == BM_THEME_LIGHT) scheme = ADW_COLOR_SCHEME_FORCE_LIGHT;
    if (self->theme == BM_THEME_DARK) scheme = ADW_COLOR_SCHEME_FORCE_DARK;
    adw_style_manager_set_color_scheme(style_manager, scheme);
  }
  bm_theme_scope_apply_context_to_widget(self, widget, &self->applied_custom_class);
}

gboolean
bm_theme_scope_set_custom_accent(BmThemeScope *self,
                                 const GdkRGBA *seed,
                                 GError **error)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), FALSE);
  g_return_val_if_fail(seed != NULL, FALSE);
  if (seed->red < 0.0 || seed->red > 1.0 ||
      seed->green < 0.0 || seed->green > 1.0 ||
      seed->blue < 0.0 || seed->blue > 1.0 ||
      fabs(seed->alpha - 1.0) > 1e-6) {
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                        "Custom accent must be an opaque in-range sRGB color");
    return FALSE;
  }
  BmAccentPalette palette = { 0 };
  if (!bm_accent_palette_generate(seed, &palette, error)) return FALSE;
  bm_accent_palette_clear(&self->custom_palette);
  self->custom_palette = palette;
  self->custom_seed = *seed;
  self->has_custom_seed = TRUE;
  remove_custom_provider(self);
  bm_theme_scope_set_accent(self, BM_ACCENT_CUSTOM);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CUSTOM_ACCENT]);
  update_widget(self);
  return TRUE;
}

const GdkRGBA *
bm_theme_scope_get_custom_accent(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), NULL);
  return self->has_custom_seed ? &self->custom_seed : NULL;
}

void
bm_theme_scope_apply(BmThemeScope *self, GtkWidget *widget)
{
  g_return_if_fail(BM_IS_THEME_SCOPE(self));
  g_return_if_fail(GTK_IS_WIDGET(widget));
  g_autoptr(GtkWidget) previous = g_weak_ref_get(&self->widget);
  if (previous != NULL && previous != widget)
    bm_theme_scope_clear_context_from_widget(previous, &self->applied_custom_class);
  g_weak_ref_set(&self->widget, widget);
  update_widget(self);
}

GtkWidget *
bm_theme_scope_ref_widget(BmThemeScope *self)
{
  g_return_val_if_fail(BM_IS_THEME_SCOPE(self), NULL);
  return g_weak_ref_get(&self->widget);
}

static void
bm_theme_scope_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
  BmThemeScope *self = BM_THEME_SCOPE(object);
  switch (property_id) {
  case PROP_PARENT: g_value_set_object(value, self->parent); break;
  case PROP_THEME: g_value_set_enum(value, self->theme); break;
  case PROP_DENSITY: g_value_set_enum(value, self->density); break;
  case PROP_ACCENT: g_value_set_enum(value, self->accent); break;
  case PROP_CUSTOM_ACCENT: g_value_set_boxed(value, bm_theme_scope_get_custom_accent(self)); break;
  case PROP_EFFECTIVE_THEME: g_value_set_enum(value, bm_theme_scope_get_effective_theme(self)); break;
  case PROP_EFFECTIVE_DENSITY: g_value_set_enum(value, bm_theme_scope_get_effective_density(self)); break;
  case PROP_EFFECTIVE_ACCENT: g_value_set_enum(value, bm_theme_scope_get_effective_accent(self)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void
bm_theme_scope_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
  BmThemeScope *self = BM_THEME_SCOPE(object);
  switch (property_id) {
  case PROP_PARENT: bm_theme_scope_set_parent(self, g_value_get_object(value)); break;
  case PROP_THEME: bm_theme_scope_set_theme(self, g_value_get_enum(value)); break;
  case PROP_DENSITY: bm_theme_scope_set_density(self, g_value_get_enum(value)); break;
  case PROP_ACCENT: bm_theme_scope_set_accent(self, g_value_get_enum(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void
bm_theme_scope_dispose(GObject *object)
{
  BmThemeScope *self = BM_THEME_SCOPE(object);
  if (self->parent != NULL && self->parent_notify_id != 0)
    g_signal_handler_disconnect(self->parent, self->parent_notify_id);
  self->parent_notify_id = 0;
  g_clear_object(&self->parent);
  g_autoptr(GtkWidget) widget = g_weak_ref_get(&self->widget);
  if (widget != NULL)
    bm_theme_scope_clear_context_from_widget(widget, &self->applied_custom_class);
  remove_custom_provider(self);
  G_OBJECT_CLASS(bm_theme_scope_parent_class)->dispose(object);
}

static void
bm_theme_scope_finalize(GObject *object)
{
  BmThemeScope *self = BM_THEME_SCOPE(object);
  bm_accent_palette_clear(&self->custom_palette);
  g_clear_pointer(&self->applied_custom_class, g_free);
  g_weak_ref_clear(&self->widget);
  G_OBJECT_CLASS(bm_theme_scope_parent_class)->finalize(object);
}

static void
bm_theme_scope_class_init(BmThemeScopeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_theme_scope_get_property;
  object_class->set_property = bm_theme_scope_set_property;
  object_class->dispose = bm_theme_scope_dispose;
  object_class->finalize = bm_theme_scope_finalize;
  properties[PROP_PARENT] = g_param_spec_object("parent", NULL, NULL, BM_TYPE_THEME_SCOPE,
                                                G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME] = g_param_spec_enum("theme", NULL, NULL, BM_TYPE_THEME, BM_THEME_INHERIT,
                                             G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_DENSITY] = g_param_spec_enum("density", NULL, NULL, BM_TYPE_DENSITY, BM_DENSITY_INHERIT,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ACCENT] = g_param_spec_enum("accent", NULL, NULL, BM_TYPE_ACCENT, BM_ACCENT_INHERIT,
                                              G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_CUSTOM_ACCENT] = g_param_spec_boxed("custom-accent", NULL, NULL, GDK_TYPE_RGBA,
                                                      G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_EFFECTIVE_THEME] = g_param_spec_enum("effective-theme", NULL, NULL, BM_TYPE_THEME, BM_THEME_LIGHT,
                                                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_EFFECTIVE_DENSITY] = g_param_spec_enum("effective-density", NULL, NULL, BM_TYPE_DENSITY, BM_DENSITY_BALANCED,
                                                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_EFFECTIVE_ACCENT] = g_param_spec_enum("effective-accent", NULL, NULL, BM_TYPE_ACCENT, BM_ACCENT_CYAN,
                                                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_theme_scope_init(BmThemeScope *self)
{
  self->theme = BM_THEME_INHERIT;
  self->density = BM_DENSITY_INHERIT;
  self->accent = BM_ACCENT_INHERIT;
  self->scope_id = (guint) g_atomic_int_add(&next_scope_id, 1) + 1;
  g_weak_ref_init(&self->widget, NULL);
}

BmThemeScope *
bm_theme_scope_new(BmThemeScope *parent)
{
  return g_object_new(BM_TYPE_THEME_SCOPE, "parent", parent, NULL);
}
