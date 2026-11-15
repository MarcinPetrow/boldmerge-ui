#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_BREADCRUMBS (bm_breadcrumbs_get_type())
G_DECLARE_FINAL_TYPE(BmBreadcrumbs, bm_breadcrumbs, BM, BREADCRUMBS, GtkBox)
#define BM_TYPE_BREADCRUMB_ITEM (bm_breadcrumb_item_get_type())
G_DECLARE_FINAL_TYPE(BmBreadcrumbItem, bm_breadcrumb_item, BM, BREADCRUMB_ITEM, GtkBox)
GtkWidget *bm_breadcrumbs_new(const char *label);
void bm_breadcrumbs_append(BmBreadcrumbs *self, BmBreadcrumbItem *item);
void bm_breadcrumbs_set_theme_scope(BmBreadcrumbs *self, BmThemeScope *scope);
GtkWidget *bm_breadcrumb_item_new(const char *text, const char *uri, gboolean current);
const char *bm_breadcrumb_item_get_text(BmBreadcrumbItem *self);
const char *bm_breadcrumb_item_get_uri(BmBreadcrumbItem *self);
gboolean bm_breadcrumb_item_get_current(BmBreadcrumbItem *self);
void bm_breadcrumb_item_set_current(BmBreadcrumbItem *self, gboolean current);
G_END_DECLS
