#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_COMMAND (bm_command_get_type())
G_DECLARE_FINAL_TYPE(BmCommand, bm_command, BM, COMMAND, GObject)
#define BM_TYPE_COMMAND_PALETTE (bm_command_palette_get_type())
G_DECLARE_FINAL_TYPE(BmCommandPalette, bm_command_palette, BM, COMMAND_PALETTE, GtkWindow)
BmCommand *bm_command_new(const char *label, const char *value,
                          const char *keywords, const char *group);
const char *bm_command_get_label(BmCommand *self);
const char *bm_command_get_value(BmCommand *self);
void bm_command_set_disabled(BmCommand *self, gboolean disabled);
gboolean bm_command_get_disabled(BmCommand *self);
GtkWidget *bm_command_palette_new(const char *label);
void bm_command_palette_append(BmCommandPalette *self, BmCommand *command);
gboolean bm_command_palette_get_open(BmCommandPalette *self);
void bm_command_palette_present(BmCommandPalette *self, GtkWindow *parent,
                                GtkWidget *trigger);
void bm_command_palette_close(BmCommandPalette *self);
void bm_command_palette_set_query(BmCommandPalette *self, const char *query);
const char *bm_command_palette_get_query(BmCommandPalette *self);
guint bm_command_palette_get_visible_count(BmCommandPalette *self);
gboolean bm_command_palette_activate_first(BmCommandPalette *self);
void bm_command_palette_set_theme_scope(BmCommandPalette *self,
                                        BmThemeScope *scope);
G_END_DECLS
