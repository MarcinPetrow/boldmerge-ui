#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_TABS (bm_tabs_get_type())
G_DECLARE_FINAL_TYPE(BmTabs, bm_tabs, BM, TABS, GtkBox)
GtkWidget *bm_tabs_new(const char *label);
void bm_tabs_append(BmTabs *self, const char *value, const char *label, GtkWidget *panel);
const char *bm_tabs_get_value(BmTabs *self);
void bm_tabs_set_value(BmTabs *self, const char *value);
GtkStack *bm_tabs_get_stack(BmTabs *self);
void bm_tabs_set_theme_scope(BmTabs *self, BmThemeScope *scope);
G_END_DECLS
