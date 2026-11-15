#include "bm-private.h"

GType
bm_drawer_mode_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_DRAWER_MODE_OVERLAY, "BM_DRAWER_MODE_OVERLAY", "overlay" },
      { BM_DRAWER_MODE_PUSH, "BM_DRAWER_MODE_PUSH", "push" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmDrawerMode", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

GType
bm_drawer_position_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_DRAWER_POSITION_START, "BM_DRAWER_POSITION_START", "start" },
      { BM_DRAWER_POSITION_END, "BM_DRAWER_POSITION_END", "end" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmDrawerPosition", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

struct _BmDrawer {
  GtkWidget parent_instance;
  AdwFlap *flap;
  GtkBox *panel;
  GtkBox *header_box;
  GtkBox *content_box;
  GtkBox *footer_box;
  GtkBox *actions_box;
  GtkWidget *content;
  GtkWidget *header;
  GtkWidget *child;
  GtkWidget *footer;
  GtkWidget *actions;
  GtkWidget *trigger;
  char *label;
  gboolean open;
  gboolean dismissible;
  gboolean mobile;
  BmDrawerMode mode;
  BmDrawerPosition position;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
  char *panel_custom_class;
};

G_DEFINE_FINAL_TYPE(BmDrawer, bm_drawer, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_LABEL,
  PROP_OPEN,
  PROP_DISMISSIBLE,
  PROP_MODE,
  PROP_POSITION,
  PROP_MOBILE,
  PROP_THEME_SCOPE,
  N_PROPERTIES
};
enum { SIGNAL_REQUEST_CLOSE, SIGNAL_CLOSED, SIGNAL_OPEN_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
drawer_slot(GtkBox *box, GtkWidget **slot, GtkWidget *widget)
{
  if (*slot == widget) return;
  if (*slot != NULL) gtk_box_remove(box, *slot);
  *slot = widget;
  if (widget != NULL) gtk_box_append(box, widget);
  gtk_widget_set_visible(GTK_WIDGET(box), widget != NULL);
}

static gboolean
drawer_is_overlay(BmDrawer *self)
{
  return self->mode == BM_DRAWER_MODE_OVERLAY || self->mobile;
}

static void
sync_policy(BmDrawer *self)
{
  gboolean overlay = drawer_is_overlay(self);
  adw_flap_set_fold_policy(self->flap,
                           overlay ? ADW_FLAP_FOLD_POLICY_ALWAYS
                                   : ADW_FLAP_FOLD_POLICY_NEVER);
  adw_flap_set_transition_type(self->flap,
                               overlay ? ADW_FLAP_TRANSITION_TYPE_OVER
                                       : ADW_FLAP_TRANSITION_TYPE_SLIDE);
  adw_flap_set_modal(self->flap, overlay);
  adw_flap_set_locked(self->flap, !self->dismissible);
  adw_flap_set_swipe_to_open(self->flap, FALSE);
  adw_flap_set_swipe_to_close(self->flap,
                              overlay && self->dismissible);
}

static void
apply_theme(BmDrawer *self)
{
  if (self->theme_scope == NULL) return;
  bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self),
                                         &self->custom_class);
  bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                         GTK_WIDGET(self->panel),
                                         &self->panel_custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_DRAWER(data));
}

void
bm_drawer_set_content(BmDrawer *self, GtkWidget *content)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  if (self->content == content) return;
  self->content = content;
  adw_flap_set_content(self->flap, content);
}

void bm_drawer_set_header(BmDrawer *self, GtkWidget *header) { g_return_if_fail(BM_IS_DRAWER(self)); drawer_slot(self->header_box, &self->header, header); }
void bm_drawer_set_child(BmDrawer *self, GtkWidget *child) { g_return_if_fail(BM_IS_DRAWER(self)); drawer_slot(self->content_box, &self->child, child); }
void bm_drawer_set_footer(BmDrawer *self, GtkWidget *footer) { g_return_if_fail(BM_IS_DRAWER(self)); drawer_slot(self->footer_box, &self->footer, footer); }
void bm_drawer_set_actions(BmDrawer *self, GtkWidget *actions) { g_return_if_fail(BM_IS_DRAWER(self)); drawer_slot(self->actions_box, &self->actions, actions); }

gboolean
bm_drawer_get_open(BmDrawer *self)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), FALSE);
  return self->open;
}

void
bm_drawer_open(BmDrawer *self, GtkWidget *trigger, GtkWidget *initial_focus)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  if (self->open) return;
  if (self->trigger != NULL)
    g_object_remove_weak_pointer(G_OBJECT(self->trigger),
                                 (gpointer *) &self->trigger);
  self->trigger = trigger;
  if (trigger != NULL)
    g_object_add_weak_pointer(G_OBJECT(trigger), (gpointer *) &self->trigger);
  self->open = TRUE;
  adw_flap_set_reveal_flap(self->flap, TRUE);
  if (gtk_widget_get_root(GTK_WIDGET(self)) != NULL) {
    if (initial_focus != NULL)
      gtk_widget_grab_focus(initial_focus);
    else
      gtk_widget_child_focus(GTK_WIDGET(self->panel), GTK_DIR_TAB_FORWARD);
  }
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_OPEN]);
  g_signal_emit(self, signals[SIGNAL_OPEN_CHANGED], 0, TRUE);
}

gboolean
bm_drawer_close(BmDrawer *self, BmCloseReason reason, const char *return_value)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), FALSE);
  if (!self->open) return FALSE;
  gboolean prevented = FALSE;
  g_signal_emit(self, signals[SIGNAL_REQUEST_CLOSE], 0, reason,
                return_value != NULL ? return_value : "", &prevented);
  if (prevented) {
    adw_flap_set_reveal_flap(self->flap, TRUE);
    return FALSE;
  }
  self->open = FALSE;
  adw_flap_set_reveal_flap(self->flap, FALSE);
  if (self->trigger != NULL) gtk_widget_grab_focus(self->trigger);
  if (self->trigger != NULL) {
    g_object_remove_weak_pointer(G_OBJECT(self->trigger),
                                 (gpointer *) &self->trigger);
    self->trigger = NULL;
  }
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_OPEN]);
  g_signal_emit(self, signals[SIGNAL_CLOSED], 0, reason,
                return_value != NULL ? return_value : "");
  g_signal_emit(self, signals[SIGNAL_OPEN_CHANGED], 0, FALSE);
  return TRUE;
}

BmDrawerMode
bm_drawer_get_mode(BmDrawer *self)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), BM_DRAWER_MODE_OVERLAY);
  return self->mode;
}

void
bm_drawer_set_mode(BmDrawer *self, BmDrawerMode mode)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  if (self->mode == mode) return;
  self->mode = mode;
  sync_policy(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MODE]);
}

BmDrawerPosition
bm_drawer_get_position(BmDrawer *self)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), BM_DRAWER_POSITION_END);
  return self->position;
}

void
bm_drawer_set_position(BmDrawer *self, BmDrawerPosition position)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  if (self->position == position) return;
  self->position = position;
  adw_flap_set_flap_position(self->flap,
    position == BM_DRAWER_POSITION_START ? GTK_PACK_START : GTK_PACK_END);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_POSITION]);
}

gboolean
bm_drawer_get_mobile(BmDrawer *self)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), FALSE);
  return self->mobile;
}

void
bm_drawer_set_mobile(BmDrawer *self, gboolean mobile)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  mobile = !!mobile;
  if (self->mobile == mobile) return;
  self->mobile = mobile;
  sync_policy(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MOBILE]);
}

gboolean
bm_drawer_get_dismissible(BmDrawer *self)
{
  g_return_val_if_fail(BM_IS_DRAWER(self), FALSE);
  return self->dismissible;
}

void
bm_drawer_set_dismissible(BmDrawer *self, gboolean dismissible)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  dismissible = !!dismissible;
  if (self->dismissible == dismissible) return;
  self->dismissible = dismissible;
  sync_policy(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_DISMISSIBLE]);
}

void
bm_drawer_set_theme_scope(BmDrawer *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_DRAWER(self));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self->panel),
                                           &self->panel_custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
on_reveal_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  BmDrawer *self = BM_DRAWER(data);
  gboolean revealed = adw_flap_get_reveal_flap(self->flap);
  if (revealed == self->open) return;
  if (!revealed && self->open) {
    if (self->dismissible)
      bm_drawer_close(self, BM_CLOSE_REASON_BACKDROP, "");
    else
      adw_flap_set_reveal_flap(self->flap, TRUE);
  }
}

static void
drawer_set_label(BmDrawer *self, const char *label)
{
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->panel),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

static void
bm_drawer_get_property(GObject *object, guint id, GValue *value,
                       GParamSpec *pspec)
{
  BmDrawer *self = BM_DRAWER(object);
  switch (id) {
    case PROP_LABEL: g_value_set_string(value, self->label); break;
    case PROP_OPEN: g_value_set_boolean(value, self->open); break;
    case PROP_DISMISSIBLE: g_value_set_boolean(value, self->dismissible); break;
    case PROP_MODE: g_value_set_enum(value, self->mode); break;
    case PROP_POSITION: g_value_set_enum(value, self->position); break;
    case PROP_MOBILE: g_value_set_boolean(value, self->mobile); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_drawer_set_property(GObject *object, guint id, const GValue *value,
                       GParamSpec *pspec)
{
  BmDrawer *self = BM_DRAWER(object);
  switch (id) {
    case PROP_LABEL: drawer_set_label(self, g_value_get_string(value)); break;
    case PROP_OPEN:
      if (g_value_get_boolean(value)) bm_drawer_open(self, NULL, NULL);
      else bm_drawer_close(self, BM_CLOSE_REASON_API, "");
      break;
    case PROP_DISMISSIBLE:
      bm_drawer_set_dismissible(self, g_value_get_boolean(value));
      break;
    case PROP_MODE: bm_drawer_set_mode(self, g_value_get_enum(value)); break;
    case PROP_POSITION:
      bm_drawer_set_position(self, g_value_get_enum(value));
      break;
    case PROP_MOBILE:
      bm_drawer_set_mobile(self, g_value_get_boolean(value));
      break;
    case PROP_THEME_SCOPE:
      bm_drawer_set_theme_scope(self, g_value_get_object(value));
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_drawer_dispose(GObject *object)
{
  BmDrawer *self = BM_DRAWER(object);
  if (self->trigger != NULL)
    g_object_remove_weak_pointer(G_OBJECT(self->trigger),
                                 (gpointer *) &self->trigger);
  self->trigger = NULL;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  if (self->flap != NULL) {
    gtk_widget_unparent(GTK_WIDGET(self->flap));
    self->flap = NULL;
  }
  G_OBJECT_CLASS(bm_drawer_parent_class)->dispose(object);
}

static void
bm_drawer_finalize(GObject *object)
{
  BmDrawer *self = BM_DRAWER(object);
  g_free(self->label);
  g_free(self->custom_class);
  g_free(self->panel_custom_class);
  G_OBJECT_CLASS(bm_drawer_parent_class)->finalize(object);
}

static void
bm_drawer_class_init(BmDrawerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_drawer_get_property;
  object_class->set_property = bm_drawer_set_property;
  object_class->dispose = bm_drawer_dispose;
  object_class->finalize = bm_drawer_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  properties[PROP_LABEL] = g_param_spec_string(
    "label", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_OPEN] = g_param_spec_boolean(
    "open", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_DISMISSIBLE] = g_param_spec_boolean(
    "dismissible", NULL, NULL, TRUE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_MODE] = g_param_spec_enum(
    "mode", NULL, NULL, BM_TYPE_DRAWER_MODE, BM_DRAWER_MODE_OVERLAY,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_POSITION] = g_param_spec_enum(
    "position", NULL, NULL, BM_TYPE_DRAWER_POSITION, BM_DRAWER_POSITION_END,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_MOBILE] = g_param_spec_boolean(
    "mobile", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object(
    "theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_REQUEST_CLOSE] = g_signal_new(
    "request-close", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0,
    g_signal_accumulator_true_handled, NULL, NULL, G_TYPE_BOOLEAN, 2,
    BM_TYPE_CLOSE_REASON, G_TYPE_STRING);
  signals[SIGNAL_CLOSED] = g_signal_new(
    "closed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
    G_TYPE_NONE, 2, BM_TYPE_CLOSE_REASON, G_TYPE_STRING);
  signals[SIGNAL_OPEN_CHANGED] = g_signal_new(
    "open-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
    NULL, G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

static void
bm_drawer_init(BmDrawer *self)
{
  self->dismissible = TRUE;
  self->mode = BM_DRAWER_MODE_OVERLAY;
  self->position = BM_DRAWER_POSITION_END;
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-drawer");
  self->flap = ADW_FLAP(adw_flap_new());
  gtk_widget_set_parent(GTK_WIDGET(self->flap), GTK_WIDGET(self));
  self->panel = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
  gtk_widget_add_css_class(GTK_WIDGET(self->panel), "bm-drawer-panel");
  gtk_widget_set_size_request(GTK_WIDGET(self->panel), 360, -1);
  self->header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->content_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->footer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->actions_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(self->header_box), "bm-drawer-header");
  gtk_widget_add_css_class(GTK_WIDGET(self->content_box), "bm-drawer-content");
  gtk_widget_add_css_class(GTK_WIDGET(self->footer_box), "bm-drawer-footer");
  gtk_widget_add_css_class(GTK_WIDGET(self->actions_box), "bm-drawer-actions");
  gtk_widget_set_vexpand(GTK_WIDGET(self->content_box), TRUE);
  gtk_box_append(self->panel, GTK_WIDGET(self->header_box));
  gtk_box_append(self->panel, GTK_WIDGET(self->content_box));
  gtk_box_append(self->panel, GTK_WIDGET(self->footer_box));
  gtk_box_append(self->panel, GTK_WIDGET(self->actions_box));
  adw_flap_set_flap(self->flap, GTK_WIDGET(self->panel));
  adw_flap_set_flap_position(self->flap, GTK_PACK_END);
  adw_flap_set_reveal_flap(self->flap, FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->header_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->content_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->footer_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->actions_box), FALSE);
  sync_policy(self);
  g_signal_connect(self->flap, "notify::reveal-flap",
                   G_CALLBACK(on_reveal_notify), self);
}

GtkWidget *
bm_drawer_new(const char *label)
{
  return g_object_new(BM_TYPE_DRAWER, "label", label, NULL);
}
