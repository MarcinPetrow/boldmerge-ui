#include "bm-private.h"

struct _BmInput {
  GtkWidget parent_instance;
  GtkBox *root;
  GtkBox *field;
  GtkBox *leading_box;
  GtkBox *trailing_box;
  GtkOverlay *overlay;
  GtkEntry *entry;
  GtkLabel *floating_label;
  GtkSeparator *error_separator;
  GtkBox *error_box;
  GtkLabel *error_label;
  GtkWidget *leading_widget;
  GtkWidget *trailing_widget;
  char *label;
  char *placeholder_text;
  char *error;
  char *required_message;
  gboolean required;
  gboolean valid;
  gboolean generated_error;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmInput, bm_input, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_TEXT,
  PROP_LABEL,
  PROP_PLACEHOLDER_TEXT,
  PROP_REQUIRED,
  PROP_EDITABLE,
  PROP_ERROR,
  PROP_REQUIRED_MESSAGE,
  PROP_VALID,
  PROP_LEADING_WIDGET,
  PROP_TRAILING_WIDGET,
  PROP_THEME_SCOPE,
  N_PROPERTIES,
};

enum { SIGNAL_CHANGED, SIGNAL_ACTIVATE, SIGNAL_VALIDATION_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
apply_theme(BmInput *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self), &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_INPUT(user_data));
}

static void
update_floating(BmInput *self)
{
  gboolean floating = gtk_widget_has_focus(GTK_WIDGET(self->entry)) ||
                      gtk_editable_get_text(GTK_EDITABLE(self->entry))[0] != '\0';
  if (floating) {
    gtk_widget_add_css_class(GTK_WIDGET(self->floating_label), "bm-floating");
    gtk_widget_set_valign(GTK_WIDGET(self->floating_label), GTK_ALIGN_START);
    gtk_widget_set_margin_top(GTK_WIDGET(self->floating_label), 4);
    gtk_widget_set_margin_top(GTK_WIDGET(self->entry), 8);
    gtk_entry_set_placeholder_text(self->entry, self->placeholder_text);
  } else {
    gtk_widget_remove_css_class(GTK_WIDGET(self->floating_label), "bm-floating");
    gtk_widget_set_valign(GTK_WIDGET(self->floating_label), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(GTK_WIDGET(self->floating_label), 0);
    gtk_widget_set_margin_top(GTK_WIDGET(self->entry), 0);
    gtk_entry_set_placeholder_text(self->entry, NULL);
  }
}

static void
update_accessibility(BmInput *self)
{
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->entry),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label,
                                 GTK_ACCESSIBLE_PROPERTY_PLACEHOLDER, self->placeholder_text,
                                 GTK_ACCESSIBLE_PROPERTY_REQUIRED, self->required,
                                 GTK_ACCESSIBLE_PROPERTY_READ_ONLY,
                                   !gtk_editable_get_editable(GTK_EDITABLE(self->entry)),
                                 GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, self->error,
                                 -1);
  gtk_accessible_update_state(GTK_ACCESSIBLE(self->entry),
                              GTK_ACCESSIBLE_STATE_INVALID,
                              self->error != NULL && self->error[0] != '\0'
                                ? GTK_ACCESSIBLE_INVALID_TRUE
                                : GTK_ACCESSIBLE_INVALID_FALSE,
                              -1);
  gtk_accessible_update_relation(GTK_ACCESSIBLE(self->entry),
                                 GTK_ACCESSIBLE_RELATION_LABELLED_BY,
                                   GTK_ACCESSIBLE(self->floating_label), NULL,
                                 -1);
  if (self->error != NULL && self->error[0] != '\0') {
    gtk_accessible_update_relation(GTK_ACCESSIBLE(self->entry),
                                   GTK_ACCESSIBLE_RELATION_ERROR_MESSAGE,
                                     GTK_ACCESSIBLE(self->error_label), NULL,
                                   -1);
  } else {
    gtk_accessible_reset_relation(GTK_ACCESSIBLE(self->entry),
                                  GTK_ACCESSIBLE_RELATION_ERROR_MESSAGE);
  }
}

static void
update_valid(BmInput *self)
{
  gboolean next = (self->error == NULL || self->error[0] == '\0') &&
                  (!self->required || gtk_editable_get_text(GTK_EDITABLE(self->entry))[0] != '\0');
  if (self->valid != next) {
    self->valid = next;
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALID]);
    g_signal_emit(self, signals[SIGNAL_VALIDATION_CHANGED], 0, next);
  }
}

const char *
bm_input_get_text(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), NULL);
  return gtk_editable_get_text(GTK_EDITABLE(self->entry));
}

void
bm_input_set_text(BmInput *self, const char *text)
{
  g_return_if_fail(BM_IS_INPUT(self));
  gtk_editable_set_text(GTK_EDITABLE(self->entry), text != NULL ? text : "");
}

static void
on_entry_changed(GtkEditable *editable, gpointer user_data)
{
  BmInput *self = BM_INPUT(user_data);
  (void) editable;
  if (self->generated_error && bm_input_get_text(self)[0] != '\0') {
    self->generated_error = FALSE;
    bm_input_set_error(self, NULL);
  }
  update_floating(self);
  update_valid(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TEXT]);
  g_signal_emit(self, signals[SIGNAL_CHANGED], 0);
}

static void
on_entry_activate(GtkEntry *entry, gpointer user_data)
{
  (void) entry;
  g_signal_emit(BM_INPUT(user_data), signals[SIGNAL_ACTIVATE], 0);
}

static void
on_entry_focus_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
  (void) object;
  (void) pspec;
  BmInput *self = BM_INPUT(user_data);
  update_floating(self);
  if (!gtk_widget_has_focus(GTK_WIDGET(self->entry))) bm_input_validate(self);
}

const char *
bm_input_get_label(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), NULL);
  return self->label;
}

void
bm_input_set_label(BmInput *self, const char *label)
{
  g_return_if_fail(BM_IS_INPUT(self));
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  char *display = self->required && self->label != NULL
                    ? g_strdup_printf("%s *", self->label) : g_strdup(self->label);
  gtk_label_set_text(self->floating_label, display != NULL ? display : "");
  g_free(display);
  update_accessibility(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

const char *
bm_input_get_placeholder_text(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), NULL);
  return self->placeholder_text;
}

void
bm_input_set_placeholder_text(BmInput *self, const char *text)
{
  g_return_if_fail(BM_IS_INPUT(self));
  if (g_strcmp0(self->placeholder_text, text) == 0) return;
  g_free(self->placeholder_text);
  self->placeholder_text = g_strdup(text);
  update_floating(self);
  update_accessibility(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PLACEHOLDER_TEXT]);
}

gboolean
bm_input_get_required(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), FALSE);
  return self->required;
}

void
bm_input_set_required(BmInput *self, gboolean required)
{
  g_return_if_fail(BM_IS_INPUT(self));
  required = !!required;
  if (self->required == required) return;
  self->required = required;
  const char *label = self->label;
  self->label = NULL;
  bm_input_set_label(self, label);
  g_free((char *) label);
  update_valid(self);
  update_accessibility(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_REQUIRED]);
}

gboolean
bm_input_get_editable(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), FALSE);
  return gtk_editable_get_editable(GTK_EDITABLE(self->entry));
}

void
bm_input_set_editable(BmInput *self, gboolean editable)
{
  g_return_if_fail(BM_IS_INPUT(self));
  editable = !!editable;
  if (bm_input_get_editable(self) == editable) return;
  gtk_editable_set_editable(GTK_EDITABLE(self->entry), editable);
  update_accessibility(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_EDITABLE]);
}

const char *
bm_input_get_error(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), NULL);
  return self->error;
}

void
bm_input_set_error(BmInput *self, const char *error)
{
  g_return_if_fail(BM_IS_INPUT(self));
  if (g_strcmp0(self->error, error) == 0) return;
  g_free(self->error);
  self->error = error != NULL && error[0] != '\0' ? g_strdup(error) : NULL;
  gtk_label_set_text(self->error_label, self->error != NULL ? self->error : "");
  gtk_widget_set_visible(GTK_WIDGET(self->error_separator), self->error != NULL);
  gtk_widget_set_visible(GTK_WIDGET(self->error_box), self->error != NULL);
  if (self->error != NULL) {
    gtk_widget_add_css_class(GTK_WIDGET(self), "bm-invalid");
    gtk_widget_add_css_class(GTK_WIDGET(self->entry), "error");
  } else {
    gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-invalid");
    gtk_widget_remove_css_class(GTK_WIDGET(self->entry), "error");
  }
  update_valid(self);
  update_accessibility(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_ERROR]);
}

const char *
bm_input_get_required_message(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), NULL);
  return self->required_message;
}

void
bm_input_set_required_message(BmInput *self, const char *message)
{
  g_return_if_fail(BM_IS_INPUT(self));
  if (g_strcmp0(self->required_message, message) == 0) return;
  g_free(self->required_message);
  self->required_message = g_strdup(message);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_REQUIRED_MESSAGE]);
}

gboolean
bm_input_get_valid(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), FALSE);
  return self->valid;
}

gboolean
bm_input_validate(BmInput *self)
{
  g_return_val_if_fail(BM_IS_INPUT(self), FALSE);
  if (self->required && bm_input_get_text(self)[0] == '\0' && self->error == NULL) {
    self->generated_error = TRUE;
    bm_input_set_error(self, self->required_message);
  }
  update_valid(self);
  return self->valid;
}

GtkEntry *bm_input_get_entry(BmInput *self) { g_return_val_if_fail(BM_IS_INPUT(self), NULL); return self->entry; }

static void
set_slot(GtkBox *box, GtkWidget **storage, GtkWidget *widget)
{
  if (*storage == widget) return;
  if (*storage != NULL) gtk_box_remove(box, *storage);
  *storage = widget;
  if (widget != NULL) gtk_box_append(box, widget);
  gtk_widget_set_visible(GTK_WIDGET(box), widget != NULL);
}

GtkWidget *bm_input_get_leading_widget(BmInput *self) { g_return_val_if_fail(BM_IS_INPUT(self), NULL); return self->leading_widget; }
void bm_input_set_leading_widget(BmInput *self, GtkWidget *widget) { g_return_if_fail(BM_IS_INPUT(self)); set_slot(self->leading_box, &self->leading_widget, widget); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LEADING_WIDGET]); }
GtkWidget *bm_input_get_trailing_widget(BmInput *self) { g_return_val_if_fail(BM_IS_INPUT(self), NULL); return self->trailing_widget; }
void bm_input_set_trailing_widget(BmInput *self, GtkWidget *widget) { g_return_if_fail(BM_IS_INPUT(self)); set_slot(self->trailing_box, &self->trailing_widget, widget); g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TRAILING_WIDGET]); }

BmThemeScope *bm_input_get_theme_scope(BmInput *self) { g_return_val_if_fail(BM_IS_INPUT(self), NULL); return self->theme_scope; }

void
bm_input_set_theme_scope(BmInput *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_INPUT(self));
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

static gboolean bm_input_grab_focus(GtkWidget *widget) { return gtk_widget_grab_focus(GTK_WIDGET(BM_INPUT(widget)->entry)); }

static void
bm_input_get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmInput *self = BM_INPUT(object);
  switch (id) {
  case PROP_TEXT: g_value_set_string(value, bm_input_get_text(self)); break;
  case PROP_LABEL: g_value_set_string(value, self->label); break;
  case PROP_PLACEHOLDER_TEXT: g_value_set_string(value, self->placeholder_text); break;
  case PROP_REQUIRED: g_value_set_boolean(value, self->required); break;
  case PROP_EDITABLE: g_value_set_boolean(value, bm_input_get_editable(self)); break;
  case PROP_ERROR: g_value_set_string(value, self->error); break;
  case PROP_REQUIRED_MESSAGE: g_value_set_string(value, self->required_message); break;
  case PROP_VALID: g_value_set_boolean(value, self->valid); break;
  case PROP_LEADING_WIDGET: g_value_set_object(value, self->leading_widget); break;
  case PROP_TRAILING_WIDGET: g_value_set_object(value, self->trailing_widget); break;
  case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_input_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmInput *self = BM_INPUT(object);
  switch (id) {
  case PROP_TEXT: bm_input_set_text(self, g_value_get_string(value)); break;
  case PROP_LABEL: bm_input_set_label(self, g_value_get_string(value)); break;
  case PROP_PLACEHOLDER_TEXT: bm_input_set_placeholder_text(self, g_value_get_string(value)); break;
  case PROP_REQUIRED: bm_input_set_required(self, g_value_get_boolean(value)); break;
  case PROP_EDITABLE: bm_input_set_editable(self, g_value_get_boolean(value)); break;
  case PROP_ERROR: self->generated_error = FALSE; bm_input_set_error(self, g_value_get_string(value)); break;
  case PROP_REQUIRED_MESSAGE: bm_input_set_required_message(self, g_value_get_string(value)); break;
  case PROP_LEADING_WIDGET: bm_input_set_leading_widget(self, g_value_get_object(value)); break;
  case PROP_TRAILING_WIDGET: bm_input_set_trailing_widget(self, g_value_get_object(value)); break;
  case PROP_THEME_SCOPE: bm_input_set_theme_scope(self, g_value_get_object(value)); break;
  default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_input_dispose(GObject *object)
{
  BmInput *self = BM_INPUT(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0) g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  self->theme_notify_id = 0;
  g_clear_object(&self->theme_scope);
  if (self->root != NULL) { gtk_widget_unparent(GTK_WIDGET(self->root)); self->root = NULL; }
  G_OBJECT_CLASS(bm_input_parent_class)->dispose(object);
}

static void
bm_input_finalize(GObject *object)
{
  BmInput *self = BM_INPUT(object);
  g_free(self->label); g_free(self->placeholder_text); g_free(self->error);
  g_free(self->required_message); g_free(self->custom_class);
  G_OBJECT_CLASS(bm_input_parent_class)->finalize(object);
}

static void
bm_input_class_init(BmInputClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_input_get_property; object_class->set_property = bm_input_set_property;
  object_class->dispose = bm_input_dispose; object_class->finalize = bm_input_finalize;
  widget_class->grab_focus = bm_input_grab_focus;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_GENERIC);
  properties[PROP_TEXT] = g_param_spec_string("text", NULL, NULL, "", G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_LABEL] = g_param_spec_string("label", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_PLACEHOLDER_TEXT] = g_param_spec_string("placeholder-text", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_REQUIRED] = g_param_spec_boolean("required", NULL, NULL, FALSE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_EDITABLE] = g_param_spec_boolean("editable", NULL, NULL, TRUE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_ERROR] = g_param_spec_string("error", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_REQUIRED_MESSAGE] = g_param_spec_string("required-message", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_VALID] = g_param_spec_boolean("valid", NULL, NULL, TRUE, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_LEADING_WIDGET] = g_param_spec_object("leading-widget", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_TRAILING_WIDGET] = g_param_spec_object("trailing-widget", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_CHANGED] = g_signal_new("changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
  signals[SIGNAL_ACTIVATE] = g_signal_new("activate", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
  signals[SIGNAL_VALIDATION_CHANGED] = g_signal_new("validation-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

static void
bm_input_init(BmInput *self)
{
  self->valid = TRUE;
  self->required_message = g_strdup(g_dgettext("boldmerge-1", "This field is required."));
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-input");
  self->root = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)); gtk_widget_set_parent(GTK_WIDGET(self->root), GTK_WIDGET(self));
  self->field = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8)); gtk_widget_add_css_class(GTK_WIDGET(self->field), "bm-input-field");
  self->leading_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0)); gtk_widget_set_visible(GTK_WIDGET(self->leading_box), FALSE); gtk_box_append(self->field, GTK_WIDGET(self->leading_box));
  self->overlay = GTK_OVERLAY(gtk_overlay_new()); gtk_widget_set_hexpand(GTK_WIDGET(self->overlay), TRUE);
  self->entry = GTK_ENTRY(gtk_entry_new()); gtk_entry_set_has_frame(self->entry, FALSE); gtk_widget_add_css_class(GTK_WIDGET(self->entry), "flat"); gtk_overlay_set_child(self->overlay, GTK_WIDGET(self->entry));
  self->floating_label = GTK_LABEL(gtk_label_new("")); gtk_label_set_xalign(self->floating_label, 0.0f); gtk_widget_set_halign(GTK_WIDGET(self->floating_label), GTK_ALIGN_FILL); gtk_widget_set_can_target(GTK_WIDGET(self->floating_label), FALSE); gtk_overlay_add_overlay(self->overlay, GTK_WIDGET(self->floating_label));
  gtk_box_append(self->field, GTK_WIDGET(self->overlay));
  self->trailing_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0)); gtk_widget_set_visible(GTK_WIDGET(self->trailing_box), FALSE); gtk_box_append(self->field, GTK_WIDGET(self->trailing_box));
  gtk_box_append(self->root, GTK_WIDGET(self->field));
  self->error_separator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL)); gtk_widget_set_visible(GTK_WIDGET(self->error_separator), FALSE); gtk_box_append(self->root, GTK_WIDGET(self->error_separator));
  self->error_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8)); gtk_widget_add_css_class(GTK_WIDGET(self->error_box), "bm-input-error"); gtk_widget_set_visible(GTK_WIDGET(self->error_box), FALSE);
  GtkWidget *icon = gtk_label_new("!"); gtk_widget_add_css_class(icon, "bm-input-error-icon"); gtk_box_append(self->error_box, icon);
  self->error_label = GTK_LABEL(gtk_label_new("")); gtk_label_set_wrap(self->error_label, TRUE); gtk_label_set_xalign(self->error_label, 0.0f); gtk_box_append(self->error_box, GTK_WIDGET(self->error_label)); gtk_box_append(self->root, GTK_WIDGET(self->error_box));
  g_signal_connect(self->entry, "changed", G_CALLBACK(on_entry_changed), self);
  g_signal_connect(self->entry, "activate", G_CALLBACK(on_entry_activate), self);
  g_signal_connect(self->entry, "notify::has-focus", G_CALLBACK(on_entry_focus_notify), self);
  update_floating(self); update_accessibility(self);
}

GtkWidget *bm_input_new(const char *label) { return g_object_new(BM_TYPE_INPUT, "label", label, NULL); }
