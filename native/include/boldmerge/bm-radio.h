#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_RADIO (bm_radio_get_type())
G_DECLARE_FINAL_TYPE(BmRadio, bm_radio, BM, RADIO, GtkCheckButton)

#define BM_TYPE_RADIO_GROUP (bm_radio_group_get_type())
G_DECLARE_FINAL_TYPE(BmRadioGroup, bm_radio_group, BM, RADIO_GROUP, GtkBox)

GtkWidget *bm_radio_new(const char *text, const char *value);
const char *bm_radio_get_text(BmRadio *self);
void bm_radio_set_text(BmRadio *self, const char *text);
const char *bm_radio_get_value(BmRadio *self);
void bm_radio_set_value(BmRadio *self, const char *value);
gboolean bm_radio_get_checked(BmRadio *self);
void bm_radio_set_checked(BmRadio *self, gboolean checked);
BmThemeScope *bm_radio_get_theme_scope(BmRadio *self);
void bm_radio_set_theme_scope(BmRadio *self, BmThemeScope *scope);

GtkWidget *bm_radio_group_new(const char *label);
void bm_radio_group_append(BmRadioGroup *self, BmRadio *radio);
const char *bm_radio_group_get_label(BmRadioGroup *self);
void bm_radio_group_set_label(BmRadioGroup *self, const char *label);
const char *bm_radio_group_get_value(BmRadioGroup *self);
void bm_radio_group_set_value(BmRadioGroup *self, const char *value);
gboolean bm_radio_group_get_required(BmRadioGroup *self);
void bm_radio_group_set_required(BmRadioGroup *self, gboolean required);
gboolean bm_radio_group_get_valid(BmRadioGroup *self);
gboolean bm_radio_group_validate(BmRadioGroup *self);
BmThemeScope *bm_radio_group_get_theme_scope(BmRadioGroup *self);
void bm_radio_group_set_theme_scope(BmRadioGroup *self, BmThemeScope *scope);

G_END_DECLS
