#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_CHECKBOX (bm_checkbox_get_type())
G_DECLARE_FINAL_TYPE(BmCheckbox, bm_checkbox, BM, CHECKBOX, GtkCheckButton)

GtkWidget *bm_checkbox_new(const char *text);

const char *bm_checkbox_get_text(BmCheckbox *self);
void bm_checkbox_set_text(BmCheckbox *self, const char *text);
gboolean bm_checkbox_get_checked(BmCheckbox *self);
void bm_checkbox_set_checked(BmCheckbox *self, gboolean checked);
gboolean bm_checkbox_get_indeterminate(BmCheckbox *self);
void bm_checkbox_set_indeterminate(BmCheckbox *self, gboolean indeterminate);
gboolean bm_checkbox_get_required(BmCheckbox *self);
void bm_checkbox_set_required(BmCheckbox *self, gboolean required);
gboolean bm_checkbox_get_valid(BmCheckbox *self);
gboolean bm_checkbox_validate(BmCheckbox *self);
BmThemeScope *bm_checkbox_get_theme_scope(BmCheckbox *self);
void bm_checkbox_set_theme_scope(BmCheckbox *self, BmThemeScope *scope);

G_END_DECLS
