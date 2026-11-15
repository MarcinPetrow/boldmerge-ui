#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum {
  BM_TOAST_TONE_NEUTRAL,
  BM_TOAST_TONE_SUCCESS,
  BM_TOAST_TONE_WARNING,
  BM_TOAST_TONE_ERROR
} BmToastTone;

typedef enum {
  BM_TOAST_CLOSE_REASON_API,
  BM_TOAST_CLOSE_REASON_DISMISS,
  BM_TOAST_CLOSE_REASON_TIMEOUT
} BmToastCloseReason;

#define BM_TYPE_TOAST_TONE (bm_toast_tone_get_type())
GType bm_toast_tone_get_type(void) G_GNUC_CONST;
#define BM_TYPE_TOAST_CLOSE_REASON (bm_toast_close_reason_get_type())
GType bm_toast_close_reason_get_type(void) G_GNUC_CONST;

#define BM_TYPE_TOAST (bm_toast_get_type())
G_DECLARE_FINAL_TYPE(BmToast, bm_toast, BM, TOAST, GObject)

#define BM_TYPE_TOAST_REGION (bm_toast_region_get_type())
G_DECLARE_FINAL_TYPE(BmToastRegion, bm_toast_region, BM, TOAST_REGION, GtkWidget)

BmToast *bm_toast_new(const char *title, const char *message,
                      BmToastTone tone, guint duration_seconds);
const char *bm_toast_get_id(BmToast *self);
const char *bm_toast_get_title(BmToast *self);
const char *bm_toast_get_message(BmToast *self);
BmToastTone bm_toast_get_tone(BmToast *self);
guint bm_toast_get_duration(BmToast *self);
void bm_toast_dismiss(BmToast *self, BmToastCloseReason reason);

GtkWidget *bm_toast_region_new(const char *label);
void bm_toast_region_set_child(BmToastRegion *self, GtkWidget *child);
void bm_toast_region_add(BmToastRegion *self, BmToast *toast);
BmToast *bm_toast_region_push(BmToastRegion *self, const char *title,
                              const char *message, BmToastTone tone,
                              guint duration_seconds);
guint bm_toast_region_get_count(BmToastRegion *self);
void bm_toast_region_clear(BmToastRegion *self);
void bm_toast_region_set_theme_scope(BmToastRegion *self, BmThemeScope *scope);

G_END_DECLS
