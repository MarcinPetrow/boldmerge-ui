import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/chart.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-chart')) && Boolean(customElements.get('bm-chart-tooltip'))); });

test('renders approved line, point, and grid geometry without an area fill', async ({ page }) => {
  const chart = page.locator('#deployments');
  await expect(chart.locator('.line')).toHaveCount(2);
  await expect(chart.locator('.point')).toHaveCount(10);
  await expect(chart.locator('.grid')).toHaveCount(5);
  await expect(chart.locator('.line').first()).toHaveCSS('stroke-width', '1.5px');
  await expect(chart.locator('.grid').first()).toHaveCSS('stroke-width', '1px');
  await expect(chart.locator('.line').first()).toHaveCSS('fill', 'none');
});

test('navigates categories by keyboard and exposes aligned multi-series tooltip values', async ({ page }) => {
  const targets = page.locator('#deployments').locator('button[data-index]');
  await targets.first().focus(); await page.keyboard.press('ArrowRight');
  await expect(targets.nth(1)).toBeFocused();
  const tooltip = page.locator('#deployments').locator('bm-chart-tooltip');
  await expect(tooltip).toHaveAttribute('open', '');
  await expect(tooltip.locator('[part="heading"]')).toHaveText('Tue');
  await expect(tooltip.locator('[part="row"]')).toHaveCount(2);
  await expect(tooltip.locator('[part="value"]')).toHaveText(['24', '3']);
  await page.keyboard.press('End'); await expect(targets.last()).toBeFocused();
  await page.keyboard.press('Escape'); await expect(tooltip).not.toHaveAttribute('open', '');
});

test('keeps complete text and a consumer-authored table alternative', async ({ page }) => {
  await expect(page.getByText('Successful and failed deployments over five weekdays')).toBeVisible();
  await expect(page.getByRole('table', { name: 'Weekly deployments data' })).toBeVisible();
  await expect(page.locator('#latency').locator('button').first()).toHaveAttribute('aria-label', /P50: 112 ms, P95: 240 ms/);
});

for (const [theme, density] of visualMatrix) test(`@visual chart ${theme} ${density}`, async ({ page }) => {
  await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]);
  await expect(page.locator('#latency').locator('[part="label"]')).toBeVisible();
  await expect(page.locator('#deployments').locator('.line')).toHaveCount(2);
  await expect(page.locator('#latency').locator('.line')).toHaveCount(2);
  await page.locator('#deployments').locator('button').nth(2).focus();
  await page.waitForTimeout(100);
  await expect(page).toHaveScreenshot(`chart-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
});
