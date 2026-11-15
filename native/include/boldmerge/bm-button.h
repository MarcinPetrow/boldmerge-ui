#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum {
  BM_BUTTON_VARIANT_PRIMARY,
  BM_BUTTON_VARIANT_SECONDARY,
  BM_BUTTON_VARIANT_OUTLINE,
  BM_BUTTON_VARIANT_GHOST,
} BmButtonVariant;

typedef enum {
  BM_BUTTON_TREATMENT_DEFAULT,
  BM_BUTTON_TREATMENT_DESTRUCTIVE,
} BmButtonTreatment;

#define BM_TYPE_BUTTON_VARIANT (bm_button_variant_get_type())
GType bm_button_variant_get_type(void) G_GNUC_CONST;
#define BM_TYPE_BUTTON_TREATMENT (bm_button_treatment_get_type())
GType bm_button_treatment_get_type(void) G_GNUC_CONST;

#define BM_TYPE_BUTTON (bm_button_get_type())
G_DECLARE_FINAL_TYPE(BmButton, bm_button, BM, BUTTON, GtkButton)

GtkWidget *bm_button_new(const char *text);

const char *bm_button_get_text(BmButton *self);
void bm_button_set_text(BmButton *self, const char *text);
BmButtonVariant bm_button_get_variant(BmButton *self);
void bm_button_set_variant(BmButton *self, BmButtonVariant variant);
BmButtonTreatment bm_button_get_treatment(BmButton *self);
void bm_button_set_treatment(BmButton *self, BmButtonTreatment treatment);
gboolean bm_button_get_loading(BmButton *self);
void bm_button_set_loading(BmButton *self, gboolean loading);

GtkWidget *bm_button_get_content(BmButton *self);
void bm_button_set_content(BmButton *self, GtkWidget *content);
BmThemeScope *bm_button_get_theme_scope(BmButton *self);
void bm_button_set_theme_scope(BmButton *self, BmThemeScope *scope);

G_END_DECLS
