#include "bm-private.h"

struct _BmDropdownItem {
  GtkButton parent_instance;
  GtkBox *layout;
  GtkLabel *label;
  GtkWidget *prefix;
  GtkWidget *suffix;
  char *text;
  char *value;
  gboolean disabled;
  gboolean destructive;
};

G_DEFINE_FINAL_TYPE(BmDropdownItem, bm_dropdown_item, GTK_TYPE_BUTTON)

enum {
  ITEM_PROP_0,
  ITEM_PROP_TEXT,
  ITEM_PROP_VALUE,
  ITEM_PROP_DISABLED,
  ITEM_PROP_DESTRUCTIVE,
  ITEM_N_PROPERTIES
};

static GParamSpec *item_properties[ITEM_N_PROPERTIES];

static void
item_set_text(BmDropdownItem *self, const char *text)
{
  if (g_strcmp0(self->text, text) == 0) return;
  g_free(self->text);
  self->text = g_strdup(text);
  gtk_label_set_text(self->label, self->text != NULL ? self->text : "");
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->text, -1);
  g_object_notify_by_pspec(G_OBJECT(self), item_properties[ITEM_PROP_TEXT]);
}

static void
item_set_value(BmDropdownItem *self, const char *value)
{
  if (g_strcmp0(self->value, value) == 0) return;
  g_free(self->value);
  self->value = g_strdup(value);
  g_object_notify_by_pspec(G_OBJECT(self), item_properties[ITEM_PROP_VALUE]);
}

const char *
bm_dropdown_item_get_text(BmDropdownItem *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN_ITEM(self), NULL);
  return self->text;
}

const char *
bm_dropdown_item_get_value(BmDropdownItem *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN_ITEM(self), NULL);
  return self->value;
}

gboolean
bm_dropdown_item_get_disabled(BmDropdownItem *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN_ITEM(self), FALSE);
  return self->disabled;
}

void
bm_dropdown_item_set_disabled(BmDropdownItem *self, gboolean disabled)
{
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(self));
  disabled = !!disabled;
  if (self->disabled == disabled) return;
  self->disabled = disabled;
  gtk_widget_set_sensitive(GTK_WIDGET(self), !disabled);
  g_object_notify_by_pspec(G_OBJECT(self), item_properties[ITEM_PROP_DISABLED]);
}

gboolean
bm_dropdown_item_get_destructive(BmDropdownItem *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN_ITEM(self), FALSE);
  return self->destructive;
}

void
bm_dropdown_item_set_destructive(BmDropdownItem *self, gboolean destructive)
{
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(self));
  destructive = !!destructive;
  if (self->destructive == destructive) return;
  self->destructive = destructive;
  if (destructive)
    gtk_widget_add_css_class(GTK_WIDGET(self), "bm-destructive");
  else
    gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-destructive");
  g_object_notify_by_pspec(G_OBJECT(self), item_properties[ITEM_PROP_DESTRUCTIVE]);
}

static void
item_set_slot(BmDropdownItem *self, GtkWidget **slot, GtkWidget *widget,
              gboolean before_label)
{
  if (*slot == widget) return;
  if (*slot != NULL) gtk_box_remove(self->layout, *slot);
  *slot = widget;
  if (widget == NULL) return;
  if (before_label)
    gtk_box_insert_child_after(self->layout, widget, NULL);
  else
    gtk_box_append(self->layout, widget);
}

void
bm_dropdown_item_set_prefix(BmDropdownItem *self, GtkWidget *prefix)
{
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(self));
  item_set_slot(self, &self->prefix, prefix, TRUE);
}

void
bm_dropdown_item_set_suffix(BmDropdownItem *self, GtkWidget *suffix)
{
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(self));
  item_set_slot(self, &self->suffix, suffix, FALSE);
}

static void
bm_dropdown_item_get_property(GObject *object, guint id, GValue *value,
                              GParamSpec *pspec)
{
  BmDropdownItem *self = BM_DROPDOWN_ITEM(object);
  switch (id) {
    case ITEM_PROP_TEXT: g_value_set_string(value, self->text); break;
    case ITEM_PROP_VALUE: g_value_set_string(value, self->value); break;
    case ITEM_PROP_DISABLED: g_value_set_boolean(value, self->disabled); break;
    case ITEM_PROP_DESTRUCTIVE:
      g_value_set_boolean(value, self->destructive);
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dropdown_item_set_property(GObject *object, guint id, const GValue *value,
                              GParamSpec *pspec)
{
  BmDropdownItem *self = BM_DROPDOWN_ITEM(object);
  switch (id) {
    case ITEM_PROP_TEXT: item_set_text(self, g_value_get_string(value)); break;
    case ITEM_PROP_VALUE: item_set_value(self, g_value_get_string(value)); break;
    case ITEM_PROP_DISABLED:
      bm_dropdown_item_set_disabled(self, g_value_get_boolean(value));
      break;
    case ITEM_PROP_DESTRUCTIVE:
      bm_dropdown_item_set_destructive(self, g_value_get_boolean(value));
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dropdown_item_finalize(GObject *object)
{
  BmDropdownItem *self = BM_DROPDOWN_ITEM(object);
  g_free(self->text);
  g_free(self->value);
  G_OBJECT_CLASS(bm_dropdown_item_parent_class)->finalize(object);
}

static void
bm_dropdown_item_class_init(BmDropdownItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_dropdown_item_get_property;
  object_class->set_property = bm_dropdown_item_set_property;
  object_class->finalize = bm_dropdown_item_finalize;
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_MENU_ITEM);
  item_properties[ITEM_PROP_TEXT] = g_param_spec_string(
    "text", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  item_properties[ITEM_PROP_VALUE] = g_param_spec_string(
    "value", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  item_properties[ITEM_PROP_DISABLED] = g_param_spec_boolean(
    "disabled", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  item_properties[ITEM_PROP_DESTRUCTIVE] = g_param_spec_boolean(
    "destructive", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, ITEM_N_PROPERTIES,
                                    item_properties);
}

static void
bm_dropdown_item_init(BmDropdownItem *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-dropdown-item");
  self->layout = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
  self->label = GTK_LABEL(gtk_label_new(NULL));
  gtk_label_set_xalign(self->label, 0.0f);
  gtk_widget_set_hexpand(GTK_WIDGET(self->label), TRUE);
  gtk_box_append(self->layout, GTK_WIDGET(self->label));
  gtk_button_set_child(GTK_BUTTON(self), GTK_WIDGET(self->layout));
}

GtkWidget *
bm_dropdown_item_new(const char *text, const char *value)
{
  return g_object_new(BM_TYPE_DROPDOWN_ITEM, "text", text, "value", value,
                      NULL);
}

struct _BmDropdownGroup {
  GtkBox parent_instance;
  char *label;
  gboolean destructive;
  GPtrArray *items;
};

G_DEFINE_FINAL_TYPE(BmDropdownGroup, bm_dropdown_group, GTK_TYPE_BOX)

enum { GROUP_PROP_0, GROUP_PROP_LABEL, GROUP_PROP_DESTRUCTIVE, GROUP_N_PROPERTIES };
enum { GROUP_SIGNAL_SELECT, GROUP_N_SIGNALS };
static GParamSpec *group_properties[GROUP_N_PROPERTIES];
static guint group_signals[GROUP_N_SIGNALS];

static void
on_group_item_clicked(GtkButton *button, gpointer data)
{
  BmDropdownGroup *self = BM_DROPDOWN_GROUP(data);
  g_signal_emit(self, group_signals[GROUP_SIGNAL_SELECT], 0,
                bm_dropdown_item_get_value(BM_DROPDOWN_ITEM(button)));
}

void
bm_dropdown_group_append(BmDropdownGroup *self, BmDropdownItem *item)
{
  g_return_if_fail(BM_IS_DROPDOWN_GROUP(self));
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(item));
  g_ptr_array_add(self->items, item);
  if (self->destructive) bm_dropdown_item_set_destructive(item, TRUE);
  g_signal_connect(item, "clicked", G_CALLBACK(on_group_item_clicked), self);
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(item));
}

gboolean
bm_dropdown_group_get_destructive(BmDropdownGroup *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN_GROUP(self), FALSE);
  return self->destructive;
}

void
bm_dropdown_group_set_destructive(BmDropdownGroup *self, gboolean destructive)
{
  g_return_if_fail(BM_IS_DROPDOWN_GROUP(self));
  destructive = !!destructive;
  if (self->destructive == destructive) return;
  self->destructive = destructive;
  if (destructive)
    gtk_widget_add_css_class(GTK_WIDGET(self), "bm-destructive");
  else
    gtk_widget_remove_css_class(GTK_WIDGET(self), "bm-destructive");
  for (guint i = 0; i < self->items->len; i++)
    bm_dropdown_item_set_destructive(g_ptr_array_index(self->items, i),
                                     destructive);
  g_object_notify_by_pspec(G_OBJECT(self),
                           group_properties[GROUP_PROP_DESTRUCTIVE]);
}

static void
group_set_label(BmDropdownGroup *self, const char *label)
{
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), group_properties[GROUP_PROP_LABEL]);
}

static void
bm_dropdown_group_get_property(GObject *object, guint id, GValue *value,
                               GParamSpec *pspec)
{
  BmDropdownGroup *self = BM_DROPDOWN_GROUP(object);
  if (id == GROUP_PROP_LABEL) g_value_set_string(value, self->label);
  else if (id == GROUP_PROP_DESTRUCTIVE)
    g_value_set_boolean(value, self->destructive);
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_dropdown_group_set_property(GObject *object, guint id, const GValue *value,
                               GParamSpec *pspec)
{
  BmDropdownGroup *self = BM_DROPDOWN_GROUP(object);
  if (id == GROUP_PROP_LABEL) group_set_label(self, g_value_get_string(value));
  else if (id == GROUP_PROP_DESTRUCTIVE)
    bm_dropdown_group_set_destructive(self, g_value_get_boolean(value));
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_dropdown_group_finalize(GObject *object)
{
  BmDropdownGroup *self = BM_DROPDOWN_GROUP(object);
  g_ptr_array_unref(self->items);
  g_free(self->label);
  G_OBJECT_CLASS(bm_dropdown_group_parent_class)->finalize(object);
}

static void
bm_dropdown_group_class_init(BmDropdownGroupClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_dropdown_group_get_property;
  object_class->set_property = bm_dropdown_group_set_property;
  object_class->finalize = bm_dropdown_group_finalize;
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_GROUP);
  group_properties[GROUP_PROP_LABEL] = g_param_spec_string(
    "label", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  group_properties[GROUP_PROP_DESTRUCTIVE] = g_param_spec_boolean(
    "destructive", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, GROUP_N_PROPERTIES,
                                    group_properties);
  group_signals[GROUP_SIGNAL_SELECT] = g_signal_new(
    "select", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
    G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
bm_dropdown_group_init(BmDropdownGroup *self)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-dropdown-group");
  self->items = g_ptr_array_new();
}

GtkWidget *
bm_dropdown_group_new(const char *label)
{
  return g_object_new(BM_TYPE_DROPDOWN_GROUP, "label", label, NULL);
}

struct _BmDropdown {
  GtkWidget parent_instance;
  GtkMenuButton *trigger;
  GtkPopover *popover;
  GtkBox *menu;
  char *label;
  gboolean disabled;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
  char *popover_custom_class;
};

G_DEFINE_FINAL_TYPE(BmDropdown, bm_dropdown, GTK_TYPE_WIDGET)

enum { PROP_0, PROP_LABEL, PROP_OPEN, PROP_DISABLED, PROP_THEME_SCOPE, N_PROPERTIES };
enum { SIGNAL_SELECT, SIGNAL_OPEN_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void
apply_dropdown_theme(BmDropdown *self)
{
  if (self->theme_scope == NULL) return;
  bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self),
                                         &self->custom_class);
  bm_theme_scope_apply_context_to_widget(self->theme_scope,
                                         GTK_WIDGET(self->popover),
                                         &self->popover_custom_class);
}

static void
on_dropdown_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  apply_dropdown_theme(BM_DROPDOWN(data));
}

static void
dropdown_emit_select(BmDropdown *self, const char *value)
{
  g_signal_emit(self, signals[SIGNAL_SELECT], 0, value);
  gtk_menu_button_popdown(self->trigger);
}

static void
on_dropdown_item_clicked(GtkButton *button, gpointer data)
{
  dropdown_emit_select(BM_DROPDOWN(data),
                       bm_dropdown_item_get_value(BM_DROPDOWN_ITEM(button)));
}

static void
on_dropdown_group_select(BmDropdownGroup *group, const char *value,
                         gpointer data)
{
  (void) group;
  dropdown_emit_select(BM_DROPDOWN(data), value);
}

static void
on_popover_visible(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  BmDropdown *self = BM_DROPDOWN(data);
  gboolean open = gtk_widget_get_visible(GTK_WIDGET(self->popover));
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_OPEN]);
  g_signal_emit(self, signals[SIGNAL_OPEN_CHANGED], 0, open);
}

static void
dropdown_set_label(BmDropdown *self, const char *label)
{
  if (g_strcmp0(self->label, label) == 0) return;
  g_free(self->label);
  self->label = g_strdup(label);
  gtk_menu_button_set_label(self->trigger,
                            self->label != NULL ? self->label : "");
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->trigger),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, self->label, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
}

GtkMenuButton *
bm_dropdown_get_trigger(BmDropdown *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN(self), NULL);
  return self->trigger;
}

void
bm_dropdown_append_item(BmDropdown *self, BmDropdownItem *item)
{
  g_return_if_fail(BM_IS_DROPDOWN(self));
  g_return_if_fail(BM_IS_DROPDOWN_ITEM(item));
  g_signal_connect(item, "clicked", G_CALLBACK(on_dropdown_item_clicked), self);
  gtk_box_append(self->menu, GTK_WIDGET(item));
}

void
bm_dropdown_append_group(BmDropdown *self, BmDropdownGroup *group)
{
  g_return_if_fail(BM_IS_DROPDOWN(self));
  g_return_if_fail(BM_IS_DROPDOWN_GROUP(group));
  g_signal_connect(group, "select", G_CALLBACK(on_dropdown_group_select), self);
  gtk_box_append(self->menu, GTK_WIDGET(group));
}

gboolean
bm_dropdown_get_open(BmDropdown *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN(self), FALSE);
  return gtk_widget_get_visible(GTK_WIDGET(self->popover));
}

void
bm_dropdown_set_open(BmDropdown *self, gboolean open)
{
  g_return_if_fail(BM_IS_DROPDOWN(self));
  if (open && !self->disabled) gtk_menu_button_popup(self->trigger);
  else gtk_menu_button_popdown(self->trigger);
}

gboolean
bm_dropdown_get_disabled(BmDropdown *self)
{
  g_return_val_if_fail(BM_IS_DROPDOWN(self), FALSE);
  return self->disabled;
}

void
bm_dropdown_set_disabled(BmDropdown *self, gboolean disabled)
{
  g_return_if_fail(BM_IS_DROPDOWN(self));
  disabled = !!disabled;
  if (self->disabled == disabled) return;
  self->disabled = disabled;
  gtk_widget_set_sensitive(GTK_WIDGET(self->trigger), !disabled);
  if (disabled) gtk_menu_button_popdown(self->trigger);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_DISABLED]);
}

void
bm_dropdown_set_theme_scope(BmDropdown *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_DROPDOWN(self));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self->popover),
                                           &self->popover_custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_dropdown_theme_notify), self);
  apply_dropdown_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
bm_dropdown_get_property(GObject *object, guint id, GValue *value,
                         GParamSpec *pspec)
{
  BmDropdown *self = BM_DROPDOWN(object);
  switch (id) {
    case PROP_LABEL: g_value_set_string(value, self->label); break;
    case PROP_OPEN: g_value_set_boolean(value, bm_dropdown_get_open(self)); break;
    case PROP_DISABLED: g_value_set_boolean(value, self->disabled); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->theme_scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dropdown_set_property(GObject *object, guint id, const GValue *value,
                         GParamSpec *pspec)
{
  BmDropdown *self = BM_DROPDOWN(object);
  switch (id) {
    case PROP_LABEL: dropdown_set_label(self, g_value_get_string(value)); break;
    case PROP_OPEN: bm_dropdown_set_open(self, g_value_get_boolean(value)); break;
    case PROP_DISABLED:
      bm_dropdown_set_disabled(self, g_value_get_boolean(value));
      break;
    case PROP_THEME_SCOPE:
      bm_dropdown_set_theme_scope(self, g_value_get_object(value));
      break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_dropdown_dispose(GObject *object)
{
  BmDropdown *self = BM_DROPDOWN(object);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  if (self->trigger != NULL) {
    gtk_widget_unparent(GTK_WIDGET(self->trigger));
    self->trigger = NULL;
    self->popover = NULL;
    self->menu = NULL;
  }
  G_OBJECT_CLASS(bm_dropdown_parent_class)->dispose(object);
}

static void
bm_dropdown_finalize(GObject *object)
{
  BmDropdown *self = BM_DROPDOWN(object);
  g_free(self->label);
  g_free(self->custom_class);
  g_free(self->popover_custom_class);
  G_OBJECT_CLASS(bm_dropdown_parent_class)->finalize(object);
}

static void
bm_dropdown_class_init(BmDropdownClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_dropdown_get_property;
  object_class->set_property = bm_dropdown_set_property;
  object_class->dispose = bm_dropdown_dispose;
  object_class->finalize = bm_dropdown_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  properties[PROP_LABEL] = g_param_spec_string(
    "label", NULL, NULL, NULL,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_OPEN] = g_param_spec_boolean(
    "open", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_DISABLED] = g_param_spec_boolean(
    "disabled", NULL, NULL, FALSE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object(
    "theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_SELECT] = g_signal_new(
    "select", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
    G_TYPE_NONE, 1, G_TYPE_STRING);
  signals[SIGNAL_OPEN_CHANGED] = g_signal_new(
    "open-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
    NULL, G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

static void
bm_dropdown_init(BmDropdown *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-dropdown");
  self->trigger = GTK_MENU_BUTTON(gtk_menu_button_new());
  gtk_widget_add_css_class(GTK_WIDGET(self->trigger), "bm-dropdown-trigger");
  gtk_menu_button_set_direction(self->trigger, GTK_ARROW_DOWN);
  gtk_widget_set_parent(GTK_WIDGET(self->trigger), GTK_WIDGET(self));
  self->popover = GTK_POPOVER(gtk_popover_new());
  gtk_widget_add_css_class(GTK_WIDGET(self->popover), "bm-dropdown-popover");
  self->menu = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
  gtk_widget_add_css_class(GTK_WIDGET(self->menu), "bm-dropdown-menu");
  gtk_popover_set_child(self->popover, GTK_WIDGET(self->menu));
  gtk_menu_button_set_popover(self->trigger, GTK_WIDGET(self->popover));
  g_signal_connect(self->popover, "notify::visible",
                   G_CALLBACK(on_popover_visible), self);
}

GtkWidget *
bm_dropdown_new(const char *label)
{
  return g_object_new(BM_TYPE_DROPDOWN, "label", label, NULL);
}
