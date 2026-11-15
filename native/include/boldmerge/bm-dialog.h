#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum {
  BM_CLOSE_REASON_API,
  BM_CLOSE_REASON_ACTION,
  BM_CLOSE_REASON_ESCAPE,
  BM_CLOSE_REASON_BACKDROP
} BmCloseReason;

#define BM_TYPE_CLOSE_REASON (bm_close_reason_get_type())
GType bm_close_reason_get_type(void) G_GNUC_CONST;

#define BM_TYPE_DIALOG (bm_dialog_get_type())
G_DECLARE_FINAL_TYPE(BmDialog, bm_dialog, BM, DIALOG, GtkWindow)

GtkWidget *bm_dialog_new(const char *label);
void bm_dialog_set_header(BmDialog *self, GtkWidget *header);
void bm_dialog_set_child(BmDialog *self, GtkWidget *child);
void bm_dialog_set_footer(BmDialog *self, GtkWidget *footer);
void bm_dialog_set_actions(BmDialog *self, GtkWidget *actions);
GtkButton *bm_dialog_add_action(BmDialog *self, const char *label,
                                const char *return_value,
                                gboolean suggested, gboolean destructive);
gboolean bm_dialog_get_dismissible(BmDialog *self);
void bm_dialog_set_dismissible(BmDialog *self, gboolean dismissible);
gboolean bm_dialog_get_open(BmDialog *self);
void bm_dialog_present(BmDialog *self, GtkWindow *parent, GtkWidget *trigger,
                       GtkWidget *initial_focus);
gboolean bm_dialog_close(BmDialog *self, BmCloseReason reason,
                         const char *return_value);
void bm_dialog_set_theme_scope(BmDialog *self, BmThemeScope *scope);

G_END_DECLS
