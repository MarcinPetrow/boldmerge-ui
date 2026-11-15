#include <boldmerge/boldmerge.h>

static void
test_scope_inheritance(void)
{
  g_autoptr(BmThemeScope) parent = bm_theme_scope_new(NULL);
  g_autoptr(BmThemeScope) child = bm_theme_scope_new(parent);
  bm_theme_scope_set_theme(parent, BM_THEME_DARK);
  bm_theme_scope_set_density(parent, BM_DENSITY_COMPACT);
  bm_theme_scope_set_accent(parent, BM_ACCENT_VIOLET);
  g_assert_cmpint(bm_theme_scope_get_effective_theme(child), ==, BM_THEME_DARK);
  g_assert_cmpint(bm_theme_scope_get_effective_density(child), ==, BM_DENSITY_COMPACT);
  g_assert_cmpint(bm_theme_scope_get_effective_accent(child), ==, BM_ACCENT_VIOLET);
  bm_theme_scope_set_density(child, BM_DENSITY_BALANCED);
  g_assert_cmpint(bm_theme_scope_get_effective_density(child), ==, BM_DENSITY_BALANCED);
}

static void
test_custom_accent(void)
{
  g_autoptr(BmThemeScope) scope = bm_theme_scope_new(NULL);
  GdkRGBA seed;
  g_assert_true(gdk_rgba_parse(&seed, "#c026d3"));
  g_autoptr(GError) error = NULL;
  g_assert_true(bm_theme_scope_set_custom_accent(scope, &seed, &error));
  g_assert_no_error(error);
  g_assert_cmpint(bm_theme_scope_get_effective_accent(scope), ==, BM_ACCENT_CUSTOM);
  g_assert_nonnull(bm_theme_scope_get_custom_accent(scope));

  seed.alpha = 0.5;
  g_assert_false(bm_theme_scope_set_custom_accent(scope, &seed, &error));
  g_assert_error(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT);
}

int
main(int argc, char **argv)
{
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/boldmerge/scope/inheritance", test_scope_inheritance);
  g_test_add_func("/boldmerge/accent/custom", test_custom_accent);
  return g_test_run();
}
