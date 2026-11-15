import { readFile } from 'node:fs/promises';
import path from 'node:path';
import { describe, expect, it } from 'vitest';

const root = path.resolve(import.meta.dirname, '../..');

describe('package contract', () => {
  it('publishes explicit full, selective, foundation, and CSS exports', async () => {
    const manifest = JSON.parse(await readFile(path.join(root, 'package.json'), 'utf8'));
    expect(manifest.name).toBe('@boldmerge/web');
    expect(Object.keys(manifest.exports)).toEqual(['.', './theme', './foundation', './icon', './button', './input', './card', './checkbox', './radio', './switch', './select', './icon-button', './tag', './status', './page-layout', './section', './breadcrumbs', './tabs', './segmented-control', './sidebar', './topbar', './app-shell', './tooltip', './dropdown', './dialog', './drawer', './toast', './validation-summary', './combobox', './command-palette', './pagination', './table', './avatar', './empty-state', './skeleton', './progress', './kpi', './chart', './fonts.css', './tokens.css', './styles.css', './package.json']);
    expect(manifest.exports['./theme'].import).toBe('./dist/theme.js');
    expect(manifest.exports['./foundation'].import).toBe('./dist/foundation.js');
    expect(manifest.exports['./icon'].import).toBe('./dist/icon.js');
    expect(manifest.exports['./button'].import).toBe('./dist/button.js');
    expect(manifest.exports['./input'].import).toBe('./dist/input.js');
    expect(manifest.exports['./card'].import).toBe('./dist/card.js');
    expect(manifest.exports['./checkbox'].import).toBe('./dist/checkbox.js');
    expect(manifest.exports['./radio'].import).toBe('./dist/radio.js');
    expect(manifest.exports['./switch'].import).toBe('./dist/switch.js');
    expect(manifest.exports['./select'].import).toBe('./dist/select.js');
    expect(manifest.exports['./icon-button'].import).toBe('./dist/icon-button.js');
    expect(manifest.exports['./tag'].import).toBe('./dist/tag.js');
    expect(manifest.exports['./status'].import).toBe('./dist/status.js');
    expect(manifest.exports['./page-layout'].import).toBe('./dist/page-layout.js');
    expect(manifest.exports['./section'].import).toBe('./dist/section.js');
    expect(manifest.exports['./breadcrumbs'].import).toBe('./dist/breadcrumbs.js');
    expect(manifest.exports['./tabs'].import).toBe('./dist/tabs.js');
    expect(manifest.exports['./segmented-control'].import).toBe('./dist/segmented-control.js');
    expect(manifest.exports['./sidebar'].import).toBe('./dist/sidebar.js');
    expect(manifest.exports['./topbar'].import).toBe('./dist/topbar.js');
    expect(manifest.exports['./app-shell'].import).toBe('./dist/app-shell.js');
    expect(manifest.exports['./tooltip'].import).toBe('./dist/tooltip.js');
    expect(manifest.exports['./dropdown'].import).toBe('./dist/dropdown.js');
    expect(manifest.exports['./dialog'].import).toBe('./dist/dialog.js');
    expect(manifest.exports['./drawer'].import).toBe('./dist/drawer.js');
    expect(manifest.exports['./toast'].import).toBe('./dist/toast.js');
    expect(manifest.exports['./validation-summary'].import).toBe('./dist/validation-summary.js');
    expect(manifest.exports['./combobox'].import).toBe('./dist/combobox.js');
    expect(manifest.exports['./command-palette'].import).toBe('./dist/command-palette.js');
    expect(manifest.exports['./pagination'].import).toBe('./dist/pagination.js');
    expect(manifest.exports['./table'].import).toBe('./dist/table.js');
    expect(manifest.exports['./avatar'].import).toBe('./dist/avatar.js');
    expect(manifest.exports['./empty-state'].import).toBe('./dist/empty-state.js');
    expect(manifest.exports['./skeleton'].import).toBe('./dist/skeleton.js');
    expect(manifest.exports['./progress'].import).toBe('./dist/progress.js');
    expect(manifest.exports['./kpi'].import).toBe('./dist/kpi.js');
    expect(manifest.exports['./chart'].import).toBe('./dist/chart.js');
  });

  it('has no consumer framework or runtime renderer dependency', async () => {
    const manifest = JSON.parse(await readFile(path.join(root, 'package.json'), 'utf8'));
    expect(manifest.dependencies).toEqual({ lit: '3.3.3', lucide: '1.73.0' });
    expect(manifest.dependencies).not.toHaveProperty('react');
    expect(manifest.dependencies).not.toHaveProperty('@angular/core');
    expect(manifest.dependencies).not.toHaveProperty('vue');
  });
});
