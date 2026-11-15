#include "bm-private.h"

typedef struct {
  GtkCssProvider *tokens;
  GtkCssProvider *foundation;
  GtkCssProvider *components;
  GtkCssProvider *component_metrics;
} DisplayProviders;

typedef struct { GError *error; } ParseCapture;

static void
display_providers_free(DisplayProviders *providers)
{
  g_clear_object(&providers->tokens);
  g_clear_object(&providers->foundation);
  g_clear_object(&providers->components);
  g_clear_object(&providers->component_metrics);
  g_free(providers);
}

static void
on_parsing_error(GtkCssProvider *provider,
                 GtkCssSection *section,
                 const GError *error,
                 gpointer user_data)
{
  ParseCapture *capture = user_data;
  (void) provider;
  (void) section;
  if (capture->error == NULL)
    capture->error = g_error_copy(error);
}

static GtkCssProvider *
provider_from_resource(const char *path, GError **error)
{
  g_autoptr(GBytes) bytes = g_resources_lookup_data(path, G_RESOURCE_LOOKUP_FLAGS_NONE, error);
  if (bytes == NULL)
    return NULL;

  gsize size = 0;
  const char *data = g_bytes_get_data(bytes, &size);
  GtkCssProvider *provider = gtk_css_provider_new();
  ParseCapture capture = { 0 };
  gulong handler = g_signal_connect(provider, "parsing-error", G_CALLBACK(on_parsing_error), &capture);
  gtk_css_provider_load_from_data(provider, data, (gssize) size);
  g_signal_handler_disconnect(provider, handler);
  if (capture.error != NULL) {
    g_propagate_prefixed_error(error, capture.error, "Invalid BoldMerge CSS resource %s: ", path);
    g_object_unref(provider);
    return NULL;
  }
  return provider;
}

gboolean
bm_init_for_display(GdkDisplay *display, GError **error)
{
  g_return_val_if_fail(GDK_IS_DISPLAY(display), FALSE);
  static GQuark providers_quark = 0;
  if (G_UNLIKELY(providers_quark == 0))
    providers_quark = g_quark_from_static_string("boldmerge-display-providers");
  if (g_object_get_qdata(G_OBJECT(display), providers_quark) != NULL)
    return TRUE;

  DisplayProviders *providers = g_new0(DisplayProviders, 1);
  providers->tokens = provider_from_resource("/com/boldmerge/native/boldmerge-tokens.css", error);
  if (providers->tokens == NULL) {
    display_providers_free(providers);
    return FALSE;
  }
  providers->foundation = provider_from_resource("/com/boldmerge/native/foundation.css", error);
  if (providers->foundation == NULL) {
    display_providers_free(providers);
    return FALSE;
  }
  providers->components = provider_from_resource("/com/boldmerge/native/components.css", error);
  if (providers->components == NULL) {
    display_providers_free(providers);
    return FALSE;
  }
  providers->component_metrics = provider_from_resource("/com/boldmerge/native/component-metrics.css", error);
  if (providers->component_metrics == NULL) {
    display_providers_free(providers);
    return FALSE;
  }

  gtk_style_context_add_provider_for_display(display,
                                              GTK_STYLE_PROVIDER(providers->tokens),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_style_context_add_provider_for_display(display,
                                              GTK_STYLE_PROVIDER(providers->foundation),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
  gtk_style_context_add_provider_for_display(display,
                                              GTK_STYLE_PROVIDER(providers->components),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
  gtk_style_context_add_provider_for_display(display,
                                              GTK_STYLE_PROVIDER(providers->component_metrics),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
  g_object_set_qdata_full(G_OBJECT(display), providers_quark, providers,
                          (GDestroyNotify) display_providers_free);
  return TRUE;
}
