#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_SELECT (bm_select_get_type())
G_DECLARE_FINAL_TYPE(BmSelect, bm_select, BM, SELECT, GtkBox)

GtkWidget *bm_select_new(const char *label);
void bm_select_append(BmSelect *self, const char *text, const char *value);
void bm_select_remove_all(BmSelect *self);
const char *bm_select_get_label(BmSelect *self);
void bm_select_set_label(BmSelect *self, const char *label);
const char *bm_select_get_value(BmSelect *self);
void bm_select_set_value(BmSelect *self, const char *value);
gboolean bm_select_get_required(BmSelect *self);
void bm_select_set_required(BmSelect *self, gboolean required);
const char *bm_select_get_error(BmSelect *self);
void bm_select_set_error(BmSelect *self, const char *error);
gboolean bm_select_get_valid(BmSelect *self);
gboolean bm_select_validate(BmSelect *self);
GtkDropDown *bm_select_get_control(BmSelect *self);
BmThemeScope *bm_select_get_theme_scope(BmSelect *self);
void bm_select_set_theme_scope(BmSelect *self, BmThemeScope *scope);

G_END_DECLS
