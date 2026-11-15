import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';
test.beforeEach(async ({ page }) => { await page.goto('/test/fixtures/command-palette.html'); await page.waitForFunction(() => Boolean(customElements.get('bm-command-palette'))); });

test('palette filters grouped commands, selects by keyboard, and restores trigger focus', async ({ page }) => {
  await page.locator('#trigger').click(); const palette = page.locator('#palette'); const input = palette.locator('input');
  await expect(input).toBeFocused(); await expect(palette.locator('[role="group"]')).toHaveCount(2);
  await input.fill('preferences'); await expect(palette.locator('[role="option"]')).toHaveCount(1); await input.press('Enter');
  await expect(page.locator('#result')).toHaveText('settings'); await expect(palette.locator('dialog')).not.toHaveAttribute('open', ''); await expect(page.locator('#trigger')).toBeFocused();
});

test('opt-in shortcut opens, disabled commands are skipped, and Escape closes', async ({ page }) => {
  await page.keyboard.press('Control+k'); const input = page.locator('#palette input'); await expect(input).toBeFocused();
  await input.press('ArrowDown'); await input.press('Enter'); await expect(page.locator('#result')).toHaveText('settings');
  await page.keyboard.press('Control+k'); await page.keyboard.press('Escape'); await expect(page.locator('#palette dialog')).not.toHaveAttribute('open', '');
});

test('reduced motion removes modal reveal animation', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' }); await page.locator('#trigger').click();
  expect(await page.locator('#palette dialog').evaluate((element) => getComputedStyle(element).animationName)).toBe('none');
});

for (const [theme, density] of visualMatrix) test(`@visual palette ${theme} ${density}`, async ({ page }) => { await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]); await page.locator('#trigger').click(); await expect(page).toHaveScreenshot(`command-palette-${theme}-${density}.png`, { animations: 'disabled', fullPage: true }); });
