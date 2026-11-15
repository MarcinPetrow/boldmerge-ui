#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_CARD (bm_card_get_type())
G_DECLARE_FINAL_TYPE(BmCard, bm_card, BM, CARD, GtkWidget)

GtkWidget *bm_card_new(void);

GtkWidget *bm_card_get_child(BmCard *self);
void bm_card_set_child(BmCard *self, GtkWidget *child);
GtkWidget *bm_card_get_header(BmCard *self);
void bm_card_set_header(BmCard *self, GtkWidget *header);
GtkWidget *bm_card_get_actions(BmCard *self);
void bm_card_set_actions(BmCard *self, GtkWidget *actions);
GtkWidget *bm_card_get_footer(BmCard *self);
void bm_card_set_footer(BmCard *self, GtkWidget *footer);
const char *bm_card_get_accessible_label(BmCard *self);
void bm_card_set_accessible_label(BmCard *self, const char *label);
BmThemeScope *bm_card_get_theme_scope(BmCard *self);
void bm_card_set_theme_scope(BmCard *self, BmThemeScope *scope);

G_END_DECLS
