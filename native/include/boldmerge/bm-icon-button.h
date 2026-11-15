#pragma once

#include <boldmerge/bm-button.h>

G_BEGIN_DECLS

#define BM_TYPE_ICON_BUTTON (bm_icon_button_get_type())
G_DECLARE_FINAL_TYPE(BmIconButton, bm_icon_button, BM, ICON_BUTTON, GtkButton)

GtkWidget *bm_icon_button_new(const char *label, GtkWidget *icon);
const char *bm_icon_button_get_label(BmIconButton *self);
void bm_icon_button_set_label(BmIconButton *self, const char *label);
GtkWidget *bm_icon_button_get_icon(BmIconButton *self);
void bm_icon_button_set_icon(BmIconButton *self, GtkWidget *icon);
BmButtonVariant bm_icon_button_get_variant(BmIconButton *self);
void bm_icon_button_set_variant(BmIconButton *self, BmButtonVariant variant);
BmButtonTreatment bm_icon_button_get_treatment(BmIconButton *self);
void bm_icon_button_set_treatment(BmIconButton *self, BmButtonTreatment treatment);
gboolean bm_icon_button_get_loading(BmIconButton *self);
void bm_icon_button_set_loading(BmIconButton *self, gboolean loading);
BmThemeScope *bm_icon_button_get_theme_scope(BmIconButton *self);
void bm_icon_button_set_theme_scope(BmIconButton *self, BmThemeScope *scope);

G_END_DECLS
