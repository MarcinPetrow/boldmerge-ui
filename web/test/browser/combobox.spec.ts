import { expect, test } from '@playwright/test';
import { visualMatrix } from './visual-matrix.js';

test.beforeEach(async ({ page }) => {
  await page.goto('/test/fixtures/combobox.html');
  await page.waitForFunction(() => Boolean(customElements.get('bm-combobox')));
});

test('combobox filters, links listbox semantics, skips disabled options, and selects by keyboard', async ({ page }) => {
  const host = page.locator('#region');
  const input = host.locator('input');
  await input.fill('Euro');
  await expect(input).toHaveAttribute('role', 'combobox');
  await expect(input).toHaveAttribute('aria-expanded', 'true');
  await expect(host.locator('[role="listbox"]')).toBeVisible();
  await expect(host.locator('[role="option"]')).toHaveCount(1);
  await expect(input).toHaveAttribute('aria-activedescendant', /option-0/);
  await input.press('Enter');
  await expect(host).toHaveJSProperty('value', 'eu');
  await expect(input).toHaveValue('Europe');
  await expect(input).toHaveAttribute('aria-expanded', 'false');

  await input.fill('');
  await input.press('ArrowDown');
  const activeId = await input.getAttribute('aria-activedescendant');
  expect(activeId).toBeTruthy();
  await expect(host.locator(`#${activeId}`)).toHaveText('Europe');
  await input.press('Escape');
  await expect(input).toHaveAttribute('aria-expanded', 'false');
});

test('combobox participates in forms and defers input notification during IME composition', async ({ page }) => {
  const result = await page.locator('#region').evaluate((element) => {
    let inputs = 0;
    element.addEventListener('bm-combobox-input', () => inputs++);
    const input = element.shadowRoot?.querySelector('input') as HTMLInputElement;
    input.dispatchEvent(new CompositionEvent('compositionstart', { bubbles: true }));
    input.value = 'Eu';
    input.dispatchEvent(new InputEvent('input', { bubbles: true, composed: true, inputType: 'insertCompositionText', data: 'Eu', isComposing: true }));
    const during = inputs;
    input.dispatchEvent(new CompositionEvent('compositionend', { bubbles: true, data: 'Eu' }));
    return { during, after: inputs };
  });
  expect(result).toEqual({ during: 0, after: 1 });
  expect(await page.locator('#form').evaluate((form: HTMLFormElement) => form.checkValidity())).toBe(false);
  await page.locator('#region input').press('ArrowDown');
  await page.locator('#region input').press('Enter');
  await expect(page.locator('#region')).toHaveJSProperty('value', 'eu');
  expect(await page.locator('#form').evaluate((form: HTMLFormElement) => Object.fromEntries(new FormData(form)))).toEqual({ region: 'eu' });
});

for (const [theme, density] of visualMatrix) {
  test(`@visual combobox ${theme} ${density}`, async ({ page }) => {
    await page.locator('html').evaluate((element, values) => { element.dataset.bmTheme = values[0]; element.dataset.bmDensity = values[1]; }, [theme, density]);
    await page.locator('#region input').fill('e');
    await expect(page).toHaveScreenshot(`combobox-${theme}-${density}.png`, { animations: 'disabled', fullPage: true });
  });
}
