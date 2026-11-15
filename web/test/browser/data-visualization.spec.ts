import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/data-visualization.html');
  await page.waitForFunction(() => Boolean(customElements.get('bm-progress')) && Boolean(customElements.get('bm-kpi')));
});

test('progress exposes exact values independently from rounded visual segments', async ({ page }) => {
  const progress = page.getByRole('progressbar', { name: 'Storage used' });
  await expect(progress).toHaveAttribute('aria-valuenow', '37');
  await expect(progress).toHaveAttribute('aria-valuetext', '37 GB of 100 GB');
  await expect(page.locator('#progress').locator('[part~="segment"]')).toHaveCount(20);
  await expect(page.locator('#progress').locator('[part~="filled"]')).toHaveCount(7);
  await page.locator('#progress').evaluate((element) => { (element as HTMLElement & { value: number }).value = 140; });
  await expect(progress).toHaveAttribute('aria-valuenow', '100');
});

test('KPI keeps metric meaning textual and its sparkline decorative', async ({ page }) => {
  await expect(page.getByRole('article', { name: 'Monthly recurring revenue' })).toContainText('$128,400');
  await expect(page.locator('#revenue').locator('[part="change"]')).toHaveText('+8.2%');
  await expect(page.locator('#revenue').locator('svg')).toHaveAttribute('aria-hidden', 'true');
  await expect(page.locator('#errors')).toHaveAttribute('change-tone', 'favorable');
});

test('reduced motion removes segment transitions', async ({ page }) => {
  const segment = page.locator('#progress').locator('[part~="segment"]').first();
  await expect(segment).toHaveCSS('transition-duration', '0.12s');
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await expect(segment).toHaveCSS('transition-duration', '0s');
});

for (const [theme, density] of visualMatrix) {
  test(`@visual progress and KPI ${theme} ${density}`, async ({ page }) => {
    await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]);
    await expect(page).toHaveScreenshot(`data-visualization-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
  });
}
