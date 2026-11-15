#pragma once
#include <boldmerge/bm-sidebar.h>
#include <boldmerge/bm-topbar.h>
G_BEGIN_DECLS
#define BM_TYPE_APP_SHELL (bm_app_shell_get_type())
G_DECLARE_FINAL_TYPE(BmAppShell, bm_app_shell, BM, APP_SHELL, GtkWidget)
GtkWidget *bm_app_shell_new(void);
void bm_app_shell_set_sidebar(BmAppShell *self, BmSidebar *sidebar);
void bm_app_shell_set_topbar(BmAppShell *self, BmTopbar *topbar);
void bm_app_shell_set_child(BmAppShell *self, GtkWidget *child);
gboolean bm_app_shell_get_sidebar_collapsed(BmAppShell *self);
void bm_app_shell_set_sidebar_collapsed(BmAppShell *self, gboolean collapsed);
gboolean bm_app_shell_get_sidebar_open(BmAppShell *self);
void bm_app_shell_open_sidebar(BmAppShell *self, GtkWidget *trigger);
void bm_app_shell_close_sidebar(BmAppShell *self);
void bm_app_shell_toggle_sidebar(BmAppShell *self, GtkWidget *trigger);
gboolean bm_app_shell_get_mobile(BmAppShell *self);
void bm_app_shell_set_mobile(BmAppShell *self, gboolean mobile);
void bm_app_shell_set_theme_scope(BmAppShell *self, BmThemeScope *scope);
G_END_DECLS
