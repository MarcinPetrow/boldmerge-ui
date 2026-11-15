#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_PROGRESS (bm_progress_get_type())
G_DECLARE_FINAL_TYPE(BmProgress,bm_progress,BM,PROGRESS,GtkBox)
GtkWidget *bm_progress_new(const char *label);
void bm_progress_set_range(BmProgress *self, double min, double max);
void bm_progress_set_value(BmProgress *self, double value);
double bm_progress_get_value(BmProgress *self);
void bm_progress_set_segments(BmProgress *self, guint segments);
guint bm_progress_get_segments(BmProgress *self);
guint bm_progress_get_filled_segments(BmProgress *self);
void bm_progress_set_value_label(BmProgress *self, const char *label);
void bm_progress_set_theme_scope(BmProgress *self, BmThemeScope *scope);
G_END_DECLS
