#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_DROPDOWN_ITEM (bm_dropdown_item_get_type())
G_DECLARE_FINAL_TYPE(BmDropdownItem, bm_dropdown_item, BM, DROPDOWN_ITEM, GtkButton)

#define BM_TYPE_DROPDOWN_GROUP (bm_dropdown_group_get_type())
G_DECLARE_FINAL_TYPE(BmDropdownGroup, bm_dropdown_group, BM, DROPDOWN_GROUP, GtkBox)

#define BM_TYPE_DROPDOWN (bm_dropdown_get_type())
G_DECLARE_FINAL_TYPE(BmDropdown, bm_dropdown, BM, DROPDOWN, GtkWidget)

GtkWidget *bm_dropdown_item_new(const char *text, const char *value);
const char *bm_dropdown_item_get_text(BmDropdownItem *self);
const char *bm_dropdown_item_get_value(BmDropdownItem *self);
gboolean bm_dropdown_item_get_disabled(BmDropdownItem *self);
void bm_dropdown_item_set_disabled(BmDropdownItem *self, gboolean disabled);
gboolean bm_dropdown_item_get_destructive(BmDropdownItem *self);
void bm_dropdown_item_set_destructive(BmDropdownItem *self, gboolean destructive);
void bm_dropdown_item_set_prefix(BmDropdownItem *self, GtkWidget *prefix);
void bm_dropdown_item_set_suffix(BmDropdownItem *self, GtkWidget *suffix);

GtkWidget *bm_dropdown_group_new(const char *label);
void bm_dropdown_group_append(BmDropdownGroup *self, BmDropdownItem *item);
gboolean bm_dropdown_group_get_destructive(BmDropdownGroup *self);
void bm_dropdown_group_set_destructive(BmDropdownGroup *self, gboolean destructive);

GtkWidget *bm_dropdown_new(const char *label);
GtkMenuButton *bm_dropdown_get_trigger(BmDropdown *self);
void bm_dropdown_append_item(BmDropdown *self, BmDropdownItem *item);
void bm_dropdown_append_group(BmDropdown *self, BmDropdownGroup *group);
gboolean bm_dropdown_get_open(BmDropdown *self);
void bm_dropdown_set_open(BmDropdown *self, gboolean open);
gboolean bm_dropdown_get_disabled(BmDropdown *self);
void bm_dropdown_set_disabled(BmDropdown *self, gboolean disabled);
void bm_dropdown_set_theme_scope(BmDropdown *self, BmThemeScope *scope);

G_END_DECLS
