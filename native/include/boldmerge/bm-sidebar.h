#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_SIDEBAR_ITEM (bm_sidebar_item_get_type())
G_DECLARE_FINAL_TYPE(BmSidebarItem, bm_sidebar_item, BM, SIDEBAR_ITEM, GtkButton)
#define BM_TYPE_SIDEBAR (bm_sidebar_get_type())
G_DECLARE_FINAL_TYPE(BmSidebar, bm_sidebar, BM, SIDEBAR, GtkBox)
GtkWidget *bm_sidebar_item_new(const char *text, const char *uri, GtkWidget *icon);
const char *bm_sidebar_item_get_text(BmSidebarItem *self);
const char *bm_sidebar_item_get_uri(BmSidebarItem *self);
gboolean bm_sidebar_item_get_active(BmSidebarItem *self);
void bm_sidebar_item_set_active(BmSidebarItem *self, gboolean active);
void bm_sidebar_item_set_collapsed(BmSidebarItem *self, gboolean collapsed);
GtkWidget *bm_sidebar_new(const char *label);
void bm_sidebar_append(BmSidebar *self, BmSidebarItem *item);
void bm_sidebar_set_header(BmSidebar *self, GtkWidget *header);
void bm_sidebar_set_footer(BmSidebar *self, GtkWidget *footer);
gboolean bm_sidebar_get_collapsed(BmSidebar *self);
void bm_sidebar_set_collapsed(BmSidebar *self, gboolean collapsed);
void bm_sidebar_set_theme_scope(BmSidebar *self, BmThemeScope *scope);
G_END_DECLS
