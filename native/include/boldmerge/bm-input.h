#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_INPUT (bm_input_get_type())
G_DECLARE_FINAL_TYPE(BmInput, bm_input, BM, INPUT, GtkWidget)

GtkWidget *bm_input_new(const char *label);

const char *bm_input_get_text(BmInput *self);
void bm_input_set_text(BmInput *self, const char *text);
const char *bm_input_get_label(BmInput *self);
void bm_input_set_label(BmInput *self, const char *label);
const char *bm_input_get_placeholder_text(BmInput *self);
void bm_input_set_placeholder_text(BmInput *self, const char *placeholder_text);
gboolean bm_input_get_required(BmInput *self);
void bm_input_set_required(BmInput *self, gboolean required);
gboolean bm_input_get_editable(BmInput *self);
void bm_input_set_editable(BmInput *self, gboolean editable);
const char *bm_input_get_error(BmInput *self);
void bm_input_set_error(BmInput *self, const char *error);
const char *bm_input_get_required_message(BmInput *self);
void bm_input_set_required_message(BmInput *self, const char *message);
gboolean bm_input_get_valid(BmInput *self);
gboolean bm_input_validate(BmInput *self);

GtkEntry *bm_input_get_entry(BmInput *self);
GtkWidget *bm_input_get_leading_widget(BmInput *self);
void bm_input_set_leading_widget(BmInput *self, GtkWidget *widget);
GtkWidget *bm_input_get_trailing_widget(BmInput *self);
void bm_input_set_trailing_widget(BmInput *self, GtkWidget *widget);
BmThemeScope *bm_input_get_theme_scope(BmInput *self);
void bm_input_set_theme_scope(BmInput *self, BmThemeScope *scope);

G_END_DECLS
