import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';
test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/empty-state.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-empty-state'))); });

test('provides named region and configurable heading semantics without a default illustration', async ({ page }) => {
  const projects = page.getByRole('region', { name: 'No projects yet' }); await expect(projects).toBeVisible();
  await expect(projects.getByRole('heading', { name: 'No projects yet', level: 2 })).toBeVisible();
  await expect(page.getByRole('heading', { name: 'No search results', level: 3 })).toBeVisible();
  expect(await page.locator('#results').evaluate((host) => (host.shadowRoot?.querySelector('[part="icon"]') as HTMLElement).hidden)).toBe(true);
});

test('keeps an optional consumer action keyboard operable', async ({ page }) => {
  const action = page.getByRole('button', { name: 'Create project' }); await action.focus(); await page.keyboard.press('Enter'); await expect(page.locator('#result')).toHaveText('create');
});

for (const [theme, density] of visualMatrix) test(`@visual empty state ${theme} ${density}`, async ({ page }) => { await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]); await expect(page).toHaveScreenshot(`empty-state-${theme}-${density}.png`, { animations: 'disabled', fullPage: true }); });
