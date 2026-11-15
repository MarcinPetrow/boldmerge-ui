import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/skeleton.html');
  await page.waitForFunction(() => Boolean(customElements.get('bm-skeleton')));
});

test('supports text, rectangle, and circle shapes with one labelled loading status', async ({ page }) => {
  await expect(page.getByRole('status', { name: 'Loading project' })).toHaveCount(1);
  await expect(page.locator('#loading')).toHaveAttribute('aria-busy', 'true');
  await expect(page.locator('#circle')).toHaveAttribute('shape', 'circle');
  await expect(page.locator('#rectangle')).toHaveAttribute('shape', 'rectangle');
  expect(await page.locator('#circle').evaluate((host) => getComputedStyle(host).borderRadius)).toBe('0px');
  expect(await page.locator('#circle').evaluate((host) => getComputedStyle(host.shadowRoot!.querySelector('[part~="circle"]')!).borderRadius)).toBe('50%');
});

test('uses the approved shimmer timing and becomes static under reduced motion', async ({ page }) => {
  const bone = page.locator('#text').locator('[part~="skeleton"]');
  await expect(bone).toHaveCSS('animation-duration', '1.5s');
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await expect(bone).toHaveCSS('animation-name', 'none');
});

for (const [theme, density] of visualMatrix) {
  test(`@visual skeleton ${theme} ${density}`, async ({ page }) => {
    await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]);
    await expect(page).toHaveScreenshot(`skeleton-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
  });
}
