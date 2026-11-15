import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';
test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/table.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-table'))); });

test('preserves native semantics and emits application-controlled sorting', async ({ page }) => {
  const table = page.getByRole('table', { name: 'Workspace projects' });
  await expect(table).toBeVisible();
  const sort = table.getByRole('button', { name: 'Project' });
  await sort.focus(); await page.keyboard.press('Enter');
  await expect(page.locator('#result')).toHaveText('sort:name:ascending');
  await expect(sort.locator('xpath=ancestor::th')).toHaveAttribute('aria-sort', 'ascending');
  await sort.click(); await expect(page.locator('#result')).toHaveText('sort:name:descending');
});

test('activates rows from pointer and keyboard but not independent controls', async ({ page }) => {
  const row = page.getByRole('row', { name: /Atlas/ });
  await row.getByRole('cell', { name: 'Atlas', exact: true }).click(); await expect(page.locator('#result')).toHaveText('row:atlas');
  await row.getByRole('button', { name: 'Atlas actions' }).click(); await expect(page.locator('#result')).toHaveText('row:atlas');
  await row.focus(); await page.keyboard.press('Enter'); await expect(page.locator('#result')).toHaveText('row:atlas');
});

test('synchronizes row and select-all checkboxes while excluding disabled rows', async ({ page }) => {
  await page.getByRole('checkbox', { name: 'Select Atlas' }).check(); await expect(page.locator('#result')).toHaveText('selected:atlas');
  const all = page.getByRole('checkbox', { name: 'Select all projects' });
  await expect(all).toHaveJSProperty('indeterminate', true);
  await all.click(); await expect(page.locator('#result')).toHaveText('selected:atlas,northstar');
  await expect(page.getByRole('checkbox', { name: 'Select Archive' })).not.toBeChecked();
});

test('uses approved density row heights independent of viewport', async ({ page }) => {
  const row = page.getByRole('row', { name: /Atlas/ });
  await expect(row).toHaveCSS('height', '48px');
  await page.locator('html').evaluate((element) => { element.dataset.bmDensity = 'compact'; });
  await expect(row).toHaveCSS('height', '36px');
  await page.setViewportSize({ width: 700, height: 800 });
  await expect(row).toHaveCSS('height', '36px');
});

test('exposes loading and empty state content without a duplicate data model', async ({ page }) => {
  const states = page.getByRole('region', { name: 'Table states' });
  await expect(page.getByRole('status')).toBeVisible();
  await expect(states.getByText('Loading projects…')).toBeVisible();
  await expect(states.getByText('No projects match the filters.')).toBeVisible();
});

for (const [theme, density] of visualMatrix) test(`@visual table ${theme} ${density}`, async ({ page }) => { await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]); await expect(page).toHaveScreenshot(`table-${theme}-${density}.png`, { animations: 'disabled', fullPage: true }); });
