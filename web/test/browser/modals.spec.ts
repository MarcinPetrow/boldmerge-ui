import { expect, test } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/modals.html');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { modalsReady?: boolean }).modalsReady))).toBe(true);
});

test('dialog manages modal focus, cancellable close, Escape, action result, scroll lock, and restoration', async ({ page }) => {
  const trigger = page.getByRole('button', { name: 'Edit project' });
  const host = page.locator('#edit-dialog');
  await trigger.click();
  await expect(page.getByRole('dialog', { name: 'Edit project' })).toBeVisible();
  await expect(page.getByRole('textbox', { name: 'Project name' })).toBeFocused();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('hidden');

  await page.evaluate(() => document.querySelector('#edit-dialog')?.addEventListener('bm-dialog-request-close', (event) => event.preventDefault(), { once: true }));
  await page.getByRole('button', { name: 'Save changes' }).click();
  await expect(host).toHaveAttribute('open', '');
  await page.getByRole('button', { name: 'Save changes' }).click();
  await expect(host).not.toHaveAttribute('open', '');
  await expect(page.locator('#result')).toHaveText('action:saved');
  await expect(trigger).toBeFocused();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('');

  await trigger.click();
  await page.keyboard.press('Escape');
  await expect(host).not.toHaveAttribute('open', '');
  await expect(trigger).toBeFocused();

  await trigger.click();
  await host.evaluate((element: HTMLElement & { dismissible: boolean }) => { element.dismissible = false; });
  await page.keyboard.press('Escape');
  await expect(host).toHaveAttribute('open', '');
  await host.evaluate((element: HTMLElement & { close(reason?: string): boolean }) => element.close('api'));
});

test('overlay drawer supports nested modal focus, shared scroll lock, Escape, and restoration', async ({ page }) => {
  const dialogTrigger = page.getByRole('button', { name: 'Edit project' });
  await dialogTrigger.click();
  const dialogInput = page.getByRole('textbox', { name: 'Project name' });
  await expect(dialogInput).toBeFocused();

  await page.evaluate(() => {
    const drawer = document.querySelector('#activity-drawer') as HTMLElement & { show(options?: { trigger?: HTMLElement; initialFocus?: HTMLElement }): void };
    drawer.show({ trigger: document.querySelector('#project-name') as HTMLElement, initialFocus: document.querySelector('#activity-filter') as HTMLElement });
  });
  await expect(page.getByRole('dialog', { name: 'Activity drawer' })).toBeVisible();
  await expect(page.getByRole('textbox', { name: 'Filter activity' })).toBeFocused();
  await page.keyboard.press('Escape');
  await expect(page.locator('#activity-drawer')).not.toHaveAttribute('open', '');
  await expect(dialogInput).toBeFocused();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('hidden');
  await page.keyboard.press('Escape');
  await expect(dialogTrigger).toBeFocused();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('');
});

test('push drawer stays non-modal on desktop and becomes Overlay on mobile', async ({ page }) => {
  const push = page.locator('#push-drawer');
  await expect(push.getByRole('complementary', { name: 'Project inspector' })).toBeVisible();
  await page.getByRole('button', { name: 'Content action' }).click();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('');
  const columns = await push.evaluate((host) => getComputedStyle(host.shadowRoot?.querySelector('[part="layout"]') as HTMLElement).gridTemplateColumns);
  expect(columns).toContain('360px');

  await page.setViewportSize({ width: 480, height: 800 });
  await expect(push.getByRole('dialog', { name: 'Project inspector' })).toBeVisible();
  expect(await page.evaluate(() => document.documentElement.style.overflow)).toBe('hidden');
});

test('dialog and drawer remove motion when reduced motion is requested', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await page.getByRole('button', { name: 'Edit project' }).click();
  const dialogAnimation = await page.locator('#edit-dialog').evaluate((host) => getComputedStyle(host.shadowRoot?.querySelector('dialog') as HTMLElement).animationName);
  expect(dialogAnimation).toBe('none');
  await page.keyboard.press('Escape');
  await page.getByRole('button', { name: 'Open activity drawer' }).click();
  const drawerAnimation = await page.locator('#activity-drawer').evaluate((host) => getComputedStyle(host.shadowRoot?.querySelector('dialog') as HTMLElement).animationName);
  expect(drawerAnimation).toBe('none');
});

test('@visual dialog and drawer have deterministic Light/Balanced and Dark/Compact baselines', async ({ page }) => {
  await page.getByRole('button', { name: 'Edit project' }).click();
  await expect(page).toHaveScreenshot('dialog-light-balanced.png', { animations: 'disabled' });
  await page.getByRole('button', { name: 'Cancel' }).click();
  await page.getByRole('button', { name: 'Open activity drawer' }).click();
  await expect(page).toHaveScreenshot('drawer-dark-compact.png', { animations: 'disabled' });
});
