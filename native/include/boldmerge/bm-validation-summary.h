#pragma once

#include <boldmerge/bm-input.h>
#include <boldmerge/bm-select.h>
#include <boldmerge/bm-checkbox.h>
#include <boldmerge/bm-radio.h>
#include <boldmerge/bm-combobox.h>

G_BEGIN_DECLS

typedef enum {
  BM_VALIDATION_FOCUS_FIRST,
  BM_VALIDATION_FOCUS_SUMMARY,
  BM_VALIDATION_FOCUS_NONE
} BmValidationFocus;

#define BM_TYPE_VALIDATION_FOCUS (bm_validation_focus_get_type())
GType bm_validation_focus_get_type(void) G_GNUC_CONST;

#define BM_TYPE_VALIDATION_SUMMARY (bm_validation_summary_get_type())
G_DECLARE_FINAL_TYPE(BmValidationSummary, bm_validation_summary, BM,
                     VALIDATION_SUMMARY, GtkWidget)

GtkWidget *bm_validation_summary_new(const char *heading);
void bm_validation_summary_add(BmValidationSummary *self, GtkWidget *control,
                               const char *label, const char *message);
void bm_validation_summary_remove_all(BmValidationSummary *self);
void bm_validation_summary_refresh(BmValidationSummary *self);
gboolean bm_validation_summary_validate(BmValidationSummary *self,
                                        BmValidationFocus focus);
guint bm_validation_summary_get_error_count(BmValidationSummary *self);
void bm_validation_summary_focus_first_invalid(BmValidationSummary *self);
void bm_validation_summary_set_theme_scope(BmValidationSummary *self,
                                           BmThemeScope *scope);

G_END_DECLS
