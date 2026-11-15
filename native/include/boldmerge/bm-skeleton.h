#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
typedef enum { BM_SKELETON_TEXT, BM_SKELETON_RECTANGLE, BM_SKELETON_CIRCLE } BmSkeletonShape;
#define BM_TYPE_SKELETON_SHAPE (bm_skeleton_shape_get_type())
GType bm_skeleton_shape_get_type(void) G_GNUC_CONST;
#define BM_TYPE_SKELETON (bm_skeleton_get_type())
G_DECLARE_FINAL_TYPE(BmSkeleton,bm_skeleton,BM,SKELETON,GtkBox)
GtkWidget *bm_skeleton_new(BmSkeletonShape shape, const char *label);
void bm_skeleton_set_shape(BmSkeleton *self, BmSkeletonShape shape);
BmSkeletonShape bm_skeleton_get_shape(BmSkeleton *self);
void bm_skeleton_set_size(BmSkeleton *self, int width, int height);
void bm_skeleton_set_label(BmSkeleton *self, const char *label);
void bm_skeleton_set_theme_scope(BmSkeleton *self, BmThemeScope *scope);
G_END_DECLS
