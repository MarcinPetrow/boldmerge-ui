import { expect, test } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/shell.html');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { shellReady?: boolean }).shellReady))).toBe(true);
});

test('desktop shell preserves approved sidebar geometry, active state, and collapse', async ({ page }) => {
  const shell = page.locator('#app-shell');
  const sidebar = page.locator('#sidebar');
  await expect(page.getByRole('navigation', { name: 'Main navigation' })).toBeVisible();
  await expect(page.getByRole('link', { name: 'Overview' })).toHaveAttribute('aria-current', 'page');
  expect(await sidebar.evaluate((host) => getComputedStyle(host).width)).toBe('240px');
  await page.getByRole('button', { name: 'Toggle sidebar' }).click();
  await expect(shell).toHaveAttribute('sidebar-collapsed', '');
  await expect(sidebar).toHaveCSS('width', '64px');
  await expect(page.getByRole('link', { name: 'Projects' })).toHaveAttribute('aria-label', 'Projects');
});

test('mobile drawer keeps its closed content inert, dismisses on Escape, and restores trigger focus', async ({ page }) => {
  await page.setViewportSize({ width: 480, height: 800 });
  const menu = page.getByRole('button', { name: 'Open navigation' });
  const sidebar = page.locator('#sidebar');
  await expect(sidebar.locator('[part="sidebar"]')).toHaveAttribute('aria-hidden', 'true');
  await menu.click();
  await expect(page.locator('#app-shell')).toHaveAttribute('sidebar-open', '');
  await expect(sidebar.locator('[part="sidebar"]')).not.toHaveAttribute('aria-hidden', 'true');
  await expect(page.getByRole('link', { name: 'Overview' })).toBeFocused();
  await page.keyboard.press('Escape');
  await expect(page.locator('#app-shell')).not.toHaveAttribute('sidebar-open', '');
  await expect(menu).toBeFocused();
});

test('@visual application shell has deterministic responsive theme and density baselines', async ({ page }) => {
  await expect(page.locator('#app-shell')).toHaveScreenshot('shell-light-balanced.png', { animations: 'disabled' });
  await page.evaluate(() => { document.documentElement.dataset.bmTheme = 'dark'; document.documentElement.dataset.bmDensity = 'compact'; document.documentElement.dataset.bmAccent = 'violet'; });
  await expect(page.locator('#app-shell')).toHaveScreenshot('shell-dark-compact.png', { animations: 'disabled' });
  await page.setViewportSize({ width: 480, height: 800 });
  await page.getByRole('button', { name: 'Open navigation' }).click();
  await expect(page.locator('#app-shell')).toHaveScreenshot('shell-mobile-drawer.png', { animations: 'disabled' });
});
