import { expect, test } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/overlays.html');
  await expect.poll(() => page.evaluate(() => Boolean((window as Window & { overlaysReady?: boolean }).overlaysReady))).toBe(true);
});

test('tooltip opens after hover delay, opens on focus, auto-places, and closes on Escape', async ({ page }) => {
  const host = page.locator('#create-tooltip');
  const trigger = page.getByRole('button', { name: 'Create project' });
  const tooltip = host.getByRole('tooltip');
  await host.evaluate((element: HTMLElement) => { element.style.position = 'fixed'; element.style.inset = '2px auto auto 200px'; });
  await expect(trigger).toHaveAttribute('aria-description', 'Create a new project');
  await trigger.hover();
  await page.waitForTimeout(300);
  await expect(tooltip).not.toBeVisible();
  await expect(tooltip).toBeVisible({ timeout: 500 });
  await expect(tooltip).toHaveAttribute('data-placement', 'bottom');
  await page.mouse.move(700, 500);
  await expect(tooltip).not.toBeVisible();
  await trigger.focus();
  await expect(tooltip).toBeVisible();
  await page.keyboard.press('Escape');
  await expect(tooltip).not.toBeVisible();
  await expect(trigger).toBeFocused();
  await host.dispatchEvent('pointerenter', { pointerType: 'touch' });
  await page.waitForTimeout(450);
  await expect(tooltip).not.toBeVisible();
});

test('dropdown provides trigger semantics, keyboard navigation, typeahead, selection, dismissal, and focus restoration', async ({ page }) => {
  const host = page.locator('#project-actions');
  const trigger = page.getByRole('button', { name: 'Project actions' });
  await expect(trigger).toHaveAttribute('aria-haspopup', 'menu');
  await expect(trigger).toHaveAttribute('aria-expanded', 'false');
  await trigger.focus();
  await page.keyboard.press('ArrowDown');
  await expect(trigger).toHaveAttribute('aria-expanded', 'true');
  await expect(page.getByRole('menuitem', { name: 'Archive unavailable' })).toBeDisabled();
  await expect(page.getByRole('menuitem', { name: 'Rename' })).toBeFocused();
  await page.keyboard.press('ArrowDown');
  await expect(page.getByRole('menuitem', { name: 'Duplicate' })).toBeFocused();
  await page.keyboard.press('ArrowDown');
  await expect(page.getByRole('menuitem', { name: 'Delete project' })).toBeFocused();
  await page.keyboard.press('r');
  await expect(page.getByRole('menuitem', { name: 'Rename' })).toBeFocused();
  await page.keyboard.press('Enter');
  await expect(host).not.toHaveAttribute('open', '');
  await expect(trigger).toBeFocused();
  await expect(page.locator('#selection')).toHaveText('Selected rename.');

  await trigger.click();
  await expect(host).toHaveAttribute('open', '');
  await page.locator('#outside').click();
  await expect(host).not.toHaveAttribute('open', '');

  await trigger.focus();
  await page.keyboard.press('ArrowDown');
  await page.keyboard.press('Tab');
  await expect(host).not.toHaveAttribute('open', '');

  await trigger.focus();
  await page.keyboard.press('ArrowUp');
  await expect(page.getByRole('menuitem', { name: 'Delete project' })).toBeFocused();
  await page.keyboard.press('Escape');
  await expect(trigger).toBeFocused();
});

test('tooltip and dropdown remove overlay motion when reduced motion is requested', async ({ page }) => {
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await page.locator('#create-tooltip').getByRole('button').focus();
  await page.locator('#project-actions').getByRole('button', { name: 'Project actions' }).click();
  const motion = await page.evaluate(() => {
    const tooltip = document.querySelector('#create-tooltip')?.shadowRoot?.querySelector('[part="tooltip"]') as HTMLElement;
    const dropdown = document.querySelector('#project-actions')?.shadowRoot as ShadowRoot;
    return {
      tooltipTransition: getComputedStyle(tooltip).transitionDuration,
      indicatorTransition: getComputedStyle(dropdown.querySelector('[part="indicator"]') as HTMLElement).transitionDuration,
      menuAnimation: getComputedStyle(dropdown.querySelector('[part="menu"]') as HTMLElement).animationName,
    };
  });
  expect(motion).toEqual({ tooltipTransition: '0s', indicatorTransition: '0s', menuAnimation: 'none' });
});

test('@visual tooltip and dropdown have deterministic Light/Balanced and Dark/Compact baselines', async ({ page }) => {
  await page.locator('#create-tooltip').getByRole('button').focus();
  await page.locator('#project-actions').getByRole('button', { name: 'Project actions' }).click();
  await expect(page.locator('#light-overlays')).toHaveScreenshot('overlays-light-balanced.png', { animations: 'disabled' });
  await page.locator('#compact-actions').getByRole('button', { name: 'Compact actions' }).click();
  await expect(page.locator('#dark-overlays')).toHaveScreenshot('overlays-dark-compact.png', { animations: 'disabled' });
});
