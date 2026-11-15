#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
#define BM_TYPE_SECTION (bm_section_get_type())
G_DECLARE_FINAL_TYPE(BmSection, bm_section, BM, SECTION, GtkBox)
GtkWidget *bm_section_new(const char *label);
const char *bm_section_get_label(BmSection *self);
void bm_section_set_label(BmSection *self, const char *label);
void bm_section_set_header(BmSection *self, GtkWidget *widget);
void bm_section_set_actions(BmSection *self, GtkWidget *widget);
void bm_section_set_child(BmSection *self, GtkWidget *widget);
void bm_section_set_footer(BmSection *self, GtkWidget *widget);
gboolean bm_section_get_separator(BmSection *self);
void bm_section_set_separator(BmSection *self, gboolean separator);
void bm_section_set_theme_scope(BmSection *self, BmThemeScope *scope);
G_END_DECLS
