#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_PAGINATION (bm_pagination_get_type())
G_DECLARE_FINAL_TYPE(BmPagination, bm_pagination, BM, PAGINATION, GtkBox)

GtkWidget *bm_pagination_new(const char *label);
guint bm_pagination_get_page(BmPagination *self);
void bm_pagination_set_page(BmPagination *self, guint page);
guint bm_pagination_get_page_size(BmPagination *self);
void bm_pagination_set_page_size(BmPagination *self, guint page_size);
guint bm_pagination_get_total_items(BmPagination *self);
void bm_pagination_set_total_items(BmPagination *self, guint total_items);
guint bm_pagination_get_page_count(BmPagination *self);
guint bm_pagination_get_sibling_count(BmPagination *self);
void bm_pagination_set_sibling_count(BmPagination *self, guint count);
gboolean bm_pagination_get_disabled(BmPagination *self);
void bm_pagination_set_disabled(BmPagination *self, gboolean disabled);
gboolean bm_pagination_go_to_page(BmPagination *self, guint page);
gboolean bm_pagination_next(BmPagination *self);
gboolean bm_pagination_previous(BmPagination *self);
void bm_pagination_set_page_size_widget(BmPagination *self, GtkWidget *widget);
void bm_pagination_set_theme_scope(BmPagination *self, BmThemeScope *scope);

G_END_DECLS
