#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

typedef enum { BM_AVATAR_PERSON, BM_AVATAR_ORGANIZATION } BmAvatarKind;
typedef enum { BM_AVATAR_SMALL, BM_AVATAR_MEDIUM, BM_AVATAR_LARGE, BM_AVATAR_XLARGE } BmAvatarSize;
typedef enum { BM_AVATAR_AVAILABILITY_NONE, BM_AVATAR_AVAILABILITY_ONLINE, BM_AVATAR_AVAILABILITY_AWAY, BM_AVATAR_AVAILABILITY_BUSY, BM_AVATAR_AVAILABILITY_OFFLINE } BmAvatarAvailability;

#define BM_TYPE_AVATAR_KIND (bm_avatar_kind_get_type())
GType bm_avatar_kind_get_type(void) G_GNUC_CONST;
#define BM_TYPE_AVATAR_SIZE (bm_avatar_size_get_type())
GType bm_avatar_size_get_type(void) G_GNUC_CONST;
#define BM_TYPE_AVATAR_AVAILABILITY (bm_avatar_availability_get_type())
GType bm_avatar_availability_get_type(void) G_GNUC_CONST;

#define BM_TYPE_AVATAR (bm_avatar_get_type())
G_DECLARE_FINAL_TYPE(BmAvatar, bm_avatar, BM, AVATAR, GtkBox)
GtkWidget *bm_avatar_new(const char *name);
const char *bm_avatar_get_name(BmAvatar *self);
void bm_avatar_set_name(BmAvatar *self, const char *name);
const char *bm_avatar_get_fallback(BmAvatar *self);
void bm_avatar_set_label(BmAvatar *self, const char *label);
void bm_avatar_set_src(BmAvatar *self, const char *src);
void bm_avatar_set_kind(BmAvatar *self, BmAvatarKind kind);
void bm_avatar_set_size(BmAvatar *self, BmAvatarSize size);
BmAvatarSize bm_avatar_get_size(BmAvatar *self);
void bm_avatar_set_availability(BmAvatar *self, BmAvatarAvailability availability);
void bm_avatar_set_availability_label(BmAvatar *self, const char *label);
void bm_avatar_set_theme_scope(BmAvatar *self, BmThemeScope *scope);

#define BM_TYPE_AVATAR_GROUP (bm_avatar_group_get_type())
G_DECLARE_FINAL_TYPE(BmAvatarGroup, bm_avatar_group, BM, AVATAR_GROUP, GtkBox)
GtkWidget *bm_avatar_group_new(const char *label);
void bm_avatar_group_append(BmAvatarGroup *self, BmAvatar *avatar);
void bm_avatar_group_set_size(BmAvatarGroup *self, BmAvatarSize size);
void bm_avatar_group_set_overflow(BmAvatarGroup *self, guint count,
                                  const char *label);
void bm_avatar_group_set_theme_scope(BmAvatarGroup *self, BmThemeScope *scope);

G_END_DECLS
