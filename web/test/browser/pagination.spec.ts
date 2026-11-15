import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/pagination.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-pagination'))); });

test('supports native keyboard activation and bounded page changes', async ({ page }) => {
  const pagination = page.getByRole('navigation', { name: 'Pagination' });
  await expect(pagination.getByRole('button', { name: 'Page 4' })).toHaveAttribute('aria-current', 'page');
  const next = pagination.getByRole('button', { name: 'Next' });
  await next.focus();
  await page.keyboard.press('Enter');
  await expect(page.locator('#result')).toHaveText('5');
  await expect(pagination.getByText('Page 5 of 10')).toBeVisible();
  await pagination.getByRole('button', { name: 'Page 10' }).click();
  await expect(next).toBeDisabled();
  await pagination.getByRole('button', { name: 'Previous' }).focus();
  await page.keyboard.press('Space');
  await expect(page.locator('#result')).toHaveText('9');
});

test('reduced motion removes button transitions', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' });
  expect(await page.getByRole('button', { name: 'Next' }).evaluate((element) => getComputedStyle(element).transitionDuration)).toBe('0s');
});

for (const [theme, density] of visualMatrix) test(`@visual pagination ${theme} ${density}`, async ({ page }) => {
  await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]);
  await expect(page).toHaveScreenshot(`pagination-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
});
