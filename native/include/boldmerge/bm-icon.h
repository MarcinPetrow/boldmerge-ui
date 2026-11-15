#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
  BM_ICON_SIZE_SMALL = 16,
  BM_ICON_SIZE_DEFAULT = 20,
  BM_ICON_SIZE_LARGE = 24,
} BmIconSize;

gboolean bm_icon_name_is_supported(const char *name);

/**
 * bm_icon_new:
 * @name: a supported kebab-case icon name
 * @size: the requested canonical icon size
 *
 * Returns: (transfer full) (nullable): a new decorative #GtkImage, or %NULL
 *   when preconditions are not met
 */
GtkWidget *bm_icon_new(const char *name, BmIconSize size);

G_END_DECLS

