#include "bm-private.h"

struct _BmCard {
  GtkWidget parent_instance;
  GtkBox *root;
  GtkBox *header_row;
  GtkBox *header_box;
  GtkBox *actions_box;
  GtkBox *content_box;
  GtkBox *footer_box;
  GtkWidget *header;
  GtkWidget *actions;
  GtkWidget *child;
  GtkWidget *footer;
  char *accessible_label;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmCard, bm_card, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_CHILD,
  PROP_HEADER,
  PROP_ACTIONS,
  PROP_FOOTER,
  PROP_ACCESSIBLE_LABEL,
  PROP_THEME_SCOPE,
  N_PROPERTIES,
};

static GParamSpec *properties[N_PROPERTIES];

static void
apply_theme(BmCard *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self), &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_CARD(user_data));
}

static void
set_slot(GtkBox *box, GtkWidget **storage, GtkWidget *widget)
{
  if (*storage == widget) return;
  if (*storage != NULL) gtk_box_remove(box, *storage);
  *storage = widget;
  if (widget != NULL) gtk_box_append(box, widget);
}

static void
update_region_visibility(BmCard *self)
{
  gtk_widget_set_visible(GTK_WIDGET(self->header_row), self->header != NULL || self->actions != NULL);
  gtk_widget_set_visible(GTK_WIDGET(self->content_box), self->child != NULL);
  gtk_widget_set_visible(GTK_WIDGET(self->footer_box), self->footer != NULL);
}

GtkWidget *bm_card_get_child(BmCard *self) { g_return_val_if_fail(BM_IS_CARD(self), NULL); return self->child; }
void bm_card_set_child(BmCard *self, GtkWidget *widget) { g_return_if_fail(BM_IS_CARD(self)); set_slot(self->content_box, &self->child, widget); update_region_visibility(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CHILD]); }
GtkWidget *bm_card_get_header(BmCard *self) { g_return_val_if_fail(BM_IS_CARD(self), NULL); return self->header; }
void bm_card_set_header(BmCard *self, GtkWidget *widget) { g_return_if_fail(BM_IS_CARD(self)); set_slot(self->header_box, &self->header, widget); update_region_visibility(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_HEADER]); }
GtkWidget *bm_card_get_actions(BmCard *self) { g_return_val_if_fail(BM_IS_CARD(self), NULL); return self->actions; }
void bm_card_set_actions(BmCard *self, GtkWidget *widget) { g_return_if_fail(BM_IS_CARD(self)); set_slot(self->actions_box, &self->actions, widget); update_region_visibility(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ACTIONS]); }
GtkWidget *bm_card_get_footer(BmCard *self) { g_return_val_if_fail(BM_IS_CARD(self), NULL); return self->footer; }
void bm_card_set_footer(BmCard *self, GtkWidget *widget) { g_return_if_fail(BM_IS_CARD(self)); set_slot(self->footer_box, &self->footer, widget); update_region_visibility(self); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_FOOTER]); }

const char *
bm_card_get_accessible_label(BmCard *self)
{
  g_return_val_if_fail(BM_IS_CARD(self), NULL);
  return self->accessible_label;
}

void
bm_card_set_accessible_label(BmCard *self, const char *label)
{
  g_return_if_fail(BM_IS_CARD(self));
  if (g_strcmp0(self->accessible_label, label) == 0) return;
  g_free(self->accessible_label);
  self->accessible_label = g_strdup(label);
  if (self->accessible_label != NULL)
    gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                   GTK_ACCESSIBLE_PROPERTY_LABEL, self->accessible_label,
                                   -1);
  else
    gtk_accessible_reset_property(GTK_ACCESSIBLE(self), GTK_ACCESSIBLE_PROPERTY_LABEL);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ACCESSIBLE_LABEL]);
}

BmThemeScope *bm_card_get_theme_scope(BmCard *self) { g_return_val_if_fail(BM_IS_CARD(self), NULL); return self->theme_scope; }

void
bm_card_set_theme_scope(BmCard *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_CARD(self));
  g_return_if_fail(scope == NULL || BM_IS_THEME_SCOPE(scope));
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
bm_card_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmCard *self = BM_CARD(object);
  switch (id) {
  case PROP_CHILD: g_value_set_object(value, self->child); break;
  case PROP_HEADER: g_value_set_object(value, self->header); break;
  case PROP_ACTIONS: g_value_set_object(value, self->actions); break;
  case PROP_FOOTER: g_value_set_object(value, self->footer); break;
  case PROP_ACCESSIBLE_LABEL: g_value_set_string(value, self->accessible_label); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_card_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmCard *self = BM_CARD(object);
  switch (id) {
  case PROP_CHILD: bm_card_set_child(self, g_value_get_object(value)); break;
  case PROP_HEADER: bm_card_set_header(self, g_value_get_object(value)); break;
  case PROP_ACTIONS: bm_card_set_actions(self, g_value_get_object(value)); break;
  case PROP_FOOTER: bm_card_set_footer(self, g_value_get_object(value)); break;
  case PROP_ACCESSIBLE_LABEL: bm_card_set_accessible_label(self, g_value_get_string(value)); break;
  case PROP_THEME_SCOPE: bm_card_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_card_dispose(GObject *object)
{
  BmCard *self = BM_CARD(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0) g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  if (self->root != NULL) { gtk_widget_unparent(GTK_WIDGET(self->root)); self->root = NULL; }
  G_OBJECT_CLASS(bm_card_parent_class)->dispose(object);
}

static void
bm_card_finalize(GObject *object)
{
  BmCard *self = BM_CARD(object);
  g_free(self->accessible_label); g_free(self->custom_class);
  G_OBJECT_CLASS(bm_card_parent_class)->finalize(object);
}

static void
bm_card_class_init(BmCardClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_card_get_property; object_class->set_property = bm_card_set_property;
  object_class->dispose = bm_card_dispose; object_class->finalize = bm_card_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_GROUP);
  properties[PROP_CHILD] = g_param_spec_object("child", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_HEADER] = g_param_spec_object("header", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ACTIONS] = g_param_spec_object("actions", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_FOOTER] = g_param_spec_object("footer", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ACCESSIBLE_LABEL] = g_param_spec_string("accessible-label", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
bm_card_init(BmCard *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-card");
  self->root = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)); gtk_widget_set_parent(GTK_WIDGET(self->root), GTK_WIDGET(self));
  self->header_row = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12)); gtk_widget_add_css_class(GTK_WIDGET(self->header_row), "bm-card-header");
  self->header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0)); gtk_widget_set_hexpand(GTK_WIDGET(self->header_box), TRUE); gtk_box_append(self->header_row, GTK_WIDGET(self->header_box));
  self->actions_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8)); gtk_box_append(self->header_row, GTK_WIDGET(self->actions_box)); gtk_box_append(self->root, GTK_WIDGET(self->header_row));
  self->content_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)); gtk_widget_add_css_class(GTK_WIDGET(self->content_box), "bm-card-content"); gtk_box_append(self->root, GTK_WIDGET(self->content_box));
  self->footer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0)); gtk_widget_add_css_class(GTK_WIDGET(self->footer_box), "bm-card-footer"); gtk_box_append(self->root, GTK_WIDGET(self->footer_box));
  update_region_visibility(self);
}

GtkWidget *bm_card_new(void) { return g_object_new(BM_TYPE_CARD, NULL); }
