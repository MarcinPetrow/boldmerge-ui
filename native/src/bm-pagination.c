#include "bm-private.h"

struct _BmPagination {
  GtkBox parent_instance;
  GtkButton *previous;
  GtkBox *pages;
  GtkButton *next;
  GtkLabel *summary;
  GtkBox *page_size_box;
  guint page;
  guint page_size;
  guint total_items;
  guint sibling_count;
  gboolean disabled;
  BmThemeScope *scope;
  gulong notify_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmPagination, bm_pagination, GTK_TYPE_BOX)

enum {
  PROP_0, PROP_PAGE, PROP_PAGE_SIZE, PROP_TOTAL_ITEMS, PROP_PAGE_COUNT,
  PROP_SIBLING_COUNT, PROP_DISABLED, PROP_THEME_SCOPE, N_PROPERTIES
};
enum { SIGNAL_PAGE_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

guint
bm_pagination_get_page_count(BmPagination *self)
{
  g_return_val_if_fail(BM_IS_PAGINATION(self), 1);
  return MAX(1, (self->total_items + self->page_size - 1) / self->page_size);
}

static void
apply_theme(BmPagination *self)
{
  if (self->scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object;
  (void) pspec;
  apply_theme(BM_PAGINATION(data));
}

static void
clear_pages(BmPagination *self)
{
  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(self->pages));
  while (child != NULL) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(self->pages, child);
    child = next;
  }
}

static void
on_page_clicked(GtkButton *button, gpointer data)
{
  bm_pagination_go_to_page(BM_PAGINATION(data),
    GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "bm-page")));
}

static void rebuild(BmPagination *self);

static void
append_page(BmPagination *self, guint page)
{
  g_autofree char *text = g_strdup_printf("%u", page);
  GtkWidget *button = gtk_button_new_with_label(text);
  gtk_widget_add_css_class(button, "bm-pagination-page");
  if (page == self->page)
    gtk_widget_add_css_class(button, "bm-current");
  g_autofree char *label = g_strdup_printf("Page %u", page);
  gtk_accessible_update_property(GTK_ACCESSIBLE(button),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
  g_object_set_data(G_OBJECT(button), "bm-page", GUINT_TO_POINTER(page));
  g_signal_connect(button, "clicked", G_CALLBACK(on_page_clicked), self);
  gtk_widget_set_sensitive(button, !self->disabled);
  gtk_box_append(self->pages, button);
}

static void
append_ellipsis(BmPagination *self)
{
  GtkWidget *label = gtk_label_new("…");
  gtk_widget_add_css_class(label, "bm-pagination-ellipsis");
  gtk_accessible_update_state(GTK_ACCESSIBLE(label),
                              GTK_ACCESSIBLE_STATE_HIDDEN, TRUE, -1);
  gtk_box_append(self->pages, label);
}

static void
rebuild(BmPagination *self)
{
  guint count = bm_pagination_get_page_count(self);
  clear_pages(self);
  gtk_widget_set_sensitive(GTK_WIDGET(self->previous),
                           !self->disabled && self->page > 1);
  gtk_widget_set_sensitive(GTK_WIDGET(self->next),
                           !self->disabled && self->page < count);
  if (count <= self->sibling_count * 2 + 5) {
    for (guint page = 1; page <= count; page++) append_page(self, page);
  } else {
    guint start = MAX(2, self->page > self->sibling_count
      ? self->page - self->sibling_count : 2);
    guint end = MIN(count - 1, self->page + self->sibling_count);
    append_page(self, 1);
    if (start > 2) append_ellipsis(self);
    for (guint page = start; page <= end; page++) append_page(self, page);
    if (end < count - 1) append_ellipsis(self);
    append_page(self, count);
  }
  g_autofree char *summary = g_strdup_printf("Page %u of %u", self->page, count);
  gtk_label_set_text(self->summary, summary);
}

guint bm_pagination_get_page(BmPagination *self) { g_return_val_if_fail(BM_IS_PAGINATION(self), 1); return self->page; }
guint bm_pagination_get_page_size(BmPagination *self) { g_return_val_if_fail(BM_IS_PAGINATION(self), 25); return self->page_size; }
guint bm_pagination_get_total_items(BmPagination *self) { g_return_val_if_fail(BM_IS_PAGINATION(self), 0); return self->total_items; }
guint bm_pagination_get_sibling_count(BmPagination *self) { g_return_val_if_fail(BM_IS_PAGINATION(self), 1); return self->sibling_count; }
gboolean bm_pagination_get_disabled(BmPagination *self) { g_return_val_if_fail(BM_IS_PAGINATION(self), FALSE); return self->disabled; }

void
bm_pagination_set_page(BmPagination *self, guint page)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  page = CLAMP(page, 1, bm_pagination_get_page_count(self));
  if (self->page == page) return;
  self->page = page;
  rebuild(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PAGE]);
}

void
bm_pagination_set_page_size(BmPagination *self, guint page_size)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  page_size = page_size == 0 ? 25 : page_size;
  if (self->page_size == page_size) return;
  self->page_size = page_size;
  self->page = MIN(self->page, bm_pagination_get_page_count(self));
  rebuild(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PAGE_SIZE]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PAGE_COUNT]);
}

void
bm_pagination_set_total_items(BmPagination *self, guint total_items)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  if (self->total_items == total_items) return;
  self->total_items = total_items;
  self->page = MIN(self->page, bm_pagination_get_page_count(self));
  rebuild(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_TOTAL_ITEMS]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_PAGE_COUNT]);
}

void
bm_pagination_set_sibling_count(BmPagination *self, guint count)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  if (self->sibling_count == count) return;
  self->sibling_count = count;
  rebuild(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_SIBLING_COUNT]);
}

void
bm_pagination_set_disabled(BmPagination *self, gboolean disabled)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  disabled = !!disabled;
  if (self->disabled == disabled) return;
  self->disabled = disabled;
  rebuild(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_DISABLED]);
}

gboolean
bm_pagination_go_to_page(BmPagination *self, guint page)
{
  g_return_val_if_fail(BM_IS_PAGINATION(self), FALSE);
  if (self->disabled) return FALSE;
  page = CLAMP(page, 1, bm_pagination_get_page_count(self));
  if (page == self->page) return FALSE;
  bm_pagination_set_page(self, page);
  g_signal_emit(self, signals[SIGNAL_PAGE_CHANGED], 0, self->page,
                self->page_size, self->total_items);
  return TRUE;
}

gboolean bm_pagination_next(BmPagination *self) { return bm_pagination_go_to_page(self, self->page + 1); }
gboolean bm_pagination_previous(BmPagination *self) { return bm_pagination_go_to_page(self, self->page > 1 ? self->page - 1 : 1); }

void
bm_pagination_set_page_size_widget(BmPagination *self, GtkWidget *widget)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  GtkWidget *old = gtk_widget_get_first_child(GTK_WIDGET(self->page_size_box));
  if (old != NULL) gtk_box_remove(self->page_size_box, old);
  if (widget != NULL) gtk_box_append(self->page_size_box, widget);
  gtk_widget_set_visible(GTK_WIDGET(self->page_size_box), widget != NULL);
}

void
bm_pagination_set_theme_scope(BmPagination *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_PAGINATION(self));
  if (self->scope == scope) return;
  if (self->scope != NULL && self->notify_id != 0)
    g_signal_handler_disconnect(self->scope, self->notify_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->scope, scope);
  self->notify_id = scope == NULL ? 0
    : g_signal_connect(scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmPagination *self = BM_PAGINATION(object);
  switch (id) {
    case PROP_PAGE: g_value_set_uint(value, self->page); break;
    case PROP_PAGE_SIZE: g_value_set_uint(value, self->page_size); break;
    case PROP_TOTAL_ITEMS: g_value_set_uint(value, self->total_items); break;
    case PROP_PAGE_COUNT: g_value_set_uint(value, bm_pagination_get_page_count(self)); break;
    case PROP_SIBLING_COUNT: g_value_set_uint(value, self->sibling_count); break;
    case PROP_DISABLED: g_value_set_boolean(value, self->disabled); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmPagination *self = BM_PAGINATION(object);
  switch (id) {
    case PROP_PAGE: bm_pagination_set_page(self, g_value_get_uint(value)); break;
    case PROP_PAGE_SIZE: bm_pagination_set_page_size(self, g_value_get_uint(value)); break;
    case PROP_TOTAL_ITEMS: bm_pagination_set_total_items(self, g_value_get_uint(value)); break;
    case PROP_SIBLING_COUNT: bm_pagination_set_sibling_count(self, g_value_get_uint(value)); break;
    case PROP_DISABLED: bm_pagination_set_disabled(self, g_value_get_boolean(value)); break;
    case PROP_THEME_SCOPE: bm_pagination_set_theme_scope(self, g_value_get_object(value)); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
dispose(GObject *object)
{
  BmPagination *self = BM_PAGINATION(object);
  if (self->scope != NULL && self->notify_id != 0)
    g_signal_handler_disconnect(self->scope, self->notify_id);
  g_clear_object(&self->scope);
  G_OBJECT_CLASS(bm_pagination_parent_class)->dispose(object);
}

static void
finalize(GObject *object)
{
  BmPagination *self = BM_PAGINATION(object);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_pagination_parent_class)->finalize(object);
}

static void
bm_pagination_class_init(BmPaginationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = get_property;
  object_class->set_property = set_property;
  object_class->dispose = dispose;
  object_class->finalize = finalize;
  gtk_widget_class_set_accessible_role(GTK_WIDGET_CLASS(klass),
                                       GTK_ACCESSIBLE_ROLE_NAVIGATION);
  properties[PROP_PAGE] = g_param_spec_uint("page", NULL, NULL, 1, G_MAXUINT, 1, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_PAGE_SIZE] = g_param_spec_uint("page-size", NULL, NULL, 1, G_MAXUINT, 25, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_TOTAL_ITEMS] = g_param_spec_uint("total-items", NULL, NULL, 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_PAGE_COUNT] = g_param_spec_uint("page-count", NULL, NULL, 1, G_MAXUINT, 1, G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_SIBLING_COUNT] = g_param_spec_uint("sibling-count", NULL, NULL, 0, G_MAXUINT, 1, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_DISABLED] = g_param_spec_boolean("disabled", NULL, NULL, FALSE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope", NULL, NULL, BM_TYPE_THEME_SCOPE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
  signals[SIGNAL_PAGE_CHANGED] = g_signal_new("page-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 3, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
}

static void
bm_pagination_init(BmPagination *self)
{
  self->page = 1;
  self->page_size = 25;
  self->sibling_count = 1;
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_HORIZONTAL);
  gtk_box_set_spacing(GTK_BOX(self), 12);
  gtk_widget_add_css_class(GTK_WIDGET(self), "bm-pagination");
  self->previous = GTK_BUTTON(gtk_button_new_with_label("Previous"));
  self->pages = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  self->next = GTK_BUTTON(gtk_button_new_with_label("Next"));
  self->summary = GTK_LABEL(gtk_label_new(NULL));
  self->page_size_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(self->previous), "bm-pagination-nav");
  gtk_widget_add_css_class(GTK_WIDGET(self->next), "bm-pagination-nav");
  gtk_widget_add_css_class(GTK_WIDGET(self->pages), "bm-pagination-pages");
  gtk_widget_add_css_class(GTK_WIDGET(self->summary), "bm-pagination-summary");
  gtk_widget_set_visible(GTK_WIDGET(self->page_size_box), FALSE);
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->previous));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->pages));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->next));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->summary));
  gtk_box_append(GTK_BOX(self), GTK_WIDGET(self->page_size_box));
  g_signal_connect_swapped(self->previous, "clicked",
                           G_CALLBACK(bm_pagination_previous), self);
  g_signal_connect_swapped(self->next, "clicked",
                           G_CALLBACK(bm_pagination_next), self);
  rebuild(self);
}

GtkWidget *
bm_pagination_new(const char *label)
{
  GtkWidget *widget = g_object_new(BM_TYPE_PAGINATION, NULL);
  gtk_accessible_update_property(GTK_ACCESSIBLE(widget),
                                 GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 label != NULL ? label : "Pagination", -1);
  return widget;
}
