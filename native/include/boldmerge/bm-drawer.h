#pragma once

#include <boldmerge/bm-dialog.h>

G_BEGIN_DECLS

typedef enum {
  BM_DRAWER_MODE_OVERLAY,
  BM_DRAWER_MODE_PUSH
} BmDrawerMode;

typedef enum {
  BM_DRAWER_POSITION_START,
  BM_DRAWER_POSITION_END
} BmDrawerPosition;

#define BM_TYPE_DRAWER_MODE (bm_drawer_mode_get_type())
GType bm_drawer_mode_get_type(void) G_GNUC_CONST;
#define BM_TYPE_DRAWER_POSITION (bm_drawer_position_get_type())
GType bm_drawer_position_get_type(void) G_GNUC_CONST;

#define BM_TYPE_DRAWER (bm_drawer_get_type())
G_DECLARE_FINAL_TYPE(BmDrawer, bm_drawer, BM, DRAWER, GtkWidget)

GtkWidget *bm_drawer_new(const char *label);
void bm_drawer_set_content(BmDrawer *self, GtkWidget *content);
void bm_drawer_set_header(BmDrawer *self, GtkWidget *header);
void bm_drawer_set_child(BmDrawer *self, GtkWidget *child);
void bm_drawer_set_footer(BmDrawer *self, GtkWidget *footer);
void bm_drawer_set_actions(BmDrawer *self, GtkWidget *actions);
gboolean bm_drawer_get_open(BmDrawer *self);
void bm_drawer_open(BmDrawer *self, GtkWidget *trigger,
                    GtkWidget *initial_focus);
gboolean bm_drawer_close(BmDrawer *self, BmCloseReason reason,
                         const char *return_value);
BmDrawerMode bm_drawer_get_mode(BmDrawer *self);
void bm_drawer_set_mode(BmDrawer *self, BmDrawerMode mode);
BmDrawerPosition bm_drawer_get_position(BmDrawer *self);
void bm_drawer_set_position(BmDrawer *self, BmDrawerPosition position);
gboolean bm_drawer_get_mobile(BmDrawer *self);
void bm_drawer_set_mobile(BmDrawer *self, gboolean mobile);
gboolean bm_drawer_get_dismissible(BmDrawer *self);
void bm_drawer_set_dismissible(BmDrawer *self, gboolean dismissible);
void bm_drawer_set_theme_scope(BmDrawer *self, BmThemeScope *scope);

G_END_DECLS
