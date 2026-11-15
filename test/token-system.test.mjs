import assert from 'node:assert/strict';
import { readFile } from 'node:fs/promises';
import path from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';
import { contrastRatio, generateAccentScale } from '../scripts/lib/colors.mjs';
import { ACCENTS, buildOutputs, loadTokenSystem, resolveContext, validateTokenDocument } from '../scripts/lib/token-system.mjs';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const records = await loadTokenSystem(root);

test('loads all three canonical layers', () => {
  assert.deepEqual([...new Set(records.map((record) => record.layer))].sort(), ['component', 'primitive', 'semantic']);
});

test('structural validation rejects unknown fields and invalid units', () => {
  assert.throws(() => validateTokenDocument('bad.json', {
    layer: 'primitive',
    tokens: {
      'bad.dimension': { type: 'dimension', value: '12pt', description: 'Invalid unit.', status: 'proposal', surprise: true },
    },
  }), /unknown field surprise/);
  assert.throws(() => validateTokenDocument('bad.json', {
    layer: 'primitive',
    tokens: {
      'bad.dimension': { type: 'dimension', value: '12pt', description: 'Invalid unit.', status: 'proposal' },
    },
  }), /invalid dimension/);
});

test('resolver rejects reference cycles', () => {
  const cycle = [
    { name: 'cycle.a', type: 'dimension', value: '{cycle.b}', description: 'A.', status: 'proposal', layer: 'primitive', context: {}, file: 'cycle.json' },
    { name: 'cycle.b', type: 'dimension', value: '{cycle.a}', description: 'B.', status: 'proposal', layer: 'primitive', context: {}, file: 'cycle.json' },
  ];
  assert.throws(() => resolveContext(cycle, { theme: 'light', density: 'balanced', accent: 'cyan' }), /Reference cycle/);
  const missing = [
    { name: 'missing.a', type: 'dimension', value: '{missing.b}', description: 'A.', status: 'proposal', layer: 'primitive', context: {}, file: 'missing.json' },
  ];
  assert.throws(() => resolveContext(missing, { theme: 'light', density: 'balanced', accent: 'cyan' }), /Missing token missing.b/);
});

test('preserves approved balanced and compact component measurements', () => {
  const balanced = resolveContext(records, { theme: 'light', density: 'balanced', accent: 'cyan' });
  const compact = resolveContext(records, { theme: 'light', density: 'compact', accent: 'cyan' });
  const expected = {
    'button.height': ['40px', '32px'],
    'table.row.height': ['48px', '36px'],
    'card.padding': ['20px', '12px'],
    'section.gap': ['32px', '24px'],
    'layout.gap': ['24px', '16px'],
    'sidebar.item.height': ['40px', '32px'],
    'input.floating.height': ['48px', '40px'],
    'tab.height': ['44px', '36px'],
    'menu.item.height': ['40px', '32px'],
  };
  for (const [name, [balancedValue, compactValue]] of Object.entries(expected)) {
    assert.equal(balanced.get(name).value, balancedValue, `${name} balanced`);
    assert.equal(compact.get(name).value, compactValue, `${name} compact`);
    assert.equal(balanced.get(name).status, 'approved');
    assert.equal(compact.get(name).status, 'approved');
  }
});

test('theme and density dimensions resolve independently', () => {
  const lightCompact = resolveContext(records, { theme: 'light', density: 'compact', accent: 'blue' });
  const darkCompact = resolveContext(records, { theme: 'dark', density: 'compact', accent: 'blue' });
  const darkBalanced = resolveContext(records, { theme: 'dark', density: 'balanced', accent: 'blue' });
  assert.notEqual(lightCompact.get('color.canvas').value, darkCompact.get('color.canvas').value);
  assert.equal(lightCompact.get('button.height').value, darkCompact.get('button.height').value);
  assert.notEqual(darkCompact.get('button.height').value, darkBalanced.get('button.height').value);
});

test('all accent presets are deterministic and satisfy mapped contrast targets', () => {
  for (const accent of ACCENTS) {
    for (const theme of ['light', 'dark']) {
      const context = resolveContext(records, { theme, density: 'balanced', accent });
      const get = (name) => context.get(name).value;
      assert.ok(contrastRatio(get('color.accent.text'), get('color.canvas')) >= 4.5, `${theme}/${accent} text`);
      assert.ok(contrastRatio(get('color.accent.on-solid'), get('color.accent.solid')) >= 4.5, `${theme}/${accent} solid`);
      assert.ok(contrastRatio(get('color.accent.on-subtle'), get('color.accent.subtle')) >= 4.5, `${theme}/${accent} subtle`);
      assert.ok(contrastRatio(get('color.focus'), get('color.canvas')) >= 3, `${theme}/${accent} focus`);
    }
  }
});

test('OKLCH generation is stable for a custom seed', () => {
  const strategy = {
    lightness: Object.fromEntries(['50', '100', '200', '300', '400', '500', '600', '700', '800', '900', '950'].map((tone) => [tone, records.find((record) => record.name === `accent.strategy.lightness.${tone}`).value])),
    maxChroma: {
      light: records.find((record) => record.name === 'accent.strategy.max-chroma.light').value,
      mid: records.find((record) => record.name === 'accent.strategy.max-chroma.mid').value,
      dark: records.find((record) => record.name === 'accent.strategy.max-chroma.dark').value,
    },
  };
  assert.deepEqual(generateAccentScale('#3b82f6', strategy), generateAccentScale('#3B82F6', strategy));
  assert.match(generateAccentScale('#c026d3', strategy)['500'], /^#[0-9a-f]{6}$/);
});

test('generated artifacts exactly match committed output snapshots', async () => {
  const outputs = buildOutputs(records);
  assert.deepEqual(outputs, buildOutputs(records), 'two in-memory builds are byte-identical');
  for (const [relative, expected] of Object.entries(outputs)) {
    assert.equal(await readFile(path.join(root, relative), 'utf8'), expected, relative);
  }
  assert.match(outputs['dist/css/tokens.css'], /\[data-bm-theme="dark"\]/);
  assert.match(outputs['dist/css/tokens.css'], /\[data-bm-density="compact"\]/);
  assert.match(outputs['dist/gtk/tokens.css'], /explicitly propagate theme, density, and accent state/);
});
