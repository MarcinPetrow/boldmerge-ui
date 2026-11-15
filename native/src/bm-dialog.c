#include "bm-private.h"

GType
bm_close_reason_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_CLOSE_REASON_API, "BM_CLOSE_REASON_API", "api" },
      { BM_CLOSE_REASON_ACTION, "BM_CLOSE_REASON_ACTION", "action" },
      { BM_CLOSE_REASON_ESCAPE, "BM_CLOSE_REASON_ESCAPE", "escape" },
      { BM_CLOSE_REASON_BACKDROP, "BM_CLOSE_REASON_BACKDROP", "backdrop" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmCloseReason", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

struct _BmDialog {
  GtkWindow parent_instance;
  GtkBox *frame;
  GtkBox *header_box;
  GtkBox *content_box;
  GtkBox *footer_row;
  GtkBox *footer_box;
  GtkBox *actions_box;
  GtkWidget *header;
  GtkWidget *child;
  GtkWidget *footer;
  GtkWidget *actions;
  GtkWidget *trigger;
  char *label;
  gboolean dismissible;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmDialog, bm_dialog, GTK_TYPE_WINDOW)

enum { PROP_0, PROP_LABEL, PROP_DISMISSIBLE, PROP_OPEN, PROP_THEME_SCOPE, N_PROPERTIES };
enum { SIGNAL_REQUEST_CLOSE, SIGNAL_CLOSED, SIGNAL_OPEN_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
dialog_slot(GtkBox *box, GtkWidget **slot, GtkWidget *widget)
{
  if (*slot == widget) return;
  if (*slot != NULL) gtk_box_remove(box, *slot);
  *slot = widget;
  if (widget != NULL) gtk_box_append(box, widget);
  gtk_widget_set_visible(GTK_WIDGET(box), widget != NULL);
}

static void
apply_theme(BmDialog *self)
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
  apply_theme(BM_DIALOG(data));
}

void bm_dialog_set_header(BmDialog *self, GtkWidget *header) { g_return_if_fail(BM_IS_DIALOG(self)); dialog_slot(self->header_box, &self->header, header); }
void bm_dialog_set_child(BmDialog *self, GtkWidget *child) { g_return_if_fail(BM_IS_DIALOG(self)); dialog_slot(self->content_box, &self->child, child); }
void bm_dialog_set_footer(BmDialog *self, GtkWidget *footer) { g_return_if_fail(BM_IS_DIALOG(self)); dialog_slot(self->footer_box, &self->footer, footer); gtk_widget_set_visible(GTK_WIDGET(self->footer_row), self->footer != NULL || self->actions != NULL); }
void bm_dialog_set_actions(BmDialog *self, GtkWidget *actions) { g_return_if_fail(BM_IS_DIALOG(self)); dialog_slot(self->actions_box, &self->actions, actions); gtk_widget_set_visible(GTK_WIDGET(self->footer_row), self->footer != NULL || self->actions != NULL); }

static void
on_action_clicked(GtkButton *button, gpointer data)
{
  BmDialog *self = BM_DIALOG(data);
  const char *value = g_object_get_data(G_OBJECT(button), "bm-return-value");
  bm_dialog_close(self, BM_CLOSE_REASON_ACTION, value != NULL ? value : "");
}

GtkButton *
bm_dialog_add_action(BmDialog *self, const char *label,
                     const char *return_value, gboolean suggested,
                     gboolean destructive)
{
  g_return_val_if_fail(BM_IS_DIALOG(self), NULL);
  GtkWidget *row = self->actions;
  if (row == NULL || !GTK_IS_BOX(row)) {
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(row, GTK_ALIGN_END);
    bm_dialog_set_actions(self, row);
  }
  GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
  if (suggested) gtk_widget_add_css_class(GTK_WIDGET(button), "suggested-action");
  if (destructive) gtk_widget_add_css_class(GTK_WIDGET(button), "destructive-action");
  g_object_set_data_full(G_OBJECT(button), "bm-return-value",
                         g_strdup(return_value), g_free);
  g_signal_connect(button, "clicked", G_CALLBACK(on_action_clicked), self);
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(button));
  return button;
}

gboolean
bm_dialog_get_dismissible(BmDialog *self)
{
  g_return_val_if_fail(BM_IS_DIALOG(self), FALSE);
  return self->dismissible;
}

void
bm_dialog_set_dismissible(BmDialog *self, gboolean dismissible)
{
  g_return_if_fail(BM_IS_DIALOG(self));
  dismissible = !!dismissible;
  if (self->dismissible == dismissible) return;
  self->dismissible = dismissible;
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_DISMISSIBLE]);
}

gboolean
bm_dialog_get_open(BmDialog *self)
{
  g_return_val_if_fail(BM_IS_DIALOG(self), FALSE);
  return gtk_widget_get_visible(GTK_WIDGET(self));
}

void
bm_dialog_present(BmDialog *self, GtkWindow *parent, GtkWidget *trigger,
                  GtkWidget *initial_focus)
{
  g_return_if_fail(BM_IS_DIALOG(self));
  g_return_if_fail(parent == NULL || GTK_IS_WINDOW(parent));
  if (bm_dialog_get_open(self)) return;
  if (self->trigger != NULL)
    g_object_remove_weak_pointer(G_OBJECT(self->trigger),
                                 (gpointer *) &self->trigger);
  self->trigger = trigger;
  if (trigger != NULL)
    g_object_add_weak_pointer(G_OBJECT(trigger), (gpointer *) &self->trigger);
  gtk_window_set_transient_for(GTK_WINDOW(self), parent);
  gtk_window_present(GTK_WINDOW(self));
  if (initial_focus != NULL)
    gtk_widget_grab_focus(initial_focus);
  else
    gtk_widget_child_focus(GTK_WIDGET(self->frame), GTK_DIR_TAB_FORWARD);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_OPEN]);
  g_signal_emit(self, signals[SIGNAL_OPEN_CHANGED], 0, TRUE);
}

gboolean
bm_dialog_close(BmDialog *self, BmCloseReason reason, const char *return_value)
{
  g_return_val_if_fail(BM_IS_DIALOG(self), FALSE);
  if (!bm_dialog_get_open(self)) return FALSE;
  gboolean prevented = FALSE;
  g_signal_emit(self, signals[SIGNAL_REQUEST_CLOSE], 0, reason,
                return_value != NULL ? return_value : "", &prevented);
  if (prevented) return FALSE;
  gtk_widget_set_visible(GTK_WIDGET(self), FALSE);
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

void
bm_dialog_set_theme_scope(BmDialog *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_DIALOG(self));
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

static gboolean
on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode,
               GdkModifierType state, gpointer data)
{
  (void) controller;
  (void) keycode;
  (void) state;
  BmDialog *self = BM_DIALOG(data);
  if (keyval == GDK_KEY_Escape && self->dismissible) {
    bm_dialog_close(self, BM_CLOSE_REASON_ESCAPE, "");
    return TRUE;
  }
  return FALSE;
}

static gboolean
on_window_close_request(GtkWindow *window, gpointer data)
{
  (void) window;
  BmDialog *self = BM_DIALOG(data);
  if (self->dismissible) bm_dialog_close(self, BM_CLOSE_REASON_API, "");
  return TRUE;
}

static void
dialog_set_label(BmDialog *self, const char *label)
{
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  gtk_window_set_title(GTK_WINDOW(self), self->label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

static void
bm_dialog_get_property(GObject *object, guint id, GValue *value,
                       GParamSpec *pspec)
{
  BmDialog *self = BM_DIALOG(object);
  switch (id) {
    case PROP_LABEL: g_value_set_string(value, self->label); break;
    case PROP_DISMISSIBLE: g_value_set_boolean(value, self->dismissible); break;
    case PROP_OPEN: g_value_set_boolean(value, bm_dialog_get_open(self)); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dialog_set_property(GObject *object, guint id, const GValue *value,
                       GParamSpec *pspec)
{
  BmDialog *self = BM_DIALOG(object);
  switch (id) {
    case PROP_LABEL: dialog_set_label(self, g_value_get_string(value)); break;
    case PROP_DISMISSIBLE:
      bm_dialog_set_dismissible(self, g_value_get_boolean(value));
      break;
    case PROP_THEME_SCOPE:
      bm_dialog_set_theme_scope(self, g_value_get_object(value));
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dialog_dispose(GObject *object)
{
  BmDialog *self = BM_DIALOG(object);
  if (self->trigger != NULL)
    g_object_remove_weak_pointer(G_OBJECT(self->trigger),
                                 (gpointer *) &self->trigger);
  self->trigger = NULL;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  G_OBJECT_CLASS(bm_dialog_parent_class)->dispose(object);
}

static void
bm_dialog_finalize(GObject *object)
{
  BmDialog *self = BM_DIALOG(object);
  g_free(self->label);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_dialog_parent_class)->finalize(object);
}

static void
bm_dialog_class_init(BmDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_dialog_get_property;
  object_class->set_property = bm_dialog_set_property;
  object_class->dispose = bm_dialog_dispose;
  object_class->finalize = bm_dialog_finalize;
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_DIALOG);
  properties[PROP_LABEL] = g_param_spec_string(
    "label", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_DISMISSIBLE] = g_param_spec_boolean(
    "dismissible", NULL, NULL, TRUE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_OPEN] = g_param_spec_boolean(
    "open", NULL, NULL, FALSE,
    G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
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
bm_dialog_init(BmDialog *self)
{
  self->dismissible = TRUE;
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-dialog");
  gtk_window_set_modal(GTK_WINDOW(self), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(self), FALSE);
  gtk_window_set_hide_on_close(GTK_WINDOW(self), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(self), 560, -1);
  self->frame = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
  gtk_widget_add_css_class(GTK_WIDGET(self->frame), "bm-dialog-frame");
  self->header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->content_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->footer_row = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
  self->footer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  self->actions_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_set_hexpand(GTK_WIDGET(self->footer_box), TRUE);
  gtk_box_append(self->footer_row, GTK_WIDGET(self->footer_box));
  gtk_box_append(self->footer_row, GTK_WIDGET(self->actions_box));
  gtk_box_append(self->frame, GTK_WIDGET(self->header_box));
  gtk_box_append(self->frame, GTK_WIDGET(self->content_box));
  gtk_box_append(self->frame, GTK_WIDGET(self->footer_row));
  gtk_window_set_child(GTK_WINDOW(self), GTK_WIDGET(self->frame));
  gtk_widget_set_visible(GTK_WIDGET(self->header_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->footer_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->actions_box), FALSE);
  gtk_widget_set_visible(GTK_WIDGET(self->footer_row), FALSE);
  GtkEventController *keys = gtk_event_controller_key_new();
  g_signal_connect(keys, "key-pressed", G_CALLBACK(on_key_pressed), self);
  gtk_widget_add_controller(GTK_WIDGET(self), keys);
  g_signal_connect(self, "close-request",
                   G_CALLBACK(on_window_close_request), self);
}

GtkWidget *
bm_dialog_new(const char *label)
{
  return g_object_new(BM_TYPE_DIALOG, "label", label, NULL);
}
