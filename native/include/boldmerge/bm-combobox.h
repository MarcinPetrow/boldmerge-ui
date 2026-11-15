#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_COMBOBOX (bm_combobox_get_type())
G_DECLARE_FINAL_TYPE(BmCombobox, bm_combobox, BM, COMBOBOX, GtkBox)
GtkWidget *bm_combobox_new(const char *label);
void bm_combobox_append(BmCombobox *self, const char *text, const char *value);
void bm_combobox_remove_all(BmCombobox *self);
const char *bm_combobox_get_label(BmCombobox *self);
const char *bm_combobox_get_value(BmCombobox *self);
void bm_combobox_set_value(BmCombobox *self, const char *value);
gboolean bm_combobox_get_required(BmCombobox *self);
void bm_combobox_set_required(BmCombobox *self, gboolean required);
const char *bm_combobox_get_error(BmCombobox *self);
void bm_combobox_set_error(BmCombobox *self, const char *error);
gboolean bm_combobox_get_valid(BmCombobox *self);
gboolean bm_combobox_validate(BmCombobox *self);
GtkDropDown *bm_combobox_get_control(BmCombobox *self);
void bm_combobox_set_theme_scope(BmCombobox *self, BmThemeScope *scope);
G_END_DECLS
