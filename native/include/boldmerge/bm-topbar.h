#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_TOPBAR (bm_topbar_get_type())
G_DECLARE_FINAL_TYPE(BmTopbar, bm_topbar, BM, TOPBAR, GtkBox)
GtkWidget *bm_topbar_new(void);
void bm_topbar_set_start(BmTopbar *self, GtkWidget *widget);
void bm_topbar_set_child(BmTopbar *self, GtkWidget *widget);
void bm_topbar_set_search(BmTopbar *self, GtkWidget *widget);
void bm_topbar_set_notifications(BmTopbar *self, GtkWidget *widget);
void bm_topbar_set_account(BmTopbar *self, GtkWidget *widget);
void bm_topbar_set_theme_scope(BmTopbar *self, BmThemeScope *scope);
G_END_DECLS
