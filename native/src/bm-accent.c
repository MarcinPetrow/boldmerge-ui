#include "bm-private.h"
#include "bm-accent-strategy.h"

#include <math.h>
#include <stdio.h>

typedef struct { double r, g, b; } Rgb;
typedef struct { double l, a, b; } Oklab;
typedef struct { double l, c, h; } Oklch;

_Static_assert(BM_ACCENT_LIGHT_TEXT_TONE == 700 && BM_ACCENT_LIGHT_SOLID_TONE == 700 &&
               BM_ACCENT_LIGHT_SUBTLE_TONE == 100 && BM_ACCENT_LIGHT_ON_SUBTLE_TONE == 800 &&
               BM_ACCENT_LIGHT_FOCUS_TONE == 600 && BM_ACCENT_DARK_TEXT_TONE == 300 &&
               BM_ACCENT_DARK_SOLID_TONE == 300 && BM_ACCENT_DARK_SUBTLE_TONE == 900 &&
               BM_ACCENT_DARK_ON_SUBTLE_TONE == 200 && BM_ACCENT_DARK_FOCUS_TONE == 400,
               "Canonical accent role mapping changed; update the Native generator explicitly");

static double
linearize(double channel)
{
  return channel <= 0.04045 ? channel / 12.92 : pow((channel + 0.055) / 1.055, 2.4);
}

static double
encode(double channel)
{
  return channel <= 0.0031308 ? 12.92 * channel : 1.055 * pow(channel, 1.0 / 2.4) - 0.055;
}

static Oklch
rgb_to_oklch(Rgb rgb)
{
  double lr = linearize(rgb.r), lg = linearize(rgb.g), lb = linearize(rgb.b);
  double l = 0.4122214708 * lr + 0.5363325363 * lg + 0.0514459929 * lb;
  double m = 0.2119034982 * lr + 0.6806995451 * lg + 0.1073969566 * lb;
  double s = 0.0883024619 * lr + 0.2817188376 * lg + 0.6299787005 * lb;
  double l_root = cbrt(l), m_root = cbrt(m), s_root = cbrt(s);
  Oklab lab = {
    0.2104542553 * l_root + 0.793617785 * m_root - 0.0040720468 * s_root,
    1.9779984951 * l_root - 2.428592205 * m_root + 0.4505937099 * s_root,
    0.0259040371 * l_root + 0.7827717662 * m_root - 0.808675766 * s_root,
  };
  double chroma = hypot(lab.a, lab.b);
  double hue = chroma < 0.0001 ? 0.0 : atan2(lab.b, lab.a) * 180.0 / G_PI;
  if (hue < 0.0) hue += 360.0;
  return (Oklch) { lab.l, chroma, hue };
}

static Rgb
oklch_to_rgb(double lightness, double chroma, double hue)
{
  double radians = hue * G_PI / 180.0;
  double a = chroma * cos(radians), b = chroma * sin(radians);
  double l_root = lightness + 0.3963377774 * a + 0.2158037573 * b;
  double m_root = lightness - 0.1055613458 * a - 0.0638541728 * b;
  double s_root = lightness - 0.0894841775 * a - 1.291485548 * b;
  double ll = l_root * l_root * l_root;
  double mm = m_root * m_root * m_root;
  double ss = s_root * s_root * s_root;
  return (Rgb) {
    encode(4.0767416621 * ll - 3.3077115913 * mm + 0.2309699292 * ss),
    encode(-1.2684380046 * ll + 2.6097574011 * mm - 0.3413193965 * ss),
    encode(-0.0041960863 * ll - 0.7034186147 * mm + 1.707614701 * ss),
  };
}

static gboolean
in_gamut(Rgb rgb)
{
  const double epsilon = 1e-7;
  return rgb.r >= -epsilon && rgb.r <= 1.0 + epsilon &&
         rgb.g >= -epsilon && rgb.g <= 1.0 + epsilon &&
         rgb.b >= -epsilon && rgb.b <= 1.0 + epsilon;
}

static char *
mapped_hex(double lightness, double chroma, double hue)
{
  double low = 0.0, high = MAX(0.0, chroma);
  Rgb rgb = oklch_to_rgb(lightness, high, hue);
  if (!in_gamut(rgb)) {
    for (guint i = 0; i < 24; i++) {
      double middle = (low + high) / 2.0;
      Rgb candidate = oklch_to_rgb(lightness, middle, hue);
      if (in_gamut(candidate)) {
        low = middle;
        rgb = candidate;
      } else {
        high = middle;
      }
    }
  }
  return g_strdup_printf("#%02x%02x%02x",
                         (guint) round(CLAMP(rgb.r, 0.0, 1.0) * 255.0),
                         (guint) round(CLAMP(rgb.g, 0.0, 1.0) * 255.0),
                         (guint) round(CLAMP(rgb.b, 0.0, 1.0) * 255.0));
}

static double
luminance(const char *hex)
{
  guint value = 0;
  sscanf(hex + 1, "%x", &value);
  return 0.2126 * linearize(((value >> 16) & 255) / 255.0) +
         0.7152 * linearize(((value >> 8) & 255) / 255.0) +
         0.0722 * linearize((value & 255) / 255.0);
}

static double
contrast(const char *foreground, const char *background)
{
  double first = luminance(foreground), second = luminance(background);
  return (MAX(first, second) + 0.05) / (MIN(first, second) + 0.05);
}

gboolean
bm_accent_palette_generate(const GdkRGBA *seed,
                           BmAccentPalette *palette,
                           GError **error)
{
  g_return_val_if_fail(seed != NULL, FALSE);
  g_return_val_if_fail(palette != NULL, FALSE);

  Oklch source = rgb_to_oklch((Rgb) { seed->red, seed->green, seed->blue });
  char *tone100 = mapped_hex(BM_ACCENT_LIGHTNESS_100, MIN(source.c, BM_ACCENT_MAX_CHROMA_LIGHT), source.h);
  char *tone200 = mapped_hex(BM_ACCENT_LIGHTNESS_200, MIN(source.c, BM_ACCENT_MAX_CHROMA_LIGHT), source.h);
  char *tone300 = mapped_hex(BM_ACCENT_LIGHTNESS_300, MIN(source.c, BM_ACCENT_MAX_CHROMA_MID), source.h);
  char *tone400 = mapped_hex(BM_ACCENT_LIGHTNESS_400, MIN(source.c, BM_ACCENT_MAX_CHROMA_MID), source.h);
  char *tone600 = mapped_hex(BM_ACCENT_LIGHTNESS_600, MIN(source.c, BM_ACCENT_MAX_CHROMA_MID), source.h);
  char *tone700 = mapped_hex(BM_ACCENT_LIGHTNESS_700, MIN(source.c, BM_ACCENT_MAX_CHROMA_MID), source.h);
  char *tone800 = mapped_hex(BM_ACCENT_LIGHTNESS_800, MIN(source.c, BM_ACCENT_MAX_CHROMA_DARK), source.h);
  char *tone900 = mapped_hex(BM_ACCENT_LIGHTNESS_900, MIN(source.c, BM_ACCENT_MAX_CHROMA_DARK), source.h);
  const char *light_canvas = BM_ACCENT_LIGHT_CANVAS, *dark_canvas = BM_ACCENT_DARK_CANVAS;

  GString *failures = g_string_new(NULL);
#define CHECK(label, fg, bg, minimum) G_STMT_START { \
  double ratio = contrast((fg), (bg)); \
  if (ratio < (minimum)) \
    g_string_append_printf(failures, "%s %.2f:1 < %.1f:1; ", (label), ratio, (double) (minimum)); \
} G_STMT_END
  CHECK("light accent text", tone700, light_canvas, 4.5);
  CHECK("light solid content", BM_ACCENT_LIGHT_ON_SOLID, tone700, 4.5);
  CHECK("light subtle content", tone800, tone100, 4.5);
  CHECK("light focus", tone600, light_canvas, 3.0);
  CHECK("dark accent text", tone300, dark_canvas, 4.5);
  CHECK("dark solid content", BM_ACCENT_DARK_ON_SOLID, tone300, 4.5);
  CHECK("dark subtle content", tone200, tone900, 4.5);
  CHECK("dark focus", tone400, dark_canvas, 3.0);
#undef CHECK

  if (failures->len > 0) {
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                "Custom accent is unsafe for the canonical semantic mapping: %s",
                failures->str);
    g_string_free(failures, TRUE);
    g_free(tone100); g_free(tone200); g_free(tone300); g_free(tone400);
    g_free(tone600); g_free(tone700); g_free(tone800); g_free(tone900);
    return FALSE;
  }
  g_string_free(failures, TRUE);

  bm_accent_palette_clear(palette);
  palette->light_text = g_strdup(tone700);
  palette->light_solid = g_strdup(tone700);
  palette->light_on_solid = g_strdup(BM_ACCENT_LIGHT_ON_SOLID);
  palette->light_subtle = g_strdup(tone100);
  palette->light_on_subtle = g_strdup(tone800);
  palette->light_focus = g_strdup(tone600);
  palette->dark_text = g_strdup(tone300);
  palette->dark_solid = g_strdup(tone300);
  palette->dark_on_solid = g_strdup(BM_ACCENT_DARK_ON_SOLID);
  palette->dark_subtle = g_strdup(tone900);
  palette->dark_on_subtle = g_strdup(tone200);
  palette->dark_focus = g_strdup(tone400);
  g_free(tone100); g_free(tone200); g_free(tone300); g_free(tone400);
  g_free(tone600); g_free(tone700); g_free(tone800); g_free(tone900);
  return TRUE;
}

void
bm_accent_palette_clear(BmAccentPalette *palette)
{
  g_free(palette->light_text); g_free(palette->light_solid);
  g_free(palette->light_on_solid); g_free(palette->light_subtle);
  g_free(palette->light_on_subtle); g_free(palette->light_focus);
  g_free(palette->dark_text); g_free(palette->dark_solid);
  g_free(palette->dark_on_solid); g_free(palette->dark_subtle);
  g_free(palette->dark_on_subtle); g_free(palette->dark_focus);
  *palette = (BmAccentPalette) { 0 };
}
