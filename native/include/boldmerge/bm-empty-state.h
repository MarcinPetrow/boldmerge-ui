#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_EMPTY_STATE (bm_empty_state_get_type())
G_DECLARE_FINAL_TYPE(BmEmptyState, bm_empty_state, BM, EMPTY_STATE, GtkBox)

GtkWidget *bm_empty_state_new(const char *heading);
const char *bm_empty_state_get_heading(BmEmptyState *self);
void bm_empty_state_set_heading(BmEmptyState *self, const char *heading);
void bm_empty_state_set_description(BmEmptyState *self, const char *description);
void bm_empty_state_set_label(BmEmptyState *self, const char *label);
void bm_empty_state_set_heading_level(BmEmptyState *self, guint level);
guint bm_empty_state_get_heading_level(BmEmptyState *self);
void bm_empty_state_set_icon(BmEmptyState *self, GtkWidget *icon);
void bm_empty_state_set_description_widget(BmEmptyState *self, GtkWidget *widget);
void bm_empty_state_set_action(BmEmptyState *self, GtkWidget *action);
void bm_empty_state_set_theme_scope(BmEmptyState *self, BmThemeScope *scope);

G_END_DECLS
