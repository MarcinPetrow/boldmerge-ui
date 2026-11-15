import { expect, test } from '@playwright/test';

test('selective foundation import has no registration side effects', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=foundation');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
  expect(await page.evaluate(() => Boolean(customElements.get('bm-theme')))).toBe(false);
});

test('selective theme import registers only bm-theme', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=theme');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
  expect(await page.evaluate(() => ({
    theme: Boolean(customElements.get('bm-theme')),
    button: Boolean(customElements.get('bm-button')),
    dialog: Boolean(customElements.get('bm-dialog')),
  }))).toEqual({ theme: true, button: false, dialog: false });
});

test('selective icon import registers only bm-icon', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=icon');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
  expect(await page.evaluate(() => ({
    icon: Boolean(customElements.get('bm-icon')),
    iconButton: Boolean(customElements.get('bm-icon-button')),
    theme: Boolean(customElements.get('bm-theme')),
  }))).toEqual({ icon: true, iconButton: false, theme: false });
});

const selectiveEntries: ReadonlyArray<readonly [string, readonly string[]]> = [['button', ['bm-button']], ['input', ['bm-input']], ['card', ['bm-card']], ['checkbox', ['bm-checkbox']], ['radio', ['bm-radio', 'bm-radio-group']], ['switch', ['bm-switch']], ['select', ['bm-select', 'bm-option']], ['icon-button', ['bm-icon-button']], ['tag', ['bm-tag']], ['status', ['bm-status']], ['page-layout', ['bm-page-layout']], ['section', ['bm-section']], ['breadcrumbs', ['bm-breadcrumbs', 'bm-breadcrumb-item']], ['tabs', ['bm-tabs', 'bm-tab', 'bm-tab-panel']], ['segmented-control', ['bm-segmented-control', 'bm-segment']], ['sidebar', ['bm-sidebar', 'bm-sidebar-item']], ['topbar', ['bm-topbar']], ['app-shell', ['bm-app-shell']], ['tooltip', ['bm-tooltip']], ['dropdown', ['bm-dropdown', 'bm-dropdown-group', 'bm-dropdown-item']], ['dialog', ['bm-dialog']], ['drawer', ['bm-drawer']], ['toast', ['bm-toast', 'bm-toast-region']], ['validation-summary', ['bm-validation-summary']], ['combobox', ['bm-combobox', 'bm-option']], ['command-palette', ['bm-command-palette', 'bm-command-group', 'bm-command']], ['pagination', ['bm-pagination']], ['table', ['bm-table']], ['avatar', ['bm-avatar', 'bm-avatar-group']], ['empty-state', ['bm-empty-state']], ['skeleton', ['bm-skeleton']], ['progress', ['bm-progress']], ['kpi', ['bm-kpi']], ['chart', ['bm-chart', 'bm-chart-tooltip']]];

for (const [entry, expected] of selectiveEntries) {
  test(`selective ${entry} import registers only ${expected}`, async ({ page }) => {
    await page.goto(`/test/fixtures/foundation.html?entry=${entry}`);
    await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
    expect(await page.evaluate(() => Object.fromEntries(['bm-theme', 'bm-button', 'bm-input', 'bm-card', 'bm-checkbox', 'bm-radio', 'bm-radio-group', 'bm-switch', 'bm-select', 'bm-option', 'bm-combobox', 'bm-command-palette', 'bm-command-group', 'bm-command', 'bm-pagination', 'bm-table', 'bm-avatar', 'bm-avatar-group', 'bm-empty-state', 'bm-skeleton', 'bm-progress', 'bm-kpi', 'bm-chart', 'bm-chart-tooltip', 'bm-icon-button', 'bm-tag', 'bm-status', 'bm-page-layout', 'bm-section', 'bm-breadcrumbs', 'bm-breadcrumb-item', 'bm-tabs', 'bm-tab', 'bm-tab-panel', 'bm-segmented-control', 'bm-segment', 'bm-sidebar', 'bm-sidebar-item', 'bm-topbar', 'bm-app-shell', 'bm-tooltip', 'bm-dropdown', 'bm-dropdown-group', 'bm-dropdown-item', 'bm-dialog', 'bm-drawer', 'bm-toast', 'bm-toast-region', 'bm-validation-summary'].map((name) => [name, Boolean(customElements.get(name))])))).toEqual({
      'bm-theme': false,
      'bm-button': expected.includes('bm-button'),
      'bm-input': expected.includes('bm-input'),
      'bm-card': expected.includes('bm-card'),
      'bm-checkbox': expected.includes('bm-checkbox'),
      'bm-radio': expected.includes('bm-radio'),
      'bm-radio-group': expected.includes('bm-radio-group'),
      'bm-switch': expected.includes('bm-switch'),
      'bm-select': expected.includes('bm-select'),
      'bm-option': expected.includes('bm-option'),
      'bm-combobox': expected.includes('bm-combobox'),
      'bm-command-palette': expected.includes('bm-command-palette'),
      'bm-command-group': expected.includes('bm-command-group'),
      'bm-command': expected.includes('bm-command'),
      'bm-pagination': expected.includes('bm-pagination'),
      'bm-table': expected.includes('bm-table'),
      'bm-avatar': expected.includes('bm-avatar'),
      'bm-avatar-group': expected.includes('bm-avatar-group'),
      'bm-empty-state': expected.includes('bm-empty-state'),
      'bm-skeleton': expected.includes('bm-skeleton'),
      'bm-progress': expected.includes('bm-progress'),
      'bm-kpi': expected.includes('bm-kpi'),
      'bm-chart': expected.includes('bm-chart'),
      'bm-chart-tooltip': expected.includes('bm-chart-tooltip'),
      'bm-icon-button': expected.includes('bm-icon-button'),
      'bm-tag': expected.includes('bm-tag'),
      'bm-status': expected.includes('bm-status'),
      'bm-page-layout': expected.includes('bm-page-layout'),
      'bm-section': expected.includes('bm-section'),
      'bm-breadcrumbs': expected.includes('bm-breadcrumbs'),
      'bm-breadcrumb-item': expected.includes('bm-breadcrumb-item'),
      'bm-tabs': expected.includes('bm-tabs'),
      'bm-tab': expected.includes('bm-tab'),
      'bm-tab-panel': expected.includes('bm-tab-panel'),
      'bm-segmented-control': expected.includes('bm-segmented-control'),
      'bm-segment': expected.includes('bm-segment'),
      'bm-sidebar': expected.includes('bm-sidebar'),
      'bm-sidebar-item': expected.includes('bm-sidebar-item'),
      'bm-topbar': expected.includes('bm-topbar'),
      'bm-app-shell': expected.includes('bm-app-shell'),
      'bm-tooltip': expected.includes('bm-tooltip'),
      'bm-dropdown': expected.includes('bm-dropdown'),
      'bm-dropdown-group': expected.includes('bm-dropdown-group'),
      'bm-dropdown-item': expected.includes('bm-dropdown-item'),
      'bm-dialog': expected.includes('bm-dialog'),
      'bm-drawer': expected.includes('bm-drawer'),
      'bm-toast': expected.includes('bm-toast'),
      'bm-toast-region': expected.includes('bm-toast-region'),
      'bm-validation-summary': expected.includes('bm-validation-summary'),
    });
  });
}

test('full import registers the complete foundation element set only', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=full');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
  expect(await page.evaluate(() => ({
    theme: Boolean(customElements.get('bm-theme')),
    button: Boolean(customElements.get('bm-button')),
    input: Boolean(customElements.get('bm-input')),
    card: Boolean(customElements.get('bm-card')),
    checkbox: Boolean(customElements.get('bm-checkbox')),
    radio: Boolean(customElements.get('bm-radio')),
    radioGroup: Boolean(customElements.get('bm-radio-group')),
    switch: Boolean(customElements.get('bm-switch')),
    select: Boolean(customElements.get('bm-select')),
    option: Boolean(customElements.get('bm-option')),
    icon: Boolean(customElements.get('bm-icon')),
    iconButton: Boolean(customElements.get('bm-icon-button')),
    tag: Boolean(customElements.get('bm-tag')),
    status: Boolean(customElements.get('bm-status')),
    pageLayout: Boolean(customElements.get('bm-page-layout')),
    section: Boolean(customElements.get('bm-section')),
    breadcrumbs: Boolean(customElements.get('bm-breadcrumbs')),
    breadcrumbItem: Boolean(customElements.get('bm-breadcrumb-item')),
    tabs: Boolean(customElements.get('bm-tabs')),
    tab: Boolean(customElements.get('bm-tab')),
    tabPanel: Boolean(customElements.get('bm-tab-panel')),
    segmentedControl: Boolean(customElements.get('bm-segmented-control')),
    segment: Boolean(customElements.get('bm-segment')),
    sidebar: Boolean(customElements.get('bm-sidebar')),
    sidebarItem: Boolean(customElements.get('bm-sidebar-item')),
    topbar: Boolean(customElements.get('bm-topbar')),
    appShell: Boolean(customElements.get('bm-app-shell')),
    tooltip: Boolean(customElements.get('bm-tooltip')),
    dropdown: Boolean(customElements.get('bm-dropdown')),
    dropdownGroup: Boolean(customElements.get('bm-dropdown-group')),
    dropdownItem: Boolean(customElements.get('bm-dropdown-item')),
    dialog: Boolean(customElements.get('bm-dialog')),
    drawer: Boolean(customElements.get('bm-drawer')),
    toast: Boolean(customElements.get('bm-toast')),
    toastRegion: Boolean(customElements.get('bm-toast-region')),
    validationSummary: Boolean(customElements.get('bm-validation-summary')),
    combobox: Boolean(customElements.get('bm-combobox')),
    commandPalette: Boolean(customElements.get('bm-command-palette')),
    commandGroup: Boolean(customElements.get('bm-command-group')),
    command: Boolean(customElements.get('bm-command')),
    pagination: Boolean(customElements.get('bm-pagination')),
    table: Boolean(customElements.get('bm-table')),
    avatar: Boolean(customElements.get('bm-avatar')),
    avatarGroup: Boolean(customElements.get('bm-avatar-group')),
    emptyState: Boolean(customElements.get('bm-empty-state')),
    skeleton: Boolean(customElements.get('bm-skeleton')),
    progress: Boolean(customElements.get('bm-progress')),
    kpi: Boolean(customElements.get('bm-kpi')),
    chart: Boolean(customElements.get('bm-chart')),
    chartTooltip: Boolean(customElements.get('bm-chart-tooltip')),
  }))).toEqual({ theme: true, button: true, input: true, card: true, checkbox: true, radio: true, radioGroup: true, switch: true, select: true, option: true, icon: true, combobox: true, commandPalette: true, commandGroup: true, command: true, pagination: true, table: true, avatar: true, avatarGroup: true, emptyState: true, skeleton: true, progress: true, kpi: true, chart: true, chartTooltip: true, iconButton: true, tag: true, status: true, pageLayout: true, section: true, breadcrumbs: true, breadcrumbItem: true, tabs: true, tab: true, tabPanel: true, segmentedControl: true, segment: true, sidebar: true, sidebarItem: true, topbar: true, appShell: true, tooltip: true, dropdown: true, dropdownGroup: true, dropdownItem: true, dialog: true, drawer: true, toast: true, toastRegion: true, validationSummary: true });
});

test('theme and density variables cross open Shadow DOM and remain locally overridable', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=theme');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);

  const styles = await page.evaluate(() => {
    const read = (id: string) => {
      const element = document.querySelector(id) as HTMLElement & { shadowRoot: ShadowRoot };
      const surface = element.shadowRoot.querySelector('.surface') as HTMLElement;
      const computed = getComputedStyle(surface);
      return { color: computed.color, background: computed.backgroundColor, padding: computed.paddingTop };
    };
    return { inherited: read('#inherited'), nested: read('#nested') };
  });
  expect(styles.inherited).toEqual({ color: 'rgb(249, 250, 251)', background: 'rgb(13, 15, 19)', padding: '16px' });
  expect(styles.nested).toEqual({ color: 'rgb(21, 24, 30)', background: 'rgb(245, 247, 249)', padding: '24px' });

  await page.setViewportSize({ width: 480, height: 800 });
  expect(await page.evaluate(() => getComputedStyle(document.querySelector('#inherited') as Element).getPropertyValue('--bm-layout-gap').trim())).toBe('16px');
});

test('runtime switching and custom accents update only the selected scope', async ({ page }) => {
  await page.goto('/test/fixtures/foundation.html?entry=theme');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { foundationReady?: boolean }).foundationReady))).toBe(true);
  const result = await page.evaluate(async () => {
    const nested = document.querySelector('#nested') as HTMLElement & {
      theme: string;
      density: string;
      setCustomAccent(seed: string): unknown;
      updateComplete: Promise<boolean>;
    };
    nested.theme = 'dark';
    nested.density = 'compact';
    nested.setCustomAccent('#c026d3');
    await nested.updateComplete;
    const surface = nested.shadowRoot?.querySelector('.surface') as HTMLElement;
    const outer = document.querySelector('#outer') as HTMLElement;
    return {
      theme: nested.dataset.bmTheme,
      density: nested.dataset.bmDensity,
      accent: nested.dataset.bmAccent,
      customTone: getComputedStyle(nested).getPropertyValue('--bm-accent-tone-700').trim(),
      nestedPadding: getComputedStyle(surface).paddingTop,
      outerPadding: getComputedStyle(outer).getPropertyValue('--bm-layout-gap').trim(),
    };
  });
  expect(result).toEqual({ theme: 'dark', density: 'compact', accent: 'custom', customTone: '#7c0089', nestedPadding: '16px', outerPadding: '16px' });
});
