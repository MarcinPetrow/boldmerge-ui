import { readFile, readdir } from 'node:fs/promises';
import path from 'node:path';
import { assertHexColor, contrastRatio, generateAccentScale, gtkColor } from './colors.mjs';

export const THEMES = ['light', 'dark'];
export const DENSITIES = ['balanced', 'compact'];
export const ACCENTS = ['cyan', 'blue', 'violet', 'orange', 'green'];
const LAYERS = ['primitive', 'semantic', 'component'];
const TYPES = new Set(['color', 'dimension', 'duration', 'fontFamily', 'fontWeight', 'number', 'cubicBezier', 'shadow', 'string']);
const STATUSES = new Set(['approved', 'proposal']);
const CONTEXT_VALUES = { theme: THEMES, density: DENSITIES, accent: ACCENTS };
const REFERENCE = /^\{([a-z0-9][a-z0-9._-]*)\}$/;

function fail(file, message) {
  throw new Error(`${file}: ${message}`);
}

function validateTokenValue(file, name, token) {
  const reference = typeof token.value === 'string' && REFERENCE.test(token.value);
  if (reference) return;
  if (token.type === 'number' || token.type === 'fontWeight') {
    if (typeof token.value !== 'number' || !Number.isFinite(token.value)) fail(file, `${name} must have a finite numeric value`);
    return;
  }
  if (typeof token.value !== 'string' || token.value.length === 0) fail(file, `${name} must have a non-empty string value`);
  if (token.type === 'color') assertHexColor(token.value);
  if (token.type === 'dimension' && !/^-?(?:0|\d+(?:\.\d+)?)(?:px|rem|em|%)$/.test(token.value)) fail(file, `${name} has invalid dimension ${token.value}`);
  if (token.type === 'duration' && !/^\d+(?:\.\d+)?(?:ms|s)$/.test(token.value)) fail(file, `${name} has invalid duration ${token.value}`);
  if (token.type === 'cubicBezier' && !/^cubic-bezier\(-?\d*\.?\d+, -?\d*\.?\d+, -?\d*\.?\d+, -?\d*\.?\d+\)$/.test(token.value)) fail(file, `${name} has invalid cubic-bezier value`);
}

function validateDocument(file, document) {
  const allowedRoot = new Set(['$schema', 'layer', 'context', 'tokens']);
  if (!document || typeof document !== 'object' || Array.isArray(document)) fail(file, 'root must be an object');
  for (const key of Object.keys(document)) if (!allowedRoot.has(key)) fail(file, `unknown root field ${key}`);
  if (!LAYERS.includes(document.layer)) fail(file, `layer must be one of ${LAYERS.join(', ')}`);
  if (document.context !== undefined) {
    if (!document.context || typeof document.context !== 'object' || Array.isArray(document.context)) fail(file, 'context must be an object');
    for (const [key, value] of Object.entries(document.context)) {
      if (!CONTEXT_VALUES[key]) fail(file, `unknown context dimension ${key}`);
      if (!CONTEXT_VALUES[key].includes(value)) fail(file, `invalid ${key} context ${value}`);
    }
  }
  if (!document.tokens || typeof document.tokens !== 'object' || Array.isArray(document.tokens) || Object.keys(document.tokens).length === 0) fail(file, 'tokens must be a non-empty object');
  const allowedToken = new Set(['type', 'value', 'description', 'status']);
  for (const [name, token] of Object.entries(document.tokens)) {
    if (!/^[a-z0-9][a-z0-9._-]*$/.test(name)) fail(file, `invalid token name ${name}`);
    if (!token || typeof token !== 'object' || Array.isArray(token)) fail(file, `${name} must be an object`);
    for (const key of Object.keys(token)) if (!allowedToken.has(key)) fail(file, `${name} has unknown field ${key}`);
    for (const key of allowedToken) if (!(key in token)) fail(file, `${name} is missing ${key}`);
    if (!TYPES.has(token.type)) fail(file, `${name} has unsupported type ${token.type}`);
    if (!STATUSES.has(token.status)) fail(file, `${name} has unsupported status ${token.status}`);
    if (typeof token.description !== 'string' || token.description.length === 0) fail(file, `${name} needs a description`);
    validateTokenValue(file, name, token);
  }
}

export function validateTokenDocument(file, document) {
  validateDocument(file, document);
}

async function jsonFiles(directory) {
  const entries = await readdir(directory, { withFileTypes: true });
  const result = [];
  for (const entry of entries.sort((a, b) => a.name.localeCompare(b.name))) {
    const location = path.join(directory, entry.name);
    if (entry.isDirectory()) result.push(...(await jsonFiles(location)));
    else if (entry.isFile() && entry.name.endsWith('.json')) result.push(location);
  }
  return result;
}

function contextKey(context) {
  return Object.entries(context).sort(([a], [b]) => a.localeCompare(b)).map(([key, value]) => `${key}=${value}`).join(',');
}

function strategyFrom(records) {
  const value = (name) => records.find((record) => record.name === name && Object.keys(record.context).length === 0)?.value;
  const lightness = {};
  for (const tone of ['50', '100', '200', '300', '400', '500', '600', '700', '800', '900', '950']) lightness[tone] = value(`accent.strategy.lightness.${tone}`);
  return {
    lightness,
    maxChroma: {
      light: value('accent.strategy.max-chroma.light'),
      mid: value('accent.strategy.max-chroma.mid'),
      dark: value('accent.strategy.max-chroma.dark'),
    },
  };
}

function addGeneratedAccentTones(records) {
  const strategy = strategyFrom(records);
  for (const accent of ACCENTS) {
    const seed = records.find((record) => record.name === 'accent.seed' && record.context.accent === accent);
    if (!seed) throw new Error(`Missing accent seed for ${accent}`);
    for (const [tone, value] of Object.entries(generateAccentScale(seed.value, strategy))) {
      records.push({
        name: `accent.tone.${tone}`,
        type: 'color',
        value,
        description: `Deterministically generated ${accent} accent tone ${tone}.`,
        status: 'proposal',
        layer: 'primitive',
        context: { accent },
        file: '<generated from accent seed and strategy>',
        generated: true,
      });
    }
  }
}

export async function loadTokenSystem(root) {
  const tokenRoot = path.join(root, 'tokens');
  const files = (await jsonFiles(tokenRoot)).filter((file) => !file.includes(`${path.sep}schema${path.sep}`));
  const records = [];
  for (const file of files) {
    let document;
    try {
      document = JSON.parse(await readFile(file, 'utf8'));
    } catch (error) {
      fail(path.relative(root, file), `invalid JSON: ${error.message}`);
    }
    const relative = path.relative(root, file);
    validateDocument(relative, document);
    for (const [name, token] of Object.entries(document.tokens)) {
      records.push({ ...token, name, layer: document.layer, context: document.context ?? {}, file: relative, generated: false });
    }
  }
  addGeneratedAccentTones(records);
  validateRecords(records);
  return records;
}

function contextsCompatible(source, target) {
  return Object.entries(source).every(([key, value]) => target[key] === undefined || target[key] === value) &&
    Object.entries(target).every(([key, value]) => source[key] === undefined || source[key] === value);
}

function validateRecords(records) {
  const identities = new Set();
  for (const record of records) {
    const identity = `${record.name}|${contextKey(record.context)}`;
    if (identities.has(identity)) throw new Error(`Duplicate token ${record.name} in context ${contextKey(record.context) || 'base'}`);
    identities.add(identity);
    const match = typeof record.value === 'string' && REFERENCE.exec(record.value);
    if (!match) continue;
    const candidates = records.filter((candidate) => candidate.name === match[1] && contextsCompatible(record.context, candidate.context));
    if (candidates.length === 0) throw new Error(`${record.file}: ${record.name} references missing or incompatible token ${match[1]}`);
    for (const candidate of candidates) {
      if (LAYERS.indexOf(candidate.layer) > LAYERS.indexOf(record.layer)) throw new Error(`${record.file}: ${record.name} references later ${candidate.layer} layer token ${candidate.name}`);
      if (candidate.type !== record.type) throw new Error(`${record.file}: ${record.name} (${record.type}) references ${candidate.name} (${candidate.type})`);
    }
  }
  for (const theme of THEMES) for (const density of DENSITIES) for (const accent of ACCENTS) resolveContext(records, { theme, density, accent });
  validateContrast(records);
}

function chosenRecords(records, context) {
  const eligible = records.filter((record) => Object.entries(record.context).every(([key, value]) => context[key] === value));
  const chosen = new Map();
  for (const record of eligible) {
    const current = chosen.get(record.name);
    const specificity = Object.keys(record.context).length;
    if (!current || specificity > Object.keys(current.context).length) chosen.set(record.name, record);
    else if (specificity === Object.keys(current.context).length) throw new Error(`Ambiguous token ${record.name} for ${contextKey(context)}`);
  }
  return chosen;
}

export function resolveContext(records, context) {
  const chosen = chosenRecords(records, context);
  const resolved = new Map();
  const visiting = new Set();
  function resolve(name) {
    if (resolved.has(name)) return resolved.get(name);
    if (visiting.has(name)) throw new Error(`Reference cycle at ${name} for ${contextKey(context)}`);
    const record = chosen.get(name);
    if (!record) throw new Error(`Missing token ${name} for ${contextKey(context)}`);
    visiting.add(name);
    const match = typeof record.value === 'string' && REFERENCE.exec(record.value);
    const value = match ? resolve(match[1]).value : record.value;
    visiting.delete(name);
    const result = { ...record, value };
    resolved.set(name, result);
    return result;
  }
  for (const name of chosen.keys()) resolve(name);
  return resolved;
}

function requireContrast(label, foreground, background, minimum) {
  const ratio = contrastRatio(foreground, background);
  if (ratio + 1e-9 < minimum) throw new Error(`${label} contrast ${ratio.toFixed(2)}:1 is below ${minimum}:1 (${foreground} on ${background})`);
}

function validateContrast(records) {
  for (const theme of THEMES) {
    for (const accent of ACCENTS) {
      const tokens = resolveContext(records, { theme, density: 'balanced', accent });
      const get = (name) => tokens.get(name).value;
      requireContrast(`${theme}/${accent} accent text`, get('color.accent.text'), get('color.canvas'), 4.5);
      requireContrast(`${theme}/${accent} solid accent content`, get('color.accent.on-solid'), get('color.accent.solid'), 4.5);
      requireContrast(`${theme}/${accent} subtle accent content`, get('color.accent.on-subtle'), get('color.accent.subtle'), 4.5);
      requireContrast(`${theme}/${accent} focus indicator`, get('color.focus'), get('color.canvas'), 3);
    }
  }
}

function cssName(name) {
  return `--bm-${name.replaceAll('.', '-')}`;
}

function cssValue(record) {
  const match = typeof record.value === 'string' && REFERENCE.exec(record.value);
  return match ? `var(${cssName(match[1])})` : String(record.value);
}

function recordsForExactContext(records, context) {
  return records.filter((record) => contextKey(record.context) === contextKey(context)).sort((a, b) => a.name.localeCompare(b.name));
}

function cssBlock(selectors, records) {
  if (records.length === 0) return '';
  return `${selectors.join(',\n')} {\n${records.map((record) => `  ${cssName(record.name)}: ${cssValue(record)};`).join('\n')}\n}\n`;
}

export function emitBrowserCss(records) {
  const parts = [
    '/* Generated by scripts/build-tokens.mjs. Do not edit. */',
    '/* JSON is build-time input only. Attributes may be set on :root or any container; custom properties inherit locally. */',
    '',
    cssBlock([':root'], recordsForExactContext(records, {})).trimEnd(),
  ];
  for (const accent of ACCENTS) parts.push('', cssBlock(accent === 'cyan' ? [':root', `[data-bm-accent="${accent}"]`] : [`[data-bm-accent="${accent}"]`], recordsForExactContext(records, { accent })).trimEnd());
  for (const theme of THEMES) parts.push('', cssBlock(theme === 'light' ? [':root', `[data-bm-theme="${theme}"]`] : [`[data-bm-theme="${theme}"]`], recordsForExactContext(records, { theme })).trimEnd());
  for (const density of DENSITIES) parts.push('', cssBlock(density === 'balanced' ? [':root', `[data-bm-density="${density}"]`] : [`[data-bm-density="${density}"]`], recordsForExactContext(records, { density })).trimEnd());
  const darkRecords = recordsForExactContext(records, { theme: 'dark' });
  parts.push('', '@media (prefers-color-scheme: dark) {', cssBlock(['  :root:not([data-bm-theme])'], darkRecords).trimEnd(), '}');
  parts.push('', '@media (prefers-reduced-motion: reduce) {', '  :root:not([data-bm-motion="full"]),', '  [data-bm-motion="reduced"] {', '    --bm-motion-duration-press: 0ms;', '    --bm-motion-duration-interaction: 0ms;', '    --bm-motion-duration-overlay: 0ms;', '    --bm-skeleton-duration: 0ms;', '  }', '}');
  return `${parts.filter((part) => part !== '').join('\n')}\n`;
}

function gtkName(...parts) {
  return `bm_${parts.join('_').replaceAll('.', '_').replaceAll('-', '_')}`;
}

export function emitGtkCss(records) {
  const lines = [
    '/* Generated by scripts/build-tokens.mjs. Do not edit. */',
    '/* GTK CSS has no custom properties. Color names include context; native code must explicitly propagate theme, density, and accent state. */',
    '',
  ];
  const base = resolveContext(records, { theme: 'light', density: 'balanced', accent: 'cyan' });
  for (const record of [...base.values()].filter((item) => item.type === 'color' && Object.keys(item.context).length === 0).sort((a, b) => a.name.localeCompare(b.name))) lines.push(`@define-color ${gtkName('base', record.name)} ${gtkColor(record.value)};`);
  for (const accent of ACCENTS) {
    const resolved = resolveContext(records, { theme: 'light', density: 'balanced', accent });
    lines.push('', `/* accent=${accent} */`);
    for (const tone of ['50', '100', '200', '300', '400', '500', '600', '700', '800', '900', '950']) lines.push(`@define-color ${gtkName('accent', accent, tone)} ${gtkColor(resolved.get(`accent.tone.${tone}`).value)};`);
  }
  for (const theme of THEMES) {
    for (const accent of ACCENTS) {
      const resolved = resolveContext(records, { theme, density: 'balanced', accent });
      lines.push('', `/* theme=${theme}, accent=${accent} */`);
      for (const record of [...resolved.values()].filter((item) => item.layer === 'semantic' && item.type === 'color').sort((a, b) => a.name.localeCompare(b.name))) lines.push(`@define-color ${gtkName(theme, accent, record.name)} ${gtkColor(record.value)};`);
    }
  }
  for (const density of DENSITIES) {
    const resolved = resolveContext(records, { theme: 'light', density, accent: 'cyan' });
    lines.push('', `/* density=${density}; GTK cannot reference numeric tokens by name, so native widgets must apply these resolved values explicitly. */`);
    for (const record of [...resolved.values()].filter((item) => item.type !== 'color' && (item.layer === 'semantic' || item.layer === 'component')).sort((a, b) => a.name.localeCompare(b.name))) lines.push(`/* ${gtkName(density, record.name)}: ${record.value}; */`);
  }
  return `${lines.join('\n')}\n`;
}

function emitNativeAccentStrategy(records) {
  const exact = (name) => records.find((record) => record.name === name && Object.keys(record.context).length === 0)?.value;
  const themed = (theme, name) => records.find((record) => record.name === name && record.context.theme === theme)?.value;
  const tone = (theme, name) => {
    const match = /^\{accent\.tone\.(\d+)\}$/.exec(themed(theme, name));
    if (!match) throw new Error(`Native custom accent expects ${theme} ${name} to reference an accent tone`);
    return match[1];
  };
  const resolved = (theme, name) => resolveContext(records, { theme, density: 'balanced', accent: 'cyan' }).get(name).value;
  const lines = [
    '/* Generated by scripts/build-tokens.mjs. Do not edit. */',
    '#pragma once',
    '',
  ];
  for (const value of ['100', '200', '300', '400', '600', '700', '800', '900'])
    lines.push(`#define BM_ACCENT_LIGHTNESS_${value} ${exact(`accent.strategy.lightness.${value}`)}`);
  lines.push(
    `#define BM_ACCENT_MAX_CHROMA_LIGHT ${exact('accent.strategy.max-chroma.light')}`,
    `#define BM_ACCENT_MAX_CHROMA_MID ${exact('accent.strategy.max-chroma.mid')}`,
    `#define BM_ACCENT_MAX_CHROMA_DARK ${exact('accent.strategy.max-chroma.dark')}`,
    `#define BM_ACCENT_LIGHT_TEXT_TONE ${tone('light', 'color.accent.text')}`,
    `#define BM_ACCENT_LIGHT_SOLID_TONE ${tone('light', 'color.accent.solid')}`,
    `#define BM_ACCENT_LIGHT_SUBTLE_TONE ${tone('light', 'color.accent.subtle')}`,
    `#define BM_ACCENT_LIGHT_ON_SUBTLE_TONE ${tone('light', 'color.accent.on-subtle')}`,
    `#define BM_ACCENT_LIGHT_FOCUS_TONE ${tone('light', 'color.focus')}`,
    `#define BM_ACCENT_DARK_TEXT_TONE ${tone('dark', 'color.accent.text')}`,
    `#define BM_ACCENT_DARK_SOLID_TONE ${tone('dark', 'color.accent.solid')}`,
    `#define BM_ACCENT_DARK_SUBTLE_TONE ${tone('dark', 'color.accent.subtle')}`,
    `#define BM_ACCENT_DARK_ON_SUBTLE_TONE ${tone('dark', 'color.accent.on-subtle')}`,
    `#define BM_ACCENT_DARK_FOCUS_TONE ${tone('dark', 'color.focus')}`,
    `#define BM_ACCENT_LIGHT_CANVAS "${resolved('light', 'color.canvas')}"`,
    `#define BM_ACCENT_DARK_CANVAS "${resolved('dark', 'color.canvas')}"`,
    `#define BM_ACCENT_LIGHT_ON_SOLID "${resolved('light', 'color.accent.on-solid')}"`,
    `#define BM_ACCENT_DARK_ON_SOLID "${resolved('dark', 'color.accent.on-solid')}"`,
    '',
  );
  return `${lines.join('\n')}\n`;
}

function emitNativeComponentMetrics(records) {
  const value = (density, name) => resolveContext(records, { theme: 'light', density, accent: 'cyan' }).get(name).value;
  const balanced = (name) => value('balanced', name);
  const lines = [
    '/* Generated by scripts/build-tokens.mjs. Do not edit. */',
    '/* GTK cannot alias arbitrary numeric tokens; these selectors apply resolved canonical measurements. */',
    '',
    `.bm-button { border-radius: ${balanced('button.radius')}; border-width: ${balanced('button.border.width')}; font-weight: ${balanced('typography.weight.emphasis')}; }`,
    `.bm-button > box { border-spacing: ${balanced('button.content.gap')}; }`,
    `.bm-button:focus-visible { outline-width: ${balanced('size.2')}; outline-offset: ${balanced('size.2')}; }`,
    `.bm-button spinner { min-width: ${balanced('button.loading.size')}; min-height: ${balanced('button.loading.size')}; }`,
    `.bm-button:disabled { opacity: ${balanced('content.opacity.disabled')}; }`,
    `.bm-button.bm-density-balanced { min-height: ${value('balanced', 'button.height')}; padding-left: ${value('balanced', 'button.padding.inline')}; padding-right: ${value('balanced', 'button.padding.inline')}; }`,
    `.bm-button.bm-density-compact { min-height: ${value('compact', 'button.height')}; padding-left: ${value('compact', 'button.padding.inline')}; padding-right: ${value('compact', 'button.padding.inline')}; }`,
    `.bm-input { border-radius: ${balanced('input.radius')}; border-width: ${balanced('input.border.width')}; }`,
    `.bm-input .bm-input-field { border-spacing: ${balanced('input.content.gap')}; }`,
    `.bm-input label.bm-floating { font-size: ${balanced('font.size.12')}; }`,
    `.bm-input .bm-input-error { font-size: ${balanced('font.size.12')}; }`,
    `.bm-input:focus-within { outline-width: ${balanced('size.2')}; outline-offset: ${balanced('size.2')}; }`,
    `.bm-input-error-icon { min-width: ${balanced('input.error.icon.size')}; min-height: ${balanced('input.error.icon.size')}; }`,
    `.bm-input.bm-density-balanced .bm-input-field { min-height: ${value('balanced', 'input.floating.height')}; padding-left: ${value('balanced', 'input.padding.inline')}; padding-right: ${value('balanced', 'input.padding.inline')}; }`,
    `.bm-input.bm-density-compact .bm-input-field { min-height: ${value('compact', 'input.floating.height')}; padding-left: ${value('compact', 'input.padding.inline')}; padding-right: ${value('compact', 'input.padding.inline')}; }`,
    `.bm-input.bm-density-balanced .bm-input-error { padding: ${value('balanced', 'input.error.padding.block')} ${value('balanced', 'input.padding.inline')}; }`,
    `.bm-input.bm-density-compact .bm-input-error { padding: ${value('compact', 'input.error.padding.block')} ${value('compact', 'input.padding.inline')}; }`,
    `.bm-card { border-radius: ${balanced('card.radius')}; border-width: ${balanced('card.border.width')}; }`,
    `.bm-card.bm-density-balanced { padding: ${value('balanced', 'card.padding')}; }`,
    `.bm-card.bm-density-balanced > box { border-spacing: ${value('balanced', 'card.section.gap')}; }`,
    `.bm-card.bm-density-compact { padding: ${value('compact', 'card.padding')}; }`,
    `.bm-card.bm-density-compact > box { border-spacing: ${value('compact', 'card.section.gap')}; }`,
    '',
  ];
  return `${lines.join('\n')}\n`;
}

export function buildOutputs(records) {
  return {
    'dist/css/tokens.css': emitBrowserCss(records),
    'dist/gtk/tokens.css': emitGtkCss(records),
    'examples/output/browser-tokens.css': emitBrowserCss(records),
    'examples/output/gtk-tokens.css': emitGtkCss(records),
    'native/data/boldmerge-tokens.css': emitGtkCss(records),
    'native/src/bm-accent-strategy.h': emitNativeAccentStrategy(records),
    'native/data/component-metrics.css': emitNativeComponentMetrics(records),
  };
}

export function customAccentOutputs(seed, name = 'custom') {
  assertHexColor(seed);
  const safeName = name.toLowerCase().replace(/[^a-z0-9_-]+/g, '-').replace(/^-+|-+$/g, '');
  if (!safeName) throw new Error('Custom accent name must contain a letter or number');
  return { safeName, seed };
}
