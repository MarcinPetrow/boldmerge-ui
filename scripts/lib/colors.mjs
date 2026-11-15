const clamp = (value, min = 0, max = 1) => Math.min(max, Math.max(min, value));

function parseHex(hex) {
  const match = /^#([0-9a-f]{6})([0-9a-f]{2})?$/i.exec(hex);
  if (!match) throw new Error(`Expected #RRGGBB or #RRGGBBAA, received ${hex}`);
  const integer = Number.parseInt(match[1], 16);
  return {
    r: ((integer >> 16) & 255) / 255,
    g: ((integer >> 8) & 255) / 255,
    b: (integer & 255) / 255,
    a: match[2] ? Number.parseInt(match[2], 16) / 255 : 1,
  };
}

function linearize(channel) {
  return channel <= 0.04045 ? channel / 12.92 : ((channel + 0.055) / 1.055) ** 2.4;
}

function encode(channel) {
  return channel <= 0.0031308 ? 12.92 * channel : 1.055 * channel ** (1 / 2.4) - 0.055;
}

function srgbToOklab({ r, g, b }) {
  const lr = linearize(r);
  const lg = linearize(g);
  const lb = linearize(b);
  const l = 0.4122214708 * lr + 0.5363325363 * lg + 0.0514459929 * lb;
  const m = 0.2119034982 * lr + 0.6806995451 * lg + 0.1073969566 * lb;
  const s = 0.0883024619 * lr + 0.2817188376 * lg + 0.6299787005 * lb;
  const lRoot = Math.cbrt(l);
  const mRoot = Math.cbrt(m);
  const sRoot = Math.cbrt(s);
  return {
    l: 0.2104542553 * lRoot + 0.793617785 * mRoot - 0.0040720468 * sRoot,
    a: 1.9779984951 * lRoot - 2.428592205 * mRoot + 0.4505937099 * sRoot,
    b: 0.0259040371 * lRoot + 0.7827717662 * mRoot - 0.808675766 * sRoot,
  };
}

function oklabToSrgb({ l, a, b }) {
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

function srgbToOklch(rgb) {
  const lab = srgbToOklab(rgb);
  const chroma = Math.hypot(lab.a, lab.b);
  let hue = (Math.atan2(lab.b, lab.a) * 180) / Math.PI;
  if (hue < 0) hue += 360;
  return { l: lab.l, c: chroma, h: chroma < 0.0001 ? 0 : hue };
}

function oklchToSrgb({ l, c, h }) {
  const radians = (h * Math.PI) / 180;
  return oklabToSrgb({ l, a: c * Math.cos(radians), b: c * Math.sin(radians) });
}

function inGamut({ r, g, b }) {
  const epsilon = 1e-7;
  return [r, g, b].every((channel) => channel >= -epsilon && channel <= 1 + epsilon);
}

function toHex({ r, g, b }) {
  return `#${[r, g, b]
    .map((channel) => Math.round(clamp(channel) * 255).toString(16).padStart(2, '0'))
    .join('')}`;
}

function gamutMappedHex(lightness, chroma, hue) {
  let low = 0;
  let high = Math.max(0, chroma);
  let rgb = oklchToSrgb({ l: lightness, c: high, h: hue });
  if (inGamut(rgb)) return toHex(rgb);
  for (let iteration = 0; iteration < 24; iteration += 1) {
    const middle = (low + high) / 2;
    const candidate = oklchToSrgb({ l: lightness, c: middle, h: hue });
    if (inGamut(candidate)) {
      low = middle;
      rgb = candidate;
    } else {
      high = middle;
    }
  }
  return toHex(rgb);
}

export function generateAccentScale(seed, strategy) {
  const source = srgbToOklch(parseHex(seed));
  const tones = {};
  for (const [tone, lightness] of Object.entries(strategy.lightness)) {
    const numericTone = Number(tone);
    const cap = numericTone <= 200 ? strategy.maxChroma.light : numericTone >= 800 ? strategy.maxChroma.dark : strategy.maxChroma.mid;
    const chroma = Math.min(source.c, cap);
    tones[tone] = gamutMappedHex(lightness, chroma, source.h);
  }
  return tones;
}

function relativeLuminance(hex) {
  const { r, g, b } = parseHex(hex);
  return 0.2126 * linearize(r) + 0.7152 * linearize(g) + 0.0722 * linearize(b);
}

export function contrastRatio(foreground, background) {
  const first = relativeLuminance(foreground);
  const second = relativeLuminance(background);
  return (Math.max(first, second) + 0.05) / (Math.min(first, second) + 0.05);
}

export function gtkColor(hex) {
  const color = parseHex(hex);
  if (color.a === 1) return hex.toLowerCase();
  return `rgba(${Math.round(color.r * 255)}, ${Math.round(color.g * 255)}, ${Math.round(color.b * 255)}, ${Number(color.a.toFixed(3))})`;
}

export function assertHexColor(value) {
  parseHex(value);
}
