#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_TOOLTIP (bm_tooltip_get_type())
G_DECLARE_FINAL_TYPE(BmTooltip, bm_tooltip, BM, TOOLTIP, GtkWidget)

GtkWidget *bm_tooltip_new(GtkWidget *child, const char *content);
GtkWidget *bm_tooltip_get_child(BmTooltip *self);
void bm_tooltip_set_child(BmTooltip *self, GtkWidget *child);
const char *bm_tooltip_get_content(BmTooltip *self);
void bm_tooltip_set_content(BmTooltip *self, const char *content);
void bm_tooltip_set_theme_scope(BmTooltip *self, BmThemeScope *scope);

G_END_DECLS
