import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

const workflowUrl = 'http://127.0.0.1:4181/workflow.html';

test.beforeEach(async ({ page }) => {
  await page.goto(workflowUrl);
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { workflowReady?: boolean }).workflowReady))).toBe(true);
});

test('loads the pinned self-hosted UI and data fonts', async ({ page }) => {
  const loaded = await page.evaluate(async () => {
    await Promise.all([
      document.fonts.load('400 16px Inter', 'BoldMerge'),
      document.fonts.load('400 16px "JetBrains Mono"', '12345'),
    ]);
    return {
      ui: document.fonts.check('400 16px Inter', 'BoldMerge'),
      data: document.fonts.check('400 16px "JetBrains Mono"', '12345'),
    };
  });
  expect(loaded).toEqual({ ui: true, data: true });
});

test('consumer workflow composes release evidence and completes approval', async ({ page }) => {
  await expect(page.getByRole('heading', { name: 'Release operations', level: 1 })).toBeVisible();
  await expect(page.getByRole('table', { name: 'Production deployment history' })).toBeVisible();
  await expect(page.getByText('7 of 8 checks complete')).toBeVisible();

  await page.getByRole('button', { name: 'Deployment' }).click();
  await expect(page.locator('#workflow-result')).toContainText('Sorted deployment');

  await page.getByRole('button', { name: 'Approve release' }).click();
  await expect(page.getByRole('dialog', { name: 'Approve production release' })).toBeVisible();
  const confirm = page.getByRole('button', { name: 'Approve and deploy' });
  await expect(confirm).toBeDisabled();
  const confirmation = page.getByRole('checkbox', { name: 'I reviewed the release evidence' });
  await confirmation.focus();
  await page.keyboard.press('Space');
  await expect(confirmation).toBeChecked();
  await expect(confirm).toBeEnabled();
  await confirm.click();
  await expect(page.getByRole('dialog', { name: 'Approve production release' })).toBeHidden();

  await expect(page.locator('#candidate-status')).toContainText('Deploying');
  await expect(page.locator('#workflow-result')).toContainText('production rollout started');
  await expect(page.getByRole('status').filter({ hasText: 'Release approved' })).toContainText('v2.14.0 is deploying');
});

test('consumer workflow preserves mobile navigation focus restoration', async ({ page }) => {
  await page.setViewportSize({ width: 480, height: 800 });
  const menu = page.getByRole('button', { name: 'Open navigation' });
  await menu.click();
  await expect(page.getByRole('link', { name: 'Overview' })).toBeFocused();
  await page.keyboard.press('Escape');
  await expect(menu).toBeFocused();
});

for (const [theme, density] of visualMatrix) {
  test(`@visual consumer workflow ${theme} ${density}`, async ({ page }) => {
    await page.locator('html').evaluate((element, values) => {
      element.dataset.bmTheme = values[0];
      element.dataset.bmDensity = values[1];
      element.dataset.bmAccent = values[0] === 'dark' ? 'violet' : 'cyan';
    }, [theme, density]);
    await expect(page.locator('#workflow-shell')).toHaveScreenshot(`integration-workflow-${theme}-${density}.png`, { animations: 'disabled' });
  });
}
