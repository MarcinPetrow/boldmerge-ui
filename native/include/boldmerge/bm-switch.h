#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_SWITCH (bm_switch_get_type())
G_DECLARE_FINAL_TYPE(BmSwitch, bm_switch, BM, SWITCH, GtkBox)

GtkWidget *bm_switch_new(const char *text);

const char *bm_switch_get_text(BmSwitch *self);
void bm_switch_set_text(BmSwitch *self, const char *text);
gboolean bm_switch_get_active(BmSwitch *self);
void bm_switch_set_active(BmSwitch *self, gboolean active);
GtkSwitch *bm_switch_get_control(BmSwitch *self);
BmThemeScope *bm_switch_get_theme_scope(BmSwitch *self);
void bm_switch_set_theme_scope(BmSwitch *self, BmThemeScope *scope);

G_END_DECLS
