#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_SEGMENTED_CONTROL (bm_segmented_control_get_type())
G_DECLARE_FINAL_TYPE(BmSegmentedControl, bm_segmented_control, BM, SEGMENTED_CONTROL, GtkBox)
GtkWidget *bm_segmented_control_new(const char *label);
GtkToggleButton *bm_segmented_control_append(BmSegmentedControl *self, const char *text, const char *value);
const char *bm_segmented_control_get_value(BmSegmentedControl *self);
void bm_segmented_control_set_value(BmSegmentedControl *self, const char *value);
void bm_segmented_control_set_theme_scope(BmSegmentedControl *self, BmThemeScope *scope);
G_END_DECLS
