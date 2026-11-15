#pragma once

#include <boldmerge/boldmerge.h>

typedef struct {
  char *light_text;
  char *light_solid;
  char *light_on_solid;
  char *light_subtle;
  char *light_on_subtle;
  char *light_focus;
  char *dark_text;
  char *dark_solid;
  char *dark_on_solid;
  char *dark_subtle;
  char *dark_on_subtle;
  char *dark_focus;
} BmAccentPalette;

gboolean bm_accent_palette_generate(const GdkRGBA *seed,
                                    BmAccentPalette *palette,
                                    GError **error);
void bm_accent_palette_clear(BmAccentPalette *palette);

void bm_theme_scope_apply_context_to_widget(BmThemeScope *scope,
                                            GtkWidget *widget,
                                            char **applied_custom_class);
void bm_theme_scope_clear_context_from_widget(GtkWidget *widget,
                                              char **applied_custom_class);

