#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_TAG (bm_tag_get_type())
G_DECLARE_FINAL_TYPE(BmTag, bm_tag, BM, TAG, GtkBox)

GtkWidget *bm_tag_new(const char *text);
const char *bm_tag_get_text(BmTag *self);
void bm_tag_set_text(BmTag *self, const char *text);
GtkWidget *bm_tag_get_prefix(BmTag *self);
void bm_tag_set_prefix(BmTag *self, GtkWidget *prefix);
GtkWidget *bm_tag_get_suffix(BmTag *self);
void bm_tag_set_suffix(BmTag *self, GtkWidget *suffix);
BmThemeScope *bm_tag_get_theme_scope(BmTag *self);
void bm_tag_set_theme_scope(BmTag *self, BmThemeScope *scope);

G_END_DECLS
