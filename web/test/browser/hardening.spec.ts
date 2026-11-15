import { expect, test } from '@playwright/test';

test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/hardening.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-chart'))); });

test('reverses applicable horizontal keyboard navigation in RTL', async ({ page }) => {
  const tabs = page.locator('#rtl-tabs'); await tabs.getByRole('tab', { name: 'نظرة عامة طويلة' }).focus(); await page.keyboard.press('ArrowLeft'); await expect(tabs.getByRole('tab', { name: 'النشاط والتغييرات الأخيرة' })).toBeFocused();
  const chartTargets = page.locator('#rtl-chart').locator('button[data-index]'); await chartTargets.nth(1).focus(); await page.keyboard.press('ArrowRight'); await expect(chartTargets.nth(0)).toBeFocused(); await page.keyboard.press('ArrowLeft'); await expect(chartTargets.nth(1)).toBeFocused();
});

test('keeps localized and pseudo-localized labels complete without overflow', async ({ page }) => {
  await expect(page.getByRole('navigation', { name: 'Stronicowanie wyników' }).getByRole('button', { name: 'Następna strona' })).toBeVisible();
  await expect(page.locator('#localized-pagination').locator('[part="summary"]')).toContainText('Strona 4 z 10');
  const state = page.getByRole('region', { name: /Ńø řęšųłţš/ }); await expect(state).toBeVisible(); expect(await state.evaluate((element) => element.scrollWidth <= element.clientWidth)).toBe(true);
  await expect(page.locator('#rtl-chart').locator('button').first()).toHaveAttribute('aria-label', /ناجح: ١٨, فاشل: ٤/);
});

test('reflows at 320 CSS pixels without page-level horizontal scrolling', async ({ page }) => {
  await page.setViewportSize({ width: 320, height: 900 });
  await expect.poll(() => page.evaluate(() => ({ scroll: document.documentElement.scrollWidth, client: document.documentElement.clientWidth }))).toEqual({ scroll: 320, client: 320 });
  await expect(page.getByRole('navigation', { name: 'Stronicowanie wyników' })).toBeVisible();
  await expect(page.getByRole('table', { name: 'بيانات النشر' })).toBeVisible();
});

test('preserves custom visualization boundaries in forced colors', async ({ page, browserName }) => {
  test.skip(browserName === 'webkit', 'Playwright WebKit does not emulate the forced-colors media feature.');
  await page.emulateMedia({ forcedColors: 'active' });
  const filled = page.locator('#forced-progress').locator('[part~="filled"]').first(); await expect(filled).toHaveCSS('forced-color-adjust', 'none'); await expect(filled).toHaveCSS('background-color', 'rgb(0, 0, 0)');
  await expect(page.locator('#forced-skeleton').locator('[part~="skeleton"]')).toHaveCSS('border-top-width', '1px');
  await expect(page.locator('#rtl-chart').locator('.series-1 .line')).toHaveCSS('stroke-dasharray', '8px, 4px');
});

test('keeps all motion-bearing hardening examples static under reduced motion', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await expect(page.locator('#forced-progress').locator('[part~="segment"]').first()).toHaveCSS('transition-duration', '0s');
  await expect(page.locator('#forced-skeleton').locator('[part~="skeleton"]')).toHaveCSS('animation-name', 'none');
});

test('@visual RTL and pseudo-localized hardening baseline', async ({ page }) => { await page.setViewportSize({ width: 1280, height: 1329 }); await expect(page.locator('#rtl-chart').locator('.line')).toHaveCount(2); await page.evaluate(() => new Promise<void>((resolve) => requestAnimationFrame(() => requestAnimationFrame(() => resolve())))); await expect(page).toHaveScreenshot('hardening-rtl-pseudo-light-balanced.png', { animations: 'disabled', fullPage: true }); });
