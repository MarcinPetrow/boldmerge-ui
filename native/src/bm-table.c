#include "bm-private.h"

GType
bm_table_state_get_type(void)
{
  static gsize id = 0;
  if (g_once_init_enter(&id)) {
    static const GEnumValue values[] = {
      { BM_TABLE_STATE_READY, "BM_TABLE_STATE_READY", "ready" },
      { BM_TABLE_STATE_LOADING, "BM_TABLE_STATE_LOADING", "loading" },
      { BM_TABLE_STATE_EMPTY, "BM_TABLE_STATE_EMPTY", "empty" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmTableState", values);
    g_once_init_leave(&id, type);
  }
  return id;
}

GType
bm_table_sort_direction_get_type(void)
{
  static gsize id = 0;
  if (g_once_init_enter(&id)) {
    static const GEnumValue values[] = {
      { BM_TABLE_SORT_NONE, "BM_TABLE_SORT_NONE", "none" },
      { BM_TABLE_SORT_ASCENDING, "BM_TABLE_SORT_ASCENDING", "ascending" },
      { BM_TABLE_SORT_DESCENDING, "BM_TABLE_SORT_DESCENDING", "descending" },
      { 0, NULL, NULL }
    };
    GType type = g_enum_register_static("BmTableSortDirection", values);
    g_once_init_leave(&id, type);
  }
  return id;
}

struct _BmTable {
  GtkBox parent_instance;
  GtkStack *stack;
  GtkColumnView *view;
  GtkLabel *loading;
  GtkLabel *empty;
  GtkSelectionModel *model;
  gulong selection_id;
  char *sort_key;
  BmTableSortDirection sort_direction;
  BmTableState state;
  gboolean row_activation;
  gboolean syncing_sort;
  BmThemeScope *scope;
  gulong theme_id;
  char *custom_class;
};

G_DEFINE_FINAL_TYPE(BmTable, bm_table, GTK_TYPE_BOX)

enum {
  PROP_0, PROP_MODEL, PROP_STATE, PROP_ROW_ACTIVATION, PROP_SORT_KEY,
  PROP_SORT_DIRECTION, PROP_SELECTED_COUNT, PROP_THEME_SCOPE, N_PROPERTIES
};
enum { SIGNAL_SORT_REQUEST, SIGNAL_ROW_ACTIVATED, SIGNAL_SELECTION_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static GtkOrdering
equal_compare(gconstpointer a, gconstpointer b, gpointer data)
{
  (void) a; (void) b; (void) data;
  return GTK_ORDERING_EQUAL;
}

static void
apply_theme(BmTable *self)
{
  if (self->scope != NULL)
    bm_theme_scope_apply_context_to_widget(self->scope, GTK_WIDGET(self),
                                           &self->custom_class);
}

static void
on_theme_notify(GObject *object, GParamSpec *pspec, gpointer data)
{
  (void) object; (void) pspec;
  apply_theme(BM_TABLE(data));
}

guint
bm_table_get_selected_count(BmTable *self)
{
  g_return_val_if_fail(BM_IS_TABLE(self), 0);
  if (self->model == NULL) return 0;
  GtkBitset *selection = gtk_selection_model_get_selection(self->model);
  guint count = gtk_bitset_get_size(selection);
  gtk_bitset_unref(selection);
  return count;
}

static void
on_selection_changed(GtkSelectionModel *model, guint position, guint count,
                     gpointer data)
{
  (void) position; (void) count;
  BmTable *self = BM_TABLE(data);
  guint selected = bm_table_get_selected_count(self);
  guint total = g_list_model_get_n_items(G_LIST_MODEL(model));
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_SELECTED_COUNT]);
  g_signal_emit(self, signals[SIGNAL_SELECTION_CHANGED], 0, selected,
                total > 0 && selected == total);
}

static void
on_activate(GtkColumnView *view, guint position, gpointer data)
{
  (void) view;
  BmTable *self = BM_TABLE(data);
  if (!self->row_activation || self->model == NULL) return;
  GObject *item = g_list_model_get_item(G_LIST_MODEL(self->model), position);
  g_signal_emit(self, signals[SIGNAL_ROW_ACTIVATED], 0, position, item);
  g_clear_object(&item);
}

static GtkColumnViewColumn *
find_column(BmTable *self, const char *key)
{
  GListModel *columns = gtk_column_view_get_columns(self->view);
  for (guint i = 0; i < g_list_model_get_n_items(columns); i++) {
    GtkColumnViewColumn *column = g_list_model_get_item(columns, i);
    const char *column_key = g_object_get_data(G_OBJECT(column), "bm-sort-key");
    if (g_strcmp0(column_key, key) == 0) return column;
    g_object_unref(column);
  }
  return NULL;
}

static void
set_sort_state(BmTable *self, const char *key, BmTableSortDirection direction,
               gboolean emit)
{
  if (g_strcmp0(self->sort_key, key) == 0 && self->sort_direction == direction)
    return;
  g_free(self->sort_key);
  self->sort_key = g_strdup(key);
  self->sort_direction = direction;
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_SORT_KEY]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_SORT_DIRECTION]);
  if (emit && key != NULL && direction != BM_TABLE_SORT_NONE)
    g_signal_emit(self, signals[SIGNAL_SORT_REQUEST], 0, key, direction);
}

static void
on_sorter_changed(GtkSorter *sorter, GtkSorterChange change, gpointer data)
{
  (void) change;
  BmTable *self = BM_TABLE(data);
  if (self->syncing_sort || !GTK_IS_COLUMN_VIEW_SORTER(sorter)) return;
  GtkSortType order = GTK_SORT_ASCENDING;
  GtkColumnViewColumn *column = gtk_column_view_sorter_get_nth_sort_column(
    GTK_COLUMN_VIEW_SORTER(sorter), 0, &order);
  if (column == NULL) {
    set_sort_state(self, NULL, BM_TABLE_SORT_NONE, FALSE);
    return;
  }
  const char *key = g_object_get_data(G_OBJECT(column), "bm-sort-key");
  set_sort_state(self, key, order == GTK_SORT_DESCENDING
    ? BM_TABLE_SORT_DESCENDING : BM_TABLE_SORT_ASCENDING, TRUE);
}

GtkColumnView *bm_table_get_view(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), NULL); return self->view; }
GtkSelectionModel *bm_table_get_model(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), NULL); return self->model; }

void
bm_table_set_model(BmTable *self, GtkSelectionModel *model)
{
  g_return_if_fail(BM_IS_TABLE(self));
  if (self->model == model) return;
  if (self->model != NULL && self->selection_id != 0)
    g_signal_handler_disconnect(self->model, self->selection_id);
  g_set_object(&self->model, model);
  gtk_column_view_set_model(self->view, model);
  self->selection_id = model == NULL ? 0 : g_signal_connect(
    model, "selection-changed", G_CALLBACK(on_selection_changed), self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MODEL]);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_SELECTED_COUNT]);
}

GtkColumnViewColumn *
bm_table_append_column(BmTable *self, const char *title, const char *sort_key,
                       GtkListItemFactory *factory, gboolean expand)
{
  g_return_val_if_fail(BM_IS_TABLE(self), NULL);
  GtkColumnViewColumn *column = gtk_column_view_column_new(title, factory);
  gtk_column_view_column_set_expand(column, expand);
  gtk_column_view_column_set_resizable(column, TRUE);
  if (sort_key != NULL && *sort_key != '\0') {
    GtkSorter *sorter = GTK_SORTER(gtk_custom_sorter_new(equal_compare, NULL, NULL));
    gtk_column_view_column_set_sorter(column, sorter);
    g_object_unref(sorter);
    g_object_set_data_full(G_OBJECT(column), "bm-sort-key",
                           g_strdup(sort_key), g_free);
  }
  gtk_column_view_append_column(self->view, column);
  g_object_unref(column);
  return column;
}

BmTableState bm_table_get_state(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), BM_TABLE_STATE_READY); return self->state; }
void
bm_table_set_state(BmTable *self, BmTableState state)
{
  g_return_if_fail(BM_IS_TABLE(self));
  if (self->state == state) return;
  self->state = state;
  gtk_stack_set_visible_child_name(self->stack,
    state == BM_TABLE_STATE_LOADING ? "loading" :
    state == BM_TABLE_STATE_EMPTY ? "empty" : "ready");
  gtk_accessible_update_state(GTK_ACCESSIBLE(self->view),
                              GTK_ACCESSIBLE_STATE_BUSY,
                              state == BM_TABLE_STATE_LOADING, -1);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_STATE]);
}

void bm_table_set_loading_label(BmTable *self, const char *label) { g_return_if_fail(BM_IS_TABLE(self)); gtk_label_set_text(self->loading, label != NULL ? label : "Loading data"); }
void bm_table_set_empty_label(BmTable *self, const char *label) { g_return_if_fail(BM_IS_TABLE(self)); gtk_label_set_text(self->empty, label != NULL ? label : "No data"); }
gboolean bm_table_get_row_activation(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), FALSE); return self->row_activation; }
void bm_table_set_row_activation(BmTable *self, gboolean enabled) { g_return_if_fail(BM_IS_TABLE(self)); enabled=!!enabled; if(self->row_activation==enabled)return; self->row_activation=enabled; gtk_column_view_set_single_click_activate(self->view,enabled); g_object_notify_by_pspec(G_OBJECT(self),properties[PROP_ROW_ACTIVATION]); }
const char *bm_table_get_sort_key(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), NULL); return self->sort_key; }
BmTableSortDirection bm_table_get_sort_direction(BmTable *self) { g_return_val_if_fail(BM_IS_TABLE(self), BM_TABLE_SORT_NONE); return self->sort_direction; }

void
bm_table_set_sort(BmTable *self, const char *key, BmTableSortDirection direction)
{
  g_return_if_fail(BM_IS_TABLE(self));
  GtkColumnViewColumn *column = direction == BM_TABLE_SORT_NONE
    ? NULL : find_column(self, key);
  self->syncing_sort = TRUE;
  gtk_column_view_sort_by_column(self->view, column,
    direction == BM_TABLE_SORT_DESCENDING ? GTK_SORT_DESCENDING : GTK_SORT_ASCENDING);
  self->syncing_sort = FALSE;
  if (column != NULL) g_object_unref(column);
  set_sort_state(self, direction == BM_TABLE_SORT_NONE ? NULL : key,
                 direction, FALSE);
}

gboolean
bm_table_request_sort(BmTable *self, const char *key)
{
  g_return_val_if_fail(BM_IS_TABLE(self), FALSE);
  GtkColumnViewColumn *column = find_column(self, key);
  if (column == NULL) return FALSE;
  g_object_unref(column);
  BmTableSortDirection direction = g_strcmp0(self->sort_key, key) == 0 &&
    self->sort_direction == BM_TABLE_SORT_ASCENDING
      ? BM_TABLE_SORT_DESCENDING : BM_TABLE_SORT_ASCENDING;
  bm_table_set_sort(self, key, direction);
  g_signal_emit(self, signals[SIGNAL_SORT_REQUEST], 0, key, direction);
  return TRUE;
}

void
bm_table_set_theme_scope(BmTable *self, BmThemeScope *scope)
{
  g_return_if_fail(BM_IS_TABLE(self));
  if (self->scope == scope) return;
  if (self->scope != NULL && self->theme_id != 0)
    g_signal_handler_disconnect(self->scope, self->theme_id);
  bm_theme_scope_clear_context_from_widget(GTK_WIDGET(self), &self->custom_class);
  g_set_object(&self->scope, scope);
  self->theme_id = scope == NULL ? 0 : g_signal_connect(
    scope, "notify", G_CALLBACK(on_theme_notify), self);
  apply_theme(self);
  g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME_SCOPE]);
}

static void
get_property(GObject *object, guint id, GValue *value, GParamSpec *pspec)
{
  BmTable *self = BM_TABLE(object);
  switch (id) {
    case PROP_MODEL: g_value_set_object(value, self->model); break;
    case PROP_STATE: g_value_set_enum(value, self->state); break;
    case PROP_ROW_ACTIVATION: g_value_set_boolean(value, self->row_activation); break;
    case PROP_SORT_KEY: g_value_set_string(value, self->sort_key); break;
    case PROP_SORT_DIRECTION: g_value_set_enum(value, self->sort_direction); break;
    case PROP_SELECTED_COUNT: g_value_set_uint(value, bm_table_get_selected_count(self)); break;
    case PROP_THEME_SCOPE: g_value_set_object(value, self->scope); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec)
{
  BmTable *self = BM_TABLE(object);
  switch (id) {
    case PROP_MODEL: bm_table_set_model(self, g_value_get_object(value)); break;
    case PROP_STATE: bm_table_set_state(self, g_value_get_enum(value)); break;
    case PROP_ROW_ACTIVATION: bm_table_set_row_activation(self, g_value_get_boolean(value)); break;
    case PROP_THEME_SCOPE: bm_table_set_theme_scope(self, g_value_get_object(value)); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, id, pspec);
  }
}

static void
dispose(GObject *object)
{
  BmTable *self = BM_TABLE(object);
  if (self->model != NULL && self->selection_id != 0)
    g_signal_handler_disconnect(self->model, self->selection_id);
  if (self->scope != NULL && self->theme_id != 0)
    g_signal_handler_disconnect(self->scope, self->theme_id);
  g_clear_object(&self->model);
  g_clear_object(&self->scope);
  G_OBJECT_CLASS(bm_table_parent_class)->dispose(object);
}

static void
finalize(GObject *object)
{
  BmTable *self = BM_TABLE(object);
  g_free(self->sort_key);
  g_free(self->custom_class);
  G_OBJECT_CLASS(bm_table_parent_class)->finalize(object);
}

static void
bm_table_class_init(BmTableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = get_property;
  object_class->set_property = set_property;
  object_class->dispose = dispose;
  object_class->finalize = finalize;
  properties[PROP_MODEL] = g_param_spec_object("model",NULL,NULL,GTK_TYPE_SELECTION_MODEL,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_STATE] = g_param_spec_enum("state",NULL,NULL,BM_TYPE_TABLE_STATE,BM_TABLE_STATE_READY,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_ROW_ACTIVATION] = g_param_spec_boolean("row-activation",NULL,NULL,FALSE,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_SORT_KEY] = g_param_spec_string("sort-key",NULL,NULL,NULL,G_PARAM_READABLE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_SORT_DIRECTION] = g_param_spec_enum("sort-direction",NULL,NULL,BM_TYPE_TABLE_SORT_DIRECTION,BM_TABLE_SORT_NONE,G_PARAM_READABLE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_SELECTED_COUNT] = g_param_spec_uint("selected-count",NULL,NULL,0,G_MAXUINT,0,G_PARAM_READABLE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  properties[PROP_THEME_SCOPE] = g_param_spec_object("theme-scope",NULL,NULL,BM_TYPE_THEME_SCOPE,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties(object_class,N_PROPERTIES,properties);
  signals[SIGNAL_SORT_REQUEST]=g_signal_new("sort-request",G_TYPE_FROM_CLASS(klass),G_SIGNAL_RUN_LAST,0,NULL,NULL,NULL,G_TYPE_NONE,2,G_TYPE_STRING,BM_TYPE_TABLE_SORT_DIRECTION);
  signals[SIGNAL_ROW_ACTIVATED]=g_signal_new("row-activated",G_TYPE_FROM_CLASS(klass),G_SIGNAL_RUN_LAST,0,NULL,NULL,NULL,G_TYPE_NONE,2,G_TYPE_UINT,G_TYPE_OBJECT);
  signals[SIGNAL_SELECTION_CHANGED]=g_signal_new("selection-changed",G_TYPE_FROM_CLASS(klass),G_SIGNAL_RUN_LAST,0,NULL,NULL,NULL,G_TYPE_NONE,2,G_TYPE_UINT,G_TYPE_BOOLEAN);
}

static void
bm_table_init(BmTable *self)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
  gtk_widget_add_css_class(GTK_WIDGET(self),"bm-table");
  self->stack=GTK_STACK(gtk_stack_new());
  self->view=GTK_COLUMN_VIEW(gtk_column_view_new(NULL));
  self->loading=GTK_LABEL(gtk_label_new("Loading data"));
  self->empty=GTK_LABEL(gtk_label_new("No data"));
  GtkWidget *scroll=gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),GTK_WIDGET(self->view));
  gtk_widget_set_vexpand(scroll,TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(self->loading),"bm-table-state");
  gtk_widget_add_css_class(GTK_WIDGET(self->empty),"bm-table-state");
  gtk_stack_add_named(self->stack,scroll,"ready");
  gtk_stack_add_named(self->stack,GTK_WIDGET(self->loading),"loading");
  gtk_stack_add_named(self->stack,GTK_WIDGET(self->empty),"empty");
  gtk_stack_set_visible_child_name(self->stack,"ready");
  gtk_box_append(GTK_BOX(self),GTK_WIDGET(self->stack));
  gtk_column_view_set_show_row_separators(self->view,TRUE);
  g_signal_connect(self->view,"activate",G_CALLBACK(on_activate),self);
  GtkSorter *sorter=gtk_column_view_get_sorter(self->view);
  g_signal_connect(sorter,"changed",G_CALLBACK(on_sorter_changed),self);
}

GtkWidget *
bm_table_new(const char *label, GtkSelectionModel *model)
{
  BmTable *self=g_object_new(BM_TYPE_TABLE,NULL);
  gtk_accessible_update_property(GTK_ACCESSIBLE(self->view),GTK_ACCESSIBLE_PROPERTY_LABEL,label,-1);
  bm_table_set_model(self,model);
  return GTK_WIDGET(self);
}
