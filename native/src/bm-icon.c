#include <boldmerge/bm-icon.h>

static const char *const icon_names[] = {
  "bell", "check", "circle-alert", "circle-check", "circle-x", "diamond",
  "ellipsis", "folder", "folder-plus", "grid-2x2", "history", "house",
  "minus", "pencil", "plus", "rotate-cw", "save", "search", "settings",
  "star", "trash-2", "triangle-alert", "x",
};

gboolean
bm_icon_name_is_supported(const char *name)
{
  if (name == NULL) return FALSE;
  for (guint index = 0; index < G_N_ELEMENTS(icon_names); index++)
    if (g_str_equal(name, icon_names[index])) return TRUE;
  return FALSE;
}

GtkWidget *
bm_icon_new(const char *name, BmIconSize size)
{
  g_return_val_if_fail(bm_icon_name_is_supported(name), NULL);
  g_return_val_if_fail(size == BM_ICON_SIZE_SMALL ||
                       size == BM_ICON_SIZE_DEFAULT ||
                       size == BM_ICON_SIZE_LARGE, NULL);
  g_autofree char *resource = g_strdup_printf(
    "/com/boldmerge/native/icons/%s.svg", name);
  GtkWidget *image = gtk_image_new_from_resource(resource);
  gtk_image_set_pixel_size(GTK_IMAGE(image), size);
  gtk_accessible_update_state(
    GTK_ACCESSIBLE(image), GTK_ACCESSIBLE_STATE_HIDDEN, TRUE, -1);
  return image;
}

