#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

typedef enum {
  BM_THEME_INHERIT,
  BM_THEME_LIGHT,
  BM_THEME_DARK,
} BmTheme;

typedef enum {
  BM_DENSITY_INHERIT,
  BM_DENSITY_BALANCED,
  BM_DENSITY_COMPACT,
} BmDensity;

typedef enum {
  BM_ACCENT_INHERIT,
  BM_ACCENT_CYAN,
  BM_ACCENT_BLUE,
  BM_ACCENT_VIOLET,
  BM_ACCENT_ORANGE,
  BM_ACCENT_GREEN,
  BM_ACCENT_CUSTOM,
} BmAccent;

#define BM_TYPE_THEME (bm_theme_get_type())
GType bm_theme_get_type(void) G_GNUC_CONST;
#define BM_TYPE_DENSITY (bm_density_get_type())
GType bm_density_get_type(void) G_GNUC_CONST;
#define BM_TYPE_ACCENT (bm_accent_get_type())
GType bm_accent_get_type(void) G_GNUC_CONST;

#define BM_TYPE_THEME_SCOPE (bm_theme_scope_get_type())
G_DECLARE_FINAL_TYPE(BmThemeScope, bm_theme_scope, BM, THEME_SCOPE, GObject)

BmThemeScope *bm_theme_scope_new(BmThemeScope *parent);

BmThemeScope *bm_theme_scope_get_parent(BmThemeScope *self);
void bm_theme_scope_set_parent(BmThemeScope *self, BmThemeScope *parent);

BmTheme bm_theme_scope_get_theme(BmThemeScope *self);
void bm_theme_scope_set_theme(BmThemeScope *self, BmTheme theme);
BmDensity bm_theme_scope_get_density(BmThemeScope *self);
void bm_theme_scope_set_density(BmThemeScope *self, BmDensity density);
BmAccent bm_theme_scope_get_accent(BmThemeScope *self);
void bm_theme_scope_set_accent(BmThemeScope *self, BmAccent accent);

BmTheme bm_theme_scope_get_effective_theme(BmThemeScope *self);
BmDensity bm_theme_scope_get_effective_density(BmThemeScope *self);
BmAccent bm_theme_scope_get_effective_accent(BmThemeScope *self);

gboolean bm_theme_scope_set_custom_accent(BmThemeScope *self,
                                           const GdkRGBA *seed,
                                           GError **error);
const GdkRGBA *bm_theme_scope_get_custom_accent(BmThemeScope *self);

void bm_theme_scope_apply(BmThemeScope *self, GtkWidget *widget);
GtkWidget *bm_theme_scope_ref_widget(BmThemeScope *self);

gboolean bm_init_for_display(GdkDisplay *display, GError **error);

G_END_DECLS
