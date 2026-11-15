#include "bm-private.h"

struct _BmTooltip {
  GtkWidget parent_instance;
  GtkWidget *child;
  char *content;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmTooltip, bm_tooltip, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_CHILD,
  PROP_CONTENT,
  PROP_THEME_SCOPE,
  N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

static void
sync_tooltip(BmTooltip *self)
{
  if (self->child == NULL) return;
  const char *content = self->content != NULL && *self->content != '\0'
    ? self->content : NULL;
  gtk_widget_set_tooltip_text(self->child, content);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->child),
                                 GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, content,
                                 -1);
}

static void
apply_theme(BmTooltip *self)
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
  apply_theme(BM_TOOLTIP(data));
}

GtkWidget *
bm_tooltip_get_child(BmTooltip *self)
{
  g_return_val_if_fail(BM_IS_TOOLTIP(self), NULL);
  return self->child;
}

void
bm_tooltip_set_child(BmTooltip *self, GtkWidget *child)
{
  g_return_if_fail(BM_IS_TOOLTIP(self));
  g_return_if_fail(child == NULL || GTK_IS_WIDGET(child));
  if (self->child == child) return;
  if (self->child != NULL) {
    gtk_widget_set_tooltip_text(self->child, NULL);
    gtk_accessible_update_property(GTK_ACCESSIBLE(self->child),
                                   GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, NULL,
                                   -1);
    gtk_widget_unparent(self->child);
  }
  self->child = child;
  if (child != NULL) gtk_widget_set_parent(child, GTK_WIDGET(self));
  sync_tooltip(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CHILD]);
}

const char *
bm_tooltip_get_content(BmTooltip *self)
{
  g_return_val_if_fail(BM_IS_TOOLTIP(self), NULL);
  return self->content;
}

void
bm_tooltip_set_content(BmTooltip *self, const char *content)
{
  g_return_if_fail(BM_IS_TOOLTIP(self));
  if (g_strcmp0(self->content, content) == 0) return;
  g_free(self->content);
  self->content = g_strdup(content);
  sync_tooltip(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CONTENT]);
}

void
bm_tooltip_set_theme_scope(BmTooltip *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_TOOLTIP(self));
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
bm_tooltip_get_property(GObject *object, guint id, GValue *value,
                        GParamSpec *pspec)
{
  BmTooltip *self = BM_TOOLTIP(object);
  switch (id) {
    case PROP_CHILD: g_value_set_object(value, self->child); break;
    case PROP_CONTENT: g_value_set_string(value, self->content); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_tooltip_set_property(GObject *object, guint id, const GValue *value,
                        GParamSpec *pspec)
{
  BmTooltip *self = BM_TOOLTIP(object);
  switch (id) {
    case PROP_CHILD: bm_tooltip_set_child(self, g_value_get_object(value)); break;
    case PROP_CONTENT: bm_tooltip_set_content(self, g_value_get_string(value)); break;
    case PROP_THEME_SCOPE:
      bm_tooltip_set_theme_scope(self, g_value_get_object(value));
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_tooltip_dispose(GObject *object)
{
  BmTooltip *self = BM_TOOLTIP(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  if (self->child != NULL) {
    gtk_widget_unparent(self->child);
    self->child = NULL;
  }
  G_OBJECT_CLASS(bm_tooltip_parent_class)->dispose(object);
}

static void
bm_tooltip_finalize(GObject *object)
{
  BmTooltip *self = BM_TOOLTIP(object);
  g_free(self->content);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_tooltip_parent_class)->finalize(object);
}

static void
bm_tooltip_class_init(BmTooltipClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_tooltip_get_property;
  object_class->set_property = bm_tooltip_set_property;
  object_class->dispose = bm_tooltip_dispose;
  object_class->finalize = bm_tooltip_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  properties[PROP_CHILD] = g_param_spec_object(
    "child", NULL, NULL, GTK_TYPE_WIDGET,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_CONTENT] = g_param_spec_string(
    "content", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object(
    "theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_tooltip_init(BmTooltip *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-tooltip");
}

GtkWidget *
bm_tooltip_new(GtkWidget *child, const char *content)
{
  return g_object_new(BM_TYPE_TOOLTIP, "child", child, "content", content, NULL);
}
