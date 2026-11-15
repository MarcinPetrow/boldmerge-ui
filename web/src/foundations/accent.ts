import { DESIGN_ACCENT_STRATEGY, DESIGN_CONTRAST_REFERENCES } from '../generated/token-metadata.js';

export const ACCENT_PRESETS = ['cyan', 'blue', 'violet', 'orange', 'green'] as const;
export const ACCENT_TONES = ['50', '100', '200', '300', '400', '500', '600', '700', '800', '900', '950'] as const;

export type AccentPreset = (typeof ACCENT_PRESETS)[number];
export type AccentTone = (typeof ACCENT_TONES)[number];
export type AccentPalette = Readonly<Record<AccentTone, string>>;

interface Rgb {
  r: number;
  g: number;
  b: number;
}

function parseHex(value: string): Rgb {
  const match = /^#([0-9a-f]{6})$/i.exec(value);
  if (!match?.[1]) throw new TypeError(`Custom accent must be an opaque #RRGGBB color; received ${value}`);
  const integer = Number.parseInt(match[1], 16);
  return { r: ((integer >> 16) & 255) / 255, g: ((integer >> 8) & 255) / 255, b: (integer & 255) / 255 };
}

const linearize = (channel: number): number => channel <= 0.04045 ? channel / 12.92 : ((channel + 0.055) / 1.055) ** 2.4;
const encode = (channel: number): number => channel <= 0.0031308 ? 12.92 * channel : 1.055 * channel ** (1 / 2.4) - 0.055;

function toOklch(rgb: Rgb): { l: number; c: number; h: number } {
  const red = linearize(rgb.r);
  const green = linearize(rgb.g);
  const blue = linearize(rgb.b);
  const lRoot = Math.cbrt(0.4122214708 * red + 0.5363325363 * green + 0.0514459929 * blue);
  const mRoot = Math.cbrt(0.2119034982 * red + 0.6806995451 * green + 0.1073969566 * blue);
  const sRoot = Math.cbrt(0.0883024619 * red + 0.2817188376 * green + 0.6299787005 * blue);
  const l = 0.2104542553 * lRoot + 0.793617785 * mRoot - 0.0040720468 * sRoot;
  const a = 1.9779984951 * lRoot - 2.428592205 * mRoot + 0.4505937099 * sRoot;
  const b = 0.0259040371 * lRoot + 0.7827717662 * mRoot - 0.808675766 * sRoot;
  const c = Math.hypot(a, b);
  let h = (Math.atan2(b, a) * 180) / Math.PI;
  if (h < 0) h += 360;
  return { l, c, h: c < 0.0001 ? 0 : h };
}

function fromOklch(l: number, c: number, h: number): Rgb {
  const radians = (h * Math.PI) / 180;
  const a = c * Math.cos(radians);
  const b = c * Math.sin(radians);
  const lRoot = l + 0.3963377774 * a + 0.2158037573 * b;
  const mRoot = l - 0.1055613458 * a - 0.0638541728 * b;
  const sRoot = l - 0.0894841775 * a - 1.291485548 * b;
  const ll = lRoot ** 3;
  const mm = mRoot ** 3;
  const ss = sRoot ** 3;
  return {
    r: encode(4.0767416621 * ll - 3.3077115913 * mm + 0.2309699292 * ss),
    g: encode(-1.2684380046 * ll + 2.6097574011 * mm - 0.3413193965 * ss),
    b: encode(-0.0041960863 * ll - 0.7034186147 * mm + 1.707614701 * ss),
  };
}

const inGamut = (rgb: Rgb): boolean => [rgb.r, rgb.g, rgb.b].every((channel) => channel >= -1e-7 && channel <= 1 + 1e-7);
const toHex = (rgb: Rgb): string => `#${[rgb.r, rgb.g, rgb.b].map((channel) => Math.round(Math.min(1, Math.max(0, channel)) * 255).toString(16).padStart(2, '0')).join('')}`;

function gamutMappedHex(lightness: number, chroma: number, hue: number): string {
  let low = 0;
  let high = chroma;
  let rgb = fromOklch(lightness, high, hue);
  if (inGamut(rgb)) return toHex(rgb);
  for (let iteration = 0; iteration < 24; iteration += 1) {
    const middle = (low + high) / 2;
    const candidate = fromOklch(lightness, middle, hue);
    if (inGamut(candidate)) {
      low = middle;
      rgb = candidate;
    } else {
      high = middle;
    }
  }
  return toHex(rgb);
}

function luminance(color: string): number {
  const rgb = parseHex(color);
  return 0.2126 * linearize(rgb.r) + 0.7152 * linearize(rgb.g) + 0.0722 * linearize(rgb.b);
}

export function contrastRatio(foreground: string, background: string): number {
  const first = luminance(foreground);
  const second = luminance(background);
  return (Math.max(first, second) + 0.05) / (Math.min(first, second) + 0.05);
}

function validateSemanticPairs(palette: AccentPalette): void {
  const pairs: ReadonlyArray<readonly [string, string, string, number]> = [
    ['Light accent text', palette['700'], DESIGN_CONTRAST_REFERENCES.lightCanvas, 4.5],
    ['Light solid content', DESIGN_CONTRAST_REFERENCES.lightOnSolid, palette['700'], 4.5],
    ['Light subtle content', palette['800'], palette['100'], 4.5],
    ['Light focus indicator', palette['600'], DESIGN_CONTRAST_REFERENCES.lightCanvas, 3],
    ['Dark accent text', palette['300'], DESIGN_CONTRAST_REFERENCES.darkCanvas, 4.5],
    ['Dark solid content', DESIGN_CONTRAST_REFERENCES.darkCanvas, palette['300'], 4.5],
    ['Dark subtle content', palette['200'], palette['900'], 4.5],
    ['Dark focus indicator', palette['400'], DESIGN_CONTRAST_REFERENCES.darkCanvas, 3],
  ];
  const failures = pairs.flatMap(([label, foreground, background, minimum]) => {
    const ratio = contrastRatio(foreground, background);
    return ratio < minimum ? [`${label} is ${ratio.toFixed(2)}:1; requires ${minimum}:1`] : [];
  });
  if (failures.length) throw new RangeError(`Custom accent fails the canonical semantic mapping:\n${failures.join('\n')}`);
}

export function createAccentPalette(seed: string): AccentPalette {
  const source = toOklch(parseHex(seed));
  const entries = ACCENT_TONES.map((tone): readonly [AccentTone, string] => {
    const numericTone = Number(tone);
    const maximumChroma = numericTone <= 200 ? DESIGN_ACCENT_STRATEGY.maxChroma.light : numericTone >= 800 ? DESIGN_ACCENT_STRATEGY.maxChroma.dark : DESIGN_ACCENT_STRATEGY.maxChroma.mid;
    return [tone, gamutMappedHex(DESIGN_ACCENT_STRATEGY.lightness[tone], Math.min(source.c, maximumChroma), source.h)];
  });
  const palette = Object.freeze(Object.fromEntries(entries)) as AccentPalette;
  validateSemanticPairs(palette);
  return palette;
}

export function isAccentPreset(value: string): value is AccentPreset {
  return (ACCENT_PRESETS as readonly string[]).includes(value);
}
