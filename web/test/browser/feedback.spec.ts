import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/feedback.html');
  await page.waitForFunction(() => Boolean(customElements.get('bm-validation-summary')));
});

test('validation summary reflects native controls and focuses the selected error', async ({ page }) => {
  const summary = page.locator('bm-validation-summary');
  expect(await summary.evaluate((element: HTMLElement & { validate(): boolean }) => element.validate())).toBe(false);
  await expect(summary.locator('button')).toHaveCount(2);
  await expect(summary.locator('button').first()).toContainText('Email address');
  await expect.poll(() => page.locator('#email').evaluate((element) => element.matches(':focus-within'))).toBe(true);
  await summary.locator('button').nth(1).click();
  expect(await page.locator('#region').evaluate((element) => element.matches(':focus-within'))).toBe(true);
  await page.locator('#email').locator('input').fill('person@example.com');
  await page.locator('#region').locator('select').selectOption('eu');
  await expect(summary).toBeHidden();
});

test('toast close is keyboard operable and timeout pauses on hover', async ({ page }) => {
  const close = page.locator('#static-toasts bm-toast').first().locator('[part="close"]');
  await close.focus();
  await page.keyboard.press('Enter');
  await expect(page.locator('#static-toasts bm-toast').first()).toBeHidden();

  await page.evaluate(() => {
    const region = document.querySelector('#region-owner') as HTMLElement & { push(options: object): string };
    region.push({ title: 'Timed', message: 'Pause me', duration: 250 });
  });
  const timed = page.locator('#region-owner bm-toast');
  await timed.hover();
  await page.waitForTimeout(400);
  await expect(timed).toBeVisible();
  await page.locator('h1').hover();
  await expect(timed).toHaveCount(0, { timeout: 1000 });
});

test('toast region owns a bounded FIFO queue and exposes semantic live roles', async ({ page }) => {
  const result = await page.evaluate(() => {
    const region = document.querySelector('#region-owner') as HTMLElement & { push(options: object): string; dismiss(id: string): boolean };
    const first = region.push({ title: 'First', message: 'Visible', duration: 0, tone: 'success' });
    const second = region.push({ title: 'Second', message: 'Queued', duration: 0, tone: 'error' });
    return { first, second, count: region.querySelectorAll('bm-toast').length };
  });
  expect(result.count).toBe(1);
  await expect(page.locator('#region-owner bm-toast').locator('[role="status"]')).toBeVisible();
  await page.evaluate((id) => (document.querySelector('#region-owner') as HTMLElement & { dismiss(id: string): boolean }).dismiss(id), result.first);
  await expect(page.locator('#region-owner bm-toast')).toHaveAttribute('id', result.second);
  await expect(page.locator('#region-owner bm-toast').locator('[role="alert"]')).toBeVisible();
});

for (const [theme, density] of visualMatrix) {
  test(`@visual feedback ${theme} ${density}`, async ({ page }) => {
    await page.locator('html').evaluate((element, values) => {
      element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1];
    }, [theme, density]);
    await page.locator('bm-button').click();
    await expect(page).toHaveScreenshot(`feedback-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
  });
}

test('reduced motion removes toast reveal animation', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' });
  expect(await page.locator('bm-toast').first().locator('[part="toast"]').evaluate((element) => getComputedStyle(element).animationName)).toBe('none');
});
