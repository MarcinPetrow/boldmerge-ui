#!/usr/bin/env node
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { contrastRatio, generateAccentScale, gtkColor } from './lib/colors.mjs';
import { loadTokenSystem, resolveContext } from './lib/token-system.mjs';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const args = process.argv.slice(2);
const platformIndex = args.indexOf('--platform');
const platform = platformIndex >= 0 ? args.splice(platformIndex, 2)[1] : 'web';
const seed = args[0];
const requestedName = args[1] ?? 'custom';

function usage(message) {
  if (message) process.stderr.write(`${message}\n`);
  process.stderr.write("Usage: npm run tokens:custom -- '#RRGGBB' [name] [--platform web|gtk]\n");
  process.exitCode = 1;
}

function strategyFrom(records) {
  const get = (name) => records.find((record) => record.name === name && Object.keys(record.context).length === 0).value;
  return {
    lightness: Object.fromEntries(['50', '100', '200', '300', '400', '500', '600', '700', '800', '900', '950'].map((tone) => [tone, get(`accent.strategy.lightness.${tone}`)])),
    maxChroma: {
      light: get('accent.strategy.max-chroma.light'),
      mid: get('accent.strategy.max-chroma.mid'),
      dark: get('accent.strategy.max-chroma.dark'),
    },
  };
}

function safeName(name) {
  const result = name.toLowerCase().replace(/[^a-z0-9_-]+/g, '-').replace(/^-+|-+$/g, '');
  if (!result) throw new Error('Custom accent name must contain a letter or number');
  return result;
}

function validateCustom(scale, records) {
  const contexts = {
    light: { canvas: '#f5f7f9', text: scale['700'], solid: scale['700'], onSolid: '#ffffff', subtle: scale['100'], onSubtle: scale['800'], focus: scale['600'] },
    dark: { canvas: '#0d0f13', text: scale['300'], solid: scale['300'], onSolid: '#0d0f13', subtle: scale['900'], onSubtle: scale['200'], focus: scale['400'] },
  };
  const failures = [];
  for (const [theme, colors] of Object.entries(contexts)) {
    for (const [label, foreground, background, minimum] of [
      ['accent text', colors.text, colors.canvas, 4.5],
      ['solid content', colors.onSolid, colors.solid, 4.5],
      ['subtle content', colors.onSubtle, colors.subtle, 4.5],
      ['focus indicator', colors.focus, colors.canvas, 3],
    ]) {
      const ratio = contrastRatio(foreground, background);
      if (ratio < minimum) failures.push(`${theme} ${label}: ${ratio.toFixed(2)}:1 < ${minimum}:1`);
    }
  }
  if (failures.length) throw new Error(`Custom accent is unsafe for the fixed semantic mapping:\n${failures.join('\n')}`);
  return contexts;
}

if (!seed || !['web', 'gtk'].includes(platform)) {
  usage(!seed ? 'A seed color is required.' : `Unknown platform ${platform}.`);
} else {
  try {
    const records = await loadTokenSystem(root);
    const name = safeName(requestedName);
    const scale = generateAccentScale(seed, strategyFrom(records));
    const contexts = validateCustom(scale, records);
    if (platform === 'web') {
      const lines = [`/* Generated custom BoldMerge accent from ${seed.toLowerCase()}; review in every actual usage. */`, `[data-bm-accent="${name}"] {`, `  --bm-accent-seed: ${seed.toLowerCase()};`];
      for (const [tone, value] of Object.entries(scale)) lines.push(`  --bm-accent-tone-${tone}: ${value};`);
      lines.push('}', '');
      process.stdout.write(lines.join('\n'));
    } else {
      const lines = [`/* Generated custom BoldMerge GTK accent from ${seed.toLowerCase()}; explicit native propagation is required. */`];
      for (const [tone, value] of Object.entries(scale)) lines.push(`@define-color bm_accent_${name}_${tone} ${gtkColor(value)};`);
      for (const [theme, colors] of Object.entries(contexts)) for (const [role, value] of Object.entries(colors)) lines.push(`@define-color bm_${theme}_${name}_${role.replace(/[A-Z]/g, (letter) => `_${letter.toLowerCase()}`)} ${gtkColor(value)};`);
      lines.push('');
      process.stdout.write(lines.join('\n'));
    }
  } catch (error) {
    usage(error.message);
  }
}
