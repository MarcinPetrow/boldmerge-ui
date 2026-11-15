#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum {
  BM_STATUS_TONE_NEUTRAL,
  BM_STATUS_TONE_ACCENT,
  BM_STATUS_TONE_SUCCESS,
  BM_STATUS_TONE_WARNING,
  BM_STATUS_TONE_ERROR,
} BmStatusTone;

#define BM_TYPE_STATUS_TONE (bm_status_tone_get_type())
GType bm_status_tone_get_type(void) G_GNUC_CONST;

#define BM_TYPE_STATUS (bm_status_get_type())
G_DECLARE_FINAL_TYPE(BmStatus, bm_status, BM, STATUS, GtkBox)

GtkWidget *bm_status_new(const char *text, BmStatusTone tone);
const char *bm_status_get_text(BmStatus *self);
void bm_status_set_text(BmStatus *self, const char *text);
BmStatusTone bm_status_get_tone(BmStatus *self);
void bm_status_set_tone(BmStatus *self, BmStatusTone tone);
BmThemeScope *bm_status_get_theme_scope(BmStatus *self);
void bm_status_set_theme_scope(BmStatus *self, BmThemeScope *scope);

G_END_DECLS
