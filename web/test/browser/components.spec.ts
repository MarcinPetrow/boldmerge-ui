import { expect, test } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/components.html');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { componentsReady?: boolean }).componentsReady))).toBe(true);
});

test('button uses native keyboard activation and blocks disabled/loading states', async ({ page }) => {
  const light = page.locator('#light-states');
  const primary = light.getByRole('button', { name: 'Primary', exact: true });
  const activations = await page.evaluate(() => {
    const result = { count: 0 };
    document.querySelector('#primary')?.addEventListener('bm-activate', () => { result.count += 1; });
    (window as Window & { activationResult?: { count: number } }).activationResult = result;
    return result.count;
  });
  expect(activations).toBe(0);
  await primary.focus();
  await page.keyboard.press('Enter');
  await page.keyboard.press('Space');
  expect(await page.evaluate(() => (window as Window & { activationResult?: { count: number } }).activationResult?.count)).toBe(2);

  await expect(light.getByRole('button', { name: 'Disabled', exact: true })).toBeDisabled();
  await expect(light.getByRole('button', { name: 'Loading', exact: true })).toBeDisabled();
  await expect(light.getByRole('button', { name: 'Loading', exact: true })).toHaveAttribute('aria-busy', 'true');
});

test('icon button requires a label, activates from keyboard, and blocks unavailable states', async ({ page }) => {
  const add = page.getByRole('button', { name: 'Add workspace' });
  await page.evaluate(() => {
    const state = { count: 0 };
    document.querySelector('#add-icon')?.addEventListener('bm-activate', () => { state.count += 1; });
    (window as Window & { iconActivations?: typeof state }).iconActivations = state;
  });
  await add.focus();
  await page.keyboard.press('Enter');
  await page.keyboard.press('Space');
  expect(await page.evaluate(() => (window as Window & { iconActivations?: { count: number } }).iconActivations?.count)).toBe(2);
  await expect(page.getByRole('button', { name: 'Loading action' })).toBeDisabled();
  await expect(page.getByRole('button', { name: 'Disabled action' })).toBeDisabled();
});

test('tag and status expose visible text in addition to visual treatment', async ({ page }) => {
  const display = page.locator('#display-states');
  await expect(display.getByText('Enterprise', { exact: true })).toBeVisible();
  for (const label of ['Draft', 'In review', 'Operational', 'Degraded', 'Failed']) await expect(display.getByText(label, { exact: true })).toBeVisible();
  expect(await page.locator('bm-status[tone="error"]').first().evaluate((host) => host.shadowRoot?.querySelector('[part="dot"]')?.getAttribute('aria-hidden'))).toBe('true');
});

test('page layout, section, and breadcrumbs expose composition and native navigation', async ({ page }) => {
  const breadcrumbs = page.getByRole('navigation', { name: 'Project breadcrumbs' });
  const light = page.locator('#light-states');
  const projects = light.getByRole('link', { name: 'Projects', exact: true });
  await expect(breadcrumbs).toBeVisible();
  await expect(projects).toHaveAttribute('href', '/projects');
  expect(await page.locator('#split-layout bm-breadcrumb-item[current]').evaluate((host) => host.shadowRoot?.querySelector('[aria-current]')?.getAttribute('aria-current'))).toBe('page');
  await projects.focus();
  await expect(projects).toBeFocused();

  const layout = page.locator('#split-layout');
  const metrics = await layout.evaluate((host) => {
    const root = host.shadowRoot as ShadowRoot;
    return {
      maxWidth: getComputedStyle(root.querySelector('[part="layout"]') as HTMLElement).maxWidth,
      columns: getComputedStyle(root.querySelector('[part="body"]') as HTMLElement).gridTemplateColumns,
      asideHidden: (root.querySelector('[part="aside"]') as HTMLElement).hidden,
      sectionGap: getComputedStyle(host.querySelector('bm-section') as HTMLElement).getPropertyValue('--bm-section-gap').trim(),
    };
  });
  expect(metrics.maxWidth).toBe('1200px');
  expect(metrics.columns).toContain('320px');
  expect(metrics.asideHidden).toBe(false);
  expect(metrics.sectionGap).toBe('32px');
  await expect(page.getByRole('region', { name: 'Deployment overview' })).toBeVisible();
});

test('tabs and segmented control provide exclusive keyboard selection and relationships', async ({ page }) => {
  const light = page.locator('#light-states');
  const overview = light.getByRole('tab', { name: 'Overview' });
  const activity = light.getByRole('tab', { name: 'Activity' });
  await expect(overview).toHaveAttribute('aria-selected', 'true');
  await overview.focus();
  await page.keyboard.press('ArrowRight');
  await expect(activity).toBeFocused();
  await expect(activity).toHaveAttribute('aria-selected', 'true');
  await expect(light.getByRole('tabpanel', { name: 'Activity' })).toContainText('Activity panel content');
  await expect(light.getByRole('tab', { name: 'Audit log' })).toHaveAttribute('aria-disabled', 'true');
  expect(await activity.getAttribute('aria-controls')).toBe(await light.getByRole('tabpanel', { name: 'Activity' }).getAttribute('id'));

  const list = light.getByRole('radio', { name: 'List', exact: true });
  const board = light.getByRole('radio', { name: 'Board' });
  await list.focus();
  await page.keyboard.press('ArrowRight');
  await expect(board).toBeFocused();
  await expect(board).toHaveAttribute('aria-checked', 'true');
  expect(await page.evaluate(() => new FormData(document.querySelector('#selection-form') as HTMLFormElement).get('view'))).toBe('board');
});

test('input exposes label, form value, validity, focus, and associated error text', async ({ page }) => {
  const email = page.getByRole('textbox', { name: 'Email address' });
  await email.focus();
  await expect(email).toBeFocused();
  await email.fill('person@example.com');
  expect(await page.evaluate(() => new FormData(document.querySelector('#test-form') as HTMLFormElement).get('email'))).toBe('person@example.com');

  await email.fill('');
  expect(await page.evaluate(() => (document.querySelector('#email') as HTMLElement & { reportValidity(): boolean }).reportValidity())).toBe(false);
  const association = await page.evaluate(() => {
    const host = document.querySelector('#email') as HTMLElement;
    const input = host.shadowRoot?.querySelector('input') as HTMLInputElement;
    const error = host.shadowRoot?.querySelector('[part="error"]') as HTMLElement;
    return {
      invalid: input.getAttribute('aria-invalid'),
      errorMessage: input.getAttribute('aria-errormessage'),
      errorId: error.id,
      errorText: error.textContent?.trim(),
      nativeMessage: input.validationMessage,
    };
  });
  expect(association.invalid).toBe('true');
  expect(association.errorMessage).toBe(association.errorId);
  expect(association.nativeMessage).toBeTruthy();
  expect(association.errorText).toContain(association.nativeMessage);

  await page.evaluate(() => (document.querySelector('#email') as HTMLElement & { setCustomValidity(message: string): void }).setCustomValidity('Use a company email.'));
  await expect(page.locator('#email').getByRole('alert')).toContainText('Use a company email.');
});

test('card exposes named composition and an accessible article', async ({ page }) => {
  const host = page.locator('#light-states bm-card');
  const card = page.getByRole('article', { name: 'Deployment summary' });
  await expect(card).toBeVisible();
  await expect(host.getByText('Deployment')).toBeVisible();
  await expect(host.getByText('Updated 2 minutes ago')).toBeVisible();
});

test('checkbox uses native Space, form, mixed, disabled, and required semantics', async ({ page }) => {
  const updates = page.getByRole('checkbox', { name: 'Enable updates' });
  await updates.focus();
  await expect(updates).toBeFocused();
  await page.evaluate(() => {
    const state = { count: 0, checked: false };
    document.querySelector('#updates')?.addEventListener('bm-checkbox-change', (event) => {
      state.count += 1;
      state.checked = (event as CustomEvent<{ checked: boolean }>).detail.checked;
    });
    (window as Window & { checkboxChanges?: typeof state }).checkboxChanges = state;
  });
  await page.keyboard.press('Space');
  await expect(updates).toBeChecked();
  expect(await page.evaluate(() => (window as Window & { checkboxChanges?: { count: number; checked: boolean } }).checkboxChanges)).toEqual({ count: 1, checked: true });
  expect(await page.evaluate(() => new FormData(document.querySelector('#test-form') as HTMLFormElement).get('updates'))).toBe('enabled');

  const mixed = page.getByRole('checkbox', { name: 'Mixed selection' });
  expect(await mixed.evaluate((input: HTMLInputElement) => input.indeterminate)).toBe(true);
  await mixed.focus();
  await page.keyboard.press('Space');
  expect(await mixed.evaluate((input: HTMLInputElement) => input.indeterminate)).toBe(false);

  await expect(page.getByRole('checkbox', { name: 'Disabled choice' })).toBeDisabled();
  expect(await page.evaluate(() => (document.querySelector('#terms') as HTMLElement & { reportValidity(): boolean }).reportValidity())).toBe(false);
  expect(await page.evaluate(() => {
    const host = document.querySelector('#terms') as HTMLElement;
    return {
      hostInvalid: host.hasAttribute('data-invalid'),
      ariaInvalid: host.shadowRoot?.querySelector('input')?.getAttribute('aria-invalid'),
    };
  })).toEqual({ hostInvalid: true, ariaInvalid: 'true' });
});

test('radio group owns exclusive form value and native-style arrow navigation', async ({ page }) => {
  const group = page.getByRole('radiogroup', { name: 'Deployment plan' });
  await expect(group).toBeVisible();
  const light = page.locator('#light-states');
  const starter = light.getByRole('radio', { name: 'Starter' });
  const team = light.getByRole('radio', { name: 'Team' });
  await expect(team).toBeChecked();
  await team.focus();
  await page.keyboard.press('ArrowLeft');
  await expect(starter).toBeFocused();
  await expect(starter).toBeChecked();
  expect(await page.evaluate(() => new FormData(document.querySelector('#test-form') as HTMLFormElement).get('plan'))).toBe('starter');
  await expect(light.getByRole('radio', { name: 'Enterprise' })).toBeDisabled();
});

test('switch uses Space, switch semantics, form state, and disabled behavior', async ({ page }) => {
  const toggle = page.getByRole('switch', { name: 'Deployment alerts' });
  await expect(toggle).toBeChecked();
  await toggle.focus();
  await page.keyboard.press('Space');
  await expect(toggle).not.toBeChecked();
  expect(await page.evaluate(() => new FormData(document.querySelector('#test-form') as HTMLFormElement).get('alerts'))).toBeNull();
  await expect(page.getByRole('switch', { name: 'Disabled switch' })).toBeDisabled();
});

test('select uses native keyboard, form, options, disabled, and validation semantics', async ({ page }) => {
  const light = page.locator('#light-states');
  const region = light.getByRole('combobox', { name: 'Region' });
  await expect(region).toHaveValue('eu');
  await region.focus();
  await page.keyboard.press('ArrowUp');
  await expect(region).toHaveValue('us');
  expect(await page.evaluate(() => new FormData(document.querySelector('#test-form') as HTMLFormElement).get('region'))).toBe('us');
  await expect(region.locator('option')).toHaveCount(3);
  await expect(region.locator('option[value="apac"]')).toBeDisabled();

  const required = light.getByRole('combobox', { name: 'Environment' });
  expect(await page.evaluate(() => (document.querySelector('#empty-select') as HTMLElement & { reportValidity(): boolean }).reportValidity())).toBe(false);
  await expect(required).toHaveAttribute('aria-invalid', 'true');
  await expect(page.locator('#empty-select').getByRole('alert')).toBeVisible();
  await expect(light.getByRole('combobox', { name: 'Disabled select' })).toBeDisabled();
});

test('@visual component states match deterministic Light/Balanced and Dark/Compact baselines', async ({ page }) => {
  await expect(page.locator('#light-states')).toHaveScreenshot('components-light-balanced.png', { animations: 'disabled' });
  await expect(page.locator('#dark-states')).toHaveScreenshot('components-dark-compact.png', { animations: 'disabled' });
  await page.locator('#light-states').getByRole('button', { name: 'Primary', exact: true }).focus();
  await expect(page.locator('#button-states')).toHaveScreenshot('button-keyboard-focus.png', { animations: 'disabled' });
  await page.getByRole('checkbox', { name: 'Enable updates' }).focus();
  await expect(page.locator('#checkbox-states')).toHaveScreenshot('checkbox-states-focus.png', { animations: 'disabled' });
  await page.getByRole('radio', { name: 'Team' }).focus();
  await expect(page.locator('#radio-switch-states')).toHaveScreenshot('radio-switch-states-focus.png', { animations: 'disabled' });
  await page.locator('#light-states').getByRole('combobox', { name: 'Region' }).focus();
  await expect(page.locator('#select-states')).toHaveScreenshot('select-states-focus.png', { animations: 'disabled' });
  await page.getByRole('button', { name: 'Add workspace' }).focus();
  await expect(page.locator('#display-states')).toHaveScreenshot('display-states-focus.png', { animations: 'disabled' });
  await page.locator('#light-states').getByRole('link', { name: 'Projects', exact: true }).focus();
  await expect(page.locator('#layout-states')).toHaveScreenshot('layout-states-focus.png', { animations: 'disabled' });
  await page.locator('#light-states').getByRole('tab', { name: 'Overview' }).focus();
  await expect(page.locator('#selection-states')).toHaveScreenshot('selection-states-focus.png', { animations: 'disabled' });
});
