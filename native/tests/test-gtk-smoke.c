#include <boldmerge/boldmerge.h>

static void
test_provider_and_scope(void)
{
  GdkDisplay *display = gdk_display_get_default();
  g_assert_nonnull(display);
  g_autoptr(GError) error = NULL;
  g_assert_true(bm_init_for_display(display, &error));
  g_assert_no_error(error);
  g_assert_true(bm_init_for_display(display, &error));

  g_autoptr(BmThemeScope) scope = bm_theme_scope_new(NULL);
  bm_theme_scope_set_theme(scope, BM_THEME_DARK);
  bm_theme_scope_set_density(scope, BM_DENSITY_COMPACT);
  bm_theme_scope_set_accent(scope, BM_ACCENT_GREEN);
  g_autoptr(GtkWidget) box = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  bm_theme_scope_apply(scope, box);
  g_assert_true(gtk_widget_has_css_class(box, "bm-scope"));
  g_assert_true(gtk_widget_has_css_class(box, "bm-theme-dark"));
  g_assert_true(gtk_widget_has_css_class(box, "bm-density-compact"));
  g_assert_true(gtk_widget_has_css_class(box, "bm-accent-green"));
}

int
main(int argc, char **argv)
{
  g_test_init(&argc, &argv, NULL);
  if (!gtk_init_check()) {
    g_test_message("No GTK display is available; GTK smoke test skipped");
    return 77;
  }
  g_test_add_func("/boldmerge/gtk/provider-and-scope", test_provider_and_scope);
  return g_test_run();
}
