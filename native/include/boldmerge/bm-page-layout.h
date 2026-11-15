#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
typedef enum { BM_PAGE_LAYOUT_FLUID, BM_PAGE_LAYOUT_CONTAINED, BM_PAGE_LAYOUT_SPLIT } BmPageLayoutMode;
#define BM_TYPE_PAGE_LAYOUT_MODE (bm_page_layout_mode_get_type())
GType bm_page_layout_mode_get_type(void) G_GNUC_CONST;
#define BM_TYPE_PAGE_LAYOUT (bm_page_layout_get_type())
G_DECLARE_FINAL_TYPE(BmPageLayout, bm_page_layout, BM, PAGE_LAYOUT, GtkBox)
GtkWidget *bm_page_layout_new(void);
BmPageLayoutMode bm_page_layout_get_mode(BmPageLayout *self);
void bm_page_layout_set_mode(BmPageLayout *self, BmPageLayoutMode mode);
GtkWidget *bm_page_layout_get_header(BmPageLayout *self);
void bm_page_layout_set_header(BmPageLayout *self, GtkWidget *header);
GtkWidget *bm_page_layout_get_child(BmPageLayout *self);
void bm_page_layout_set_child(BmPageLayout *self, GtkWidget *child);
GtkWidget *bm_page_layout_get_aside(BmPageLayout *self);
void bm_page_layout_set_aside(BmPageLayout *self, GtkWidget *aside);
BmThemeScope *bm_page_layout_get_theme_scope(BmPageLayout *self);
void bm_page_layout_set_theme_scope(BmPageLayout *self, BmThemeScope *scope);
G_END_DECLS
