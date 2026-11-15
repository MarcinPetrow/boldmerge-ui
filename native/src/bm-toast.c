#include "bm-private.h"

GType
bm_toast_tone_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_TOAST_TONE_NEUTRAL, "BM_TOAST_TONE_NEUTRAL", "neutral" },
      { BM_TOAST_TONE_SUCCESS, "BM_TOAST_TONE_SUCCESS", "success" },
      { BM_TOAST_TONE_WARNING, "BM_TOAST_TONE_WARNING", "warning" },
      { BM_TOAST_TONE_ERROR, "BM_TOAST_TONE_ERROR", "error" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmToastTone", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

GType
bm_toast_close_reason_get_type(void)
{
  static gsize type_id = 0;
  if (g_once_init_enter(&type_id)) {
    static const GEnumValue values[] = {
      { BM_TOAST_CLOSE_REASON_API, "BM_TOAST_CLOSE_REASON_API", "api" },
      { BM_TOAST_CLOSE_REASON_DISMISS, "BM_TOAST_CLOSE_REASON_DISMISS", "dismiss" },
      { BM_TOAST_CLOSE_REASON_TIMEOUT, "BM_TOAST_CLOSE_REASON_TIMEOUT", "timeout" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmToastCloseReason", values);
    g_once_init_leave(&type_id, type);
  }
  return type_id;
}

struct _BmToast {
  GObject parent_instance;
  AdwToast *native_toast;
  char *id;
  char *title;
  char *message;
  BmToastTone tone;
  guint duration;
  BmToastCloseReason pending_reason;
  gboolean open;
};

G_DEFINE_FINAL_TYPE(BmToast, bm_toast, G_TYPE_OBJECT)

enum { PROP_0, PROP_ID, PROP_TITLE, PROP_MESSAGE, PROP_TONE, PROP_DURATION, PROP_OPEN, N_PROPERTIES };
enum { SIGNAL_CLOSED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];
static guint toast_sequence;

static void
sync_native_toast(BmToast *self)
{
  g_autofree char *title = g_markup_escape_text(
    self->title != NULL ? self->title : "", -1);
  g_autofree char *message = g_markup_escape_text(
    self->message != NULL ? self->message : "", -1);
  g_autofree char *display = self->message != NULL && *self->message != '\0'
    ? g_strdup_printf("<b>%s</b> — %s", title, message)
    : g_strdup(title);
  adw_toast_set_title(self->native_toast, display);
  adw_toast_set_timeout(self->native_toast, self->duration);
  adw_toast_set_priority(self->native_toast,
    self->tone == BM_TOAST_TONE_WARNING || self->tone == BM_TOAST_TONE_ERROR
      ? ADW_TOAST_PRIORITY_HIGH : ADW_TOAST_PRIORITY_NORMAL);
}

static void
on_native_dismissed(AdwToast *toast, gpointer data)
{
  (void) toast;
  BmToast *self = BM_TOAST(data);
  if (!self->open) return;
  self->open = FALSE;
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_OPEN]);
  g_signal_emit(self, signals[SIGNAL_CLOSED], 0, self->pending_reason);
  self->pending_reason = BM_TOAST_CLOSE_REASON_TIMEOUT;
}

const char *bm_toast_get_id(BmToast *self) { g_return_val_if_fail(BM_IS_TOAST(self), NULL); return self->id; }
const char *bm_toast_get_title(BmToast *self) { g_return_val_if_fail(BM_IS_TOAST(self), NULL); return self->title; }
const char *bm_toast_get_message(BmToast *self) { g_return_val_if_fail(BM_IS_TOAST(self), NULL); return self->message; }
BmToastTone bm_toast_get_tone(BmToast *self) { g_return_val_if_fail(BM_IS_TOAST(self), BM_TOAST_TONE_NEUTRAL); return self->tone; }
guint bm_toast_get_duration(BmToast *self) { g_return_val_if_fail(BM_IS_TOAST(self), 0); return self->duration; }

void
bm_toast_dismiss(BmToast *self, BmToastCloseReason reason)
{
  g_return_if_fail(BM_IS_TOAST(self));
  if (!self->open) return;
  self->pending_reason = reason;
  adw_toast_dismiss(self->native_toast);
}

static void
toast_set_string(char **field, const char *value)
{
  g_free(*field);
  *field = g_strdup(value);
}

static void
bm_toast_get_property(GObject *object, guint id, GValue *value,
                      GParamSpec *pspec)
{
  BmToast *self = BM_TOAST(object);
  switch (id) {
    case PROP_ID: g_value_set_string(value, self->id); break;
    case PROP_TITLE: g_value_set_string(value, self->title); break;
    case PROP_MESSAGE: g_value_set_string(value, self->message); break;
    case PROP_TONE: g_value_set_enum(value, self->tone); break;
    case PROP_DURATION: g_value_set_uint(value, self->duration); break;
    case PROP_OPEN: g_value_set_boolean(value, self->open); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
bm_toast_set_property(GObject *object, guint id, const GValue *value,
                      GParamSpec *pspec)
{
  BmToast *self = BM_TOAST(object);
  switch (id) {
    case PROP_TITLE: toast_set_string(&self->title, g_value_get_string(value)); break;
    case PROP_MESSAGE: toast_set_string(&self->message, g_value_get_string(value)); break;
    case PROP_TONE: self->tone = g_value_get_enum(value); break;
    case PROP_DURATION: self->duration = g_value_get_uint(value); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec); return;
  }
  sync_native_toast(self);
}

static void
bm_toast_dispose(GObject *object)
{
  BmToast *self = BM_TOAST(object);
  g_clear_object(&self->native_toast);
  G_OBJECT_CLASS(bm_toast_parent_class)->dispose(object);
}

static void
bm_toast_finalize(GObject *object)
{
  BmToast *self = BM_TOAST(object);
  g_free(self->id);
  g_free(self->title);
  g_free(self->message);
  G_OBJECT_CLASS(bm_toast_parent_class)->finalize(object);
}

static void
bm_toast_class_init(BmToastClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = bm_toast_get_property;
  object_class->set_property = bm_toast_set_property;
  object_class->dispose = bm_toast_dispose;
  object_class->finalize = bm_toast_finalize;
  properties[PROP_ID] = g_param_spec_string(
    "id", NULL, NULL, NULL, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_TITLE] = g_param_spec_string(
    "title", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_MESSAGE] = g_param_spec_string(
    "message", NULL, NULL, NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_TONE] = g_param_spec_enum(
    "tone", NULL, NULL, BM_TYPE_TOAST_TONE, BM_TOAST_TONE_NEUTRAL,
    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_DURATION] = g_param_spec_uint(
    "duration", NULL, NULL, 0, G_MAXUINT, 5,
    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_OPEN] = g_param_spec_boolean(
    "open", NULL, NULL, TRUE, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_CLOSED] = g_signal_new(
    "closed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
    G_TYPE_NONE, 1, BM_TYPE_TOAST_CLOSE_REASON);
}

static void
bm_toast_init(BmToast *self)
{
  self->id = g_strdup_printf("bm-toast-%u", ++toast_sequence);
  self->duration = 5;
  self->open = TRUE;
  self->pending_reason = BM_TOAST_CLOSE_REASON_TIMEOUT;
  self->native_toast = adw_toast_new("");
  g_signal_connect(self->native_toast, "dismissed",
                   G_CALLBACK(on_native_dismissed), self);
}

BmToast *
bm_toast_new(const char *title, const char *message, BmToastTone tone,
             guint duration_seconds)
{
  return g_object_new(BM_TYPE_TOAST, "title", title, "message", message,
                      "tone", tone, "duration", duration_seconds, NULL);
}

struct _BmToastRegion {
  GtkWidget parent_instance;
  AdwToastOverlay *overlay;
  GPtrArray *toasts;
  BmThemeScope *theme_scope;
  gulong theme_notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmToastRegion, bm_toast_region, GTK_TYPE_WIDGET)

enum { REGION_PROP_0, REGION_PROP_COUNT, REGION_PROP_THEME_SCOPE, REGION_N_PROPERTIES };
static GParamSpec *region_properties[REGION_N_PROPERTIES];

static void
apply_region_theme(BmToastRegion *self)
{
  if (self->theme_scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->theme_scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_region_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  apply_region_theme(BM_TOAST_REGION(data));
}

static void
on_region_toast_closed(BmToast *toast, BmToastCloseReason reason, gpointer data)
{
  (void) reason;
  BmToastRegion *self = BM_TOAST_REGION(data);
  for (guint i = 0; i < self->toasts->len; i++) {
    if (g_ptr_array_index(self->toasts, i) == toast) {
      g_ptr_array_remove_index(self->toasts, i);
      g_object_notify_by_pspec(G_OBJECT(self),
                               region_properties[REGION_PROP_COUNT]);
      return;
    }
  }
}

void
bm_toast_region_set_child(BmToastRegion *self, GtkWidget *child)
{
  g_return_if_fail(BM_IS_TOAST_REGION(self));
  adw_toast_overlay_set_child(self->overlay, child);
}

void
bm_toast_region_add(BmToastRegion *self, BmToast *toast)
{
  g_return_if_fail(BM_IS_TOAST_REGION(self));
  g_return_if_fail(BM_IS_TOAST(toast));
  for (guint i = 0; i < self->toasts->len; i++)
    if (g_ptr_array_index(self->toasts, i) == toast) return;
  g_ptr_array_add(self->toasts, g_object_ref(toast));
  g_signal_connect_object(toast, "closed", G_CALLBACK(on_region_toast_closed),
                          self, 0);
  adw_toast_overlay_add_toast(self->overlay,
                              g_object_ref(toast->native_toast));
  g_object_notify_by_pspec(G_OBJECT(self), region_properties[REGION_PROP_COUNT]);
}

BmToast *
bm_toast_region_push(BmToastRegion *self, const char *title,
                     const char *message, BmToastTone tone,
                     guint duration_seconds)
{
  g_return_val_if_fail(BM_IS_TOAST_REGION(self), NULL);
  BmToast *toast = bm_toast_new(title, message, tone, duration_seconds);
  bm_toast_region_add(self, toast);
  return toast;
}

guint
bm_toast_region_get_count(BmToastRegion *self)
{
  g_return_val_if_fail(BM_IS_TOAST_REGION(self), 0);
  return self->toasts->len;
}

void
bm_toast_region_clear(BmToastRegion *self)
{
  g_return_if_fail(BM_IS_TOAST_REGION(self));
  while (self->toasts->len > 0) {
    BmToast *toast = g_ptr_array_index(self->toasts, self->toasts->len - 1);
    g_signal_handlers_disconnect_by_func(toast, on_region_toast_closed, self);
    bm_toast_dismiss(toast, BM_TOAST_CLOSE_REASON_API);
    g_ptr_array_remove_index(self->toasts, self->toasts->len - 1);
  }
  g_object_notify_by_pspec(G_OBJECT(self), region_properties[REGION_PROP_COUNT]);
}

void
bm_toast_region_set_theme_scope(BmToastRegion *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_TOAST_REGION(self));
  if (self->theme_scope == scope) return;
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->theme_scope, scope);
  self->theme_notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_region_theme_notify), self);
  apply_region_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self),
                           region_properties[REGION_PROP_THEME_SCOPE]);
}

static void
bm_toast_region_get_property(GObject *object, guint id, GValue *value,
                             GParamSpec *pspec)
{
  BmToastRegion *self = BM_TOAST_REGION(object);
  if (id == REGION_PROP_COUNT) g_value_set_uint(value, self->toasts->len);
  else if (id == REGION_PROP_THEME_SCOPE)
    g_value_set_object(value, self->theme_scope);
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_toast_region_set_property(GObject *object, guint id, const GValue *value,
                             GParamSpec *pspec)
{
  if (id == REGION_PROP_THEME_SCOPE)
    bm_toast_region_set_theme_scope(BM_TOAST_REGION(object),
                                    g_value_get_object(value));
  else G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
}

static void
bm_toast_region_dispose(GObject *object)
{
  BmToastRegion *self = BM_TOAST_REGION(object);
  bm_toast_region_clear(self);
  if (self->theme_scope != NULL && self->theme_notify_id != 0)
    g_signal_handler_disconnect(self->theme_scope, self->theme_notify_id);
  g_clear_object(&self->theme_scope);
  if (self->overlay != NULL) {
    gtk_widget_unparent(GTK_WIDGET(self->overlay));
    self->overlay = NULL;
  }
  G_OBJECT_CLASS(bm_toast_region_parent_class)->dispose(object);
}

static void
bm_toast_region_finalize(GObject *object)
{
  BmToastRegion *self = BM_TOAST_REGION(object);
  g_ptr_array_unref(self->toasts);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_toast_region_parent_class)->finalize(object);
}

static void
bm_toast_region_class_init(BmToastRegionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  object_class->get_property = bm_toast_region_get_property;
  object_class->set_property = bm_toast_region_set_property;
  object_class->dispose = bm_toast_region_dispose;
  object_class->finalize = bm_toast_region_finalize;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_REGION);
  region_properties[REGION_PROP_COUNT] = g_param_spec_uint(
    "count", NULL, NULL, 0, G_MAXUINT, 0,
    G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  region_properties[REGION_PROP_THEME_SCOPE] = g_param_spec_object(
    "theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE,
    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, REGION_N_PROPERTIES,
                                    region_properties);
}

static void
bm_toast_region_init(BmToastRegion *self)
{
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-toast-region");
  self->toasts = g_ptr_array_new_with_free_func(g_object_unref);
  self->overlay = ADW_TOAST_OVERLAY(adw_toast_overlay_new());
  gtk_widget_set_parent(GTK_WIDGET(self->overlay), GTK_WIDGET(self));
}

GtkWidget *
bm_toast_region_new(const char *label)
{
  GtkWidget *widget = g_object_new(BM_TYPE_TOAST_REGION, NULL);
  gtk_accessible_update_property(GTK_ACCESSIBLE(widget),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
  return widget;
}
