#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum {
  BM_TABLE_STATE_READY,
  BM_TABLE_STATE_LOADING,
  BM_TABLE_STATE_EMPTY
} BmTableState;

typedef enum {
  BM_TABLE_SORT_NONE,
  BM_TABLE_SORT_ASCENDING,
  BM_TABLE_SORT_DESCENDING
} BmTableSortDirection;

#define BM_TYPE_TABLE_STATE (bm_table_state_get_type())
GType bm_table_state_get_type(void) G_GNUC_CONST;
#define BM_TYPE_TABLE_SORT_DIRECTION (bm_table_sort_direction_get_type())
GType bm_table_sort_direction_get_type(void) G_GNUC_CONST;

#define BM_TYPE_TABLE (bm_table_get_type())
G_DECLARE_FINAL_TYPE(BmTable, bm_table, BM, TABLE, GtkBox)

GtkWidget *bm_table_new(const char *label, GtkSelectionModel *model);
GtkColumnView *bm_table_get_view(BmTable *self);
GtkSelectionModel *bm_table_get_model(BmTable *self);
void bm_table_set_model(BmTable *self, GtkSelectionModel *model);
GtkColumnViewColumn *bm_table_append_column(BmTable *self, const char *title,
                                            const char *sort_key,
                                            GtkListItemFactory *factory,
                                            gboolean expand);
BmTableState bm_table_get_state(BmTable *self);
void bm_table_set_state(BmTable *self, BmTableState state);
void bm_table_set_loading_label(BmTable *self, const char *label);
void bm_table_set_empty_label(BmTable *self, const char *label);
gboolean bm_table_get_row_activation(BmTable *self);
void bm_table_set_row_activation(BmTable *self, gboolean enabled);
const char *bm_table_get_sort_key(BmTable *self);
BmTableSortDirection bm_table_get_sort_direction(BmTable *self);
void bm_table_set_sort(BmTable *self, const char *key,
                       BmTableSortDirection direction);
gboolean bm_table_request_sort(BmTable *self, const char *key);
guint bm_table_get_selected_count(BmTable *self);
void bm_table_set_theme_scope(BmTable *self, BmThemeScope *scope);

G_END_DECLS
