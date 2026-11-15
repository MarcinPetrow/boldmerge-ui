import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';
test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/avatar.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-avatar-group'))); });

test('exposes deterministic fallback and localized availability labels', async ({ page }) => {
  const sizes = page.getByRole('region', { name: 'Avatar sizes' });
  await expect(sizes.getByRole('img', { name: 'Ada Lovelace' })).toBeVisible();
  await expect(page.getByRole('img', { name: 'Katherine Johnson, Online' })).toBeVisible();
  expect(await page.locator('#small').evaluate((host) => host.shadowRoot?.querySelector('[part="fallback"]')?.textContent?.trim())).toBe('AL');
  expect(await page.locator('#organization').evaluate((host) => host.shadowRoot?.querySelector('[part="fallback"]')?.textContent?.trim())).toBe('B');
});

test('uses approved person, organization, and size geometry independent of density', async ({ page }) => {
  for (const [id, size] of [['small', 24], ['medium', 32], ['large', 40], ['xlarge', 48]] as const) {
    const metrics = await page.locator(`#${id}`).evaluate((host) => { const target = host.shadowRoot?.querySelector('[part="avatar"]') as HTMLElement; const style = getComputedStyle(target); return { width: target.getBoundingClientRect().width, radius: style.borderRadius }; });
    expect(metrics.width).toBe(size); expect(metrics.radius).toBe('50%');
  }
  await expect(page.locator('#organization').locator('[part="avatar"]')).toHaveCSS('border-radius', '8px');
  await page.locator('html').evaluate((element) => { element.dataset.bmDensity = 'compact'; });
  await page.setViewportSize({ width: 480, height: 700 });
  expect(await page.locator('#xlarge').evaluate((host) => (host.shadowRoot?.querySelector('[part="avatar"]') as HTMLElement).getBoundingClientRect().width)).toBe(48);
});

test('group exposes its label and explicit localized overflow', async ({ page }) => {
  const group = page.getByRole('group', { name: 'Platform team' }); await expect(group).toBeVisible();
  await expect(group.getByLabel('3 more team members')).toHaveText('+3');
  await expect(page.locator('bm-avatar-group').getByRole('img')).toHaveCount(3);
});

for (const [theme, density] of visualMatrix) test(`@visual avatar ${theme} ${density}`, async ({ page }) => { await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]); await expect(page).toHaveScreenshot(`avatar-${theme}-${density}.png`, { animations: 'disabled', fullPage: true }); });
