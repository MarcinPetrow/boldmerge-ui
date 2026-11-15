// @vitest-environment happy-dom
import { afterEach, beforeAll, describe, expect, it, vi } from 'vitest';
import type { BoldMergeButton } from '../../src/components/button.js';
import type { BoldMergeCard } from '../../src/components/card.js';
import type { BoldMergeCheckbox } from '../../src/components/checkbox.js';
import type { BoldMergeInput } from '../../src/components/input.js';
import type { BoldMergeRadio, BoldMergeRadioGroup } from '../../src/components/radio.js';
import type { BoldMergeSwitch } from '../../src/components/switch.js';
import type { BoldMergeOption, BoldMergeSelect } from '../../src/components/select.js';
import type { BoldMergeIconButton } from '../../src/components/icon-button.js';
import type { BoldMergeIcon } from '../../src/components/icon.js';
import type { BoldMergeTag } from '../../src/components/tag.js';
import type { BoldMergeStatus } from '../../src/components/status.js';
import type { BoldMergePageLayout } from '../../src/components/page-layout.js';
import type { BoldMergeSection } from '../../src/components/section.js';
import type { BoldMergeBreadcrumbItem, BoldMergeBreadcrumbs } from '../../src/components/breadcrumbs.js';
import type { BoldMergeTab, BoldMergeTabPanel, BoldMergeTabs } from '../../src/components/tabs.js';
import type { BoldMergeSegment, BoldMergeSegmentedControl } from '../../src/components/segmented-control.js';
import type { BoldMergeSidebar, BoldMergeSidebarItem } from '../../src/components/sidebar.js';
import type { BoldMergeAppShell } from '../../src/components/app-shell.js';
import type { BoldMergeTooltip } from '../../src/components/tooltip.js';
import type { BoldMergeDropdown, BoldMergeDropdownItem } from '../../src/components/dropdown.js';
import type { BoldMergeDialog } from '../../src/components/dialog.js';
import type { BoldMergeDrawer } from '../../src/components/drawer.js';
import type { BoldMergeToast, BoldMergeToastRegion } from '../../src/components/toast.js';
import type { BoldMergeValidationSummary } from '../../src/components/validation-summary.js';
import type { BoldMergeCombobox } from '../../src/components/combobox.js';
import type { BoldMergeCommand, BoldMergeCommandPalette } from '../../src/components/command-palette.js';
import type { BoldMergePagination } from '../../src/components/pagination.js';
import type { BoldMergeTable } from '../../src/components/table.js';
import type { BoldMergeAvatar, BoldMergeAvatarGroup } from '../../src/components/avatar.js';
import type { BoldMergeEmptyState } from '../../src/components/empty-state.js';
import type { BoldMergeSkeleton } from '../../src/components/skeleton.js';
import type { BoldMergeProgress } from '../../src/components/progress.js';
import type { BoldMergeKpi } from '../../src/components/kpi.js';
import type { BoldMergeChart } from '../../src/components/chart.js';

beforeAll(async () => {
  if (!HTMLDialogElement.prototype.showModal) {
    HTMLDialogElement.prototype.showModal = function showModal() { this.open = true; this.setAttribute('open', ''); };
    HTMLDialogElement.prototype.close = function close(returnValue = '') { this.returnValue = returnValue; this.open = false; this.removeAttribute('open'); };
  }
  if (!HTMLElement.prototype.attachInternals) {
    Object.defineProperty(HTMLElement.prototype, 'attachInternals', {
      configurable: true,
      value() {
        let valid = true;
        let message = '';
        return {
          form: null,
          get validity() { return { valid }; },
          get validationMessage() { return message; },
          willValidate: true,
          setFormValue() {},
          setValidity(flags: ValidityStateFlags = {}, nextMessage = '') {
            valid = !Object.values(flags).some(Boolean);
            message = nextMessage;
          },
          checkValidity() { return valid; },
          reportValidity() { return valid; },
        } as unknown as ElementInternals;
      },
    });
  }
  await Promise.all([
    import('../../src/button.js'),
    import('../../src/input.js'),
    import('../../src/card.js'),
    import('../../src/checkbox.js'),
    import('../../src/radio.js'),
    import('../../src/switch.js'),
    import('../../src/select.js'),
    import('../../src/icon-button.js'),
    import('../../src/icon.js'),
    import('../../src/tag.js'),
    import('../../src/status.js'),
    import('../../src/page-layout.js'),
    import('../../src/section.js'),
    import('../../src/breadcrumbs.js'),
    import('../../src/tabs.js'),
    import('../../src/segmented-control.js'),
    import('../../src/sidebar.js'),
    import('../../src/topbar.js'),
    import('../../src/app-shell.js'),
    import('../../src/tooltip.js'),
    import('../../src/dropdown.js'),
    import('../../src/dialog.js'),
    import('../../src/drawer.js'),
    import('../../src/toast.js'),
    import('../../src/validation-summary.js'),
    import('../../src/combobox.js'),
    import('../../src/command-palette.js'),
    import('../../src/pagination.js'),
    import('../../src/table.js'),
    import('../../src/avatar.js'),
    import('../../src/empty-state.js'),
    import('../../src/skeleton.js'),
    import('../../src/progress.js'),
    import('../../src/kpi.js'),
    import('../../src/chart.js'),
  ]);
});

describe('icon foundation', () => {
  it('renders pinned geometry with canonical stroke and accessibility', async () => {
    const icon = document.createElement('bm-icon') as BoldMergeIcon;
    icon.name = 'search';
    icon.size = 'small';
    document.body.append(icon);
    await icon.updateComplete;
    const decorative = icon.shadowRoot?.querySelector('svg');
    expect(decorative?.getAttribute('stroke-width')).toBe('1.5');
    expect(decorative?.getAttribute('aria-hidden')).toBe('true');
    icon.label = 'Search';
    await icon.updateComplete;
    const meaningful = icon.shadowRoot?.querySelector('svg');
    expect(meaningful?.getAttribute('role')).toBe('img');
    expect(meaningful?.getAttribute('aria-label')).toBe('Search');
  });

  it('rejects names outside the reviewed catalog', async () => {
    const icon = document.createElement('bm-icon') as BoldMergeIcon;
    icon.name = 'not-reviewed' as never;
    document.body.append(icon);
    await icon.updateComplete;
    expect(icon.hasAttribute('data-invalid')).toBe(true);
    expect(icon.shadowRoot?.querySelector('svg')).toBeNull();
  });
});

describe('feedback behavior', () => {
  it('queues owned toasts and drains the queue after dismissal', async () => {
    const region = document.createElement('bm-toast-region') as BoldMergeToastRegion;
    region.maxVisible = 1;
    document.body.append(region);
    const first = region.push({ title: 'Saved', message: 'First', duration: 0 });
    const second = region.push({ title: 'Queued', message: 'Second', duration: 0 });
    expect(region.querySelectorAll('bm-toast')).toHaveLength(1);
    expect(region.querySelector<BoldMergeToast>('bm-toast')?.id).toBe(first);
    expect(region.dismiss(first)).toBe(true);
    await new Promise((resolve) => setTimeout(resolve));
    expect(region.querySelectorAll('bm-toast')).toHaveLength(1);
    expect(region.querySelector<BoldMergeToast>('bm-toast')?.id).toBe(second);
  });

  it('reports native form validity without duplicating validation rules', async () => {
    const form = document.createElement('form');
    form.id = 'account-form';
    const input = document.createElement('input');
    input.required = true;
    input.id = 'email';
    input.setAttribute('aria-label', 'Email');
    form.append(input);
    const summary = document.createElement('bm-validation-summary') as BoldMergeValidationSummary;
    summary.formId = form.id;
    document.body.append(form, summary);
    await new Promise((resolve) => setTimeout(resolve));
    await summary.updateComplete;
    expect(summary.validate({ focus: 'none' })).toBe(false);
    expect(summary.errors).toHaveLength(1);
    expect(summary.errors[0]).toMatchObject({ control: input, label: 'Email' });
    input.value = 'person@example.com';
    input.dispatchEvent(new Event('input', { bubbles: true }));
    await new Promise((resolve) => setTimeout(resolve));
    expect(summary.errors).toHaveLength(0);
    expect(summary.hidden).toBe(true);
  });
});

describe('bm-combobox behavior', () => {
  it('filters declarative options and selects the active result from the keyboard', async () => {
    const element = document.createElement('bm-combobox') as BoldMergeCombobox;
    element.label = 'Region';
    element.innerHTML = '<bm-option value="us">United States</bm-option><bm-option value="eu">Europe</bm-option>';
    document.body.append(element);
    await new Promise((resolve) => setTimeout(resolve));
    await element.updateComplete;
    const changed = vi.fn();
    element.addEventListener('bm-combobox-change', changed);
    const input = element.shadowRoot?.querySelector('input') as HTMLInputElement;
    input.value = 'Euro';
    input.dispatchEvent(new InputEvent('input', { bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.open).toBe(true);
    expect(element.shadowRoot?.querySelectorAll('[role="option"]')).toHaveLength(1);
    input.dispatchEvent(new KeyboardEvent('keydown', { key: 'Enter', bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.value).toBe('eu');
    expect(element.inputValue).toBe('Europe');
    expect(changed.mock.calls[0]?.[0].detail).toMatchObject({ value: 'eu', label: 'Europe' });
  });
});

describe('bm-command-palette behavior', () => {
  it('filters declarative commands and emits selection without executing routing', async () => {
    const palette = document.createElement('bm-command-palette') as BoldMergeCommandPalette;
    const first = document.createElement('bm-command') as BoldMergeCommand;
    first.value = 'project.new'; first.keywords = 'create workspace'; first.textContent = 'New project';
    const second = document.createElement('bm-command') as BoldMergeCommand;
    second.value = 'settings'; second.textContent = 'Open settings';
    palette.append(first, second); document.body.append(palette); await palette.updateComplete;
    const selected = vi.fn(); palette.addEventListener('bm-command-palette-select', selected);
    palette.show(); await palette.updateComplete;
    const input = palette.shadowRoot?.querySelector('input') as HTMLInputElement;
    input.value = 'workspace'; input.dispatchEvent(new InputEvent('input', { bubbles: true, composed: true })); await palette.updateComplete;
    expect(palette.shadowRoot?.querySelectorAll('[role="option"]')).toHaveLength(1);
    input.dispatchEvent(new KeyboardEvent('keydown', { key: 'Enter', bubbles: true })); await palette.updateComplete;
    expect(selected.mock.calls[0]?.[0].detail).toMatchObject({ value: 'project.new', label: 'New project' });
    expect(palette.open).toBe(false);
  });
});

describe('bm-pagination behavior', () => {
  it('derives visible pages, emits typed changes, and enforces boundaries', async () => {
    const pagination = document.createElement('bm-pagination') as BoldMergePagination;
    pagination.page = 4; pagination.totalItems = 236;
    const changed = vi.fn(); pagination.addEventListener('bm-page-change', changed);
    document.body.append(pagination); await pagination.updateComplete;
    expect(pagination.pageSize).toBe(25);
    expect(pagination.pageCount).toBe(10);
    expect(pagination.shadowRoot?.querySelector('[aria-current="page"]')?.textContent).toBe('4');
    expect(pagination.next()).toBe(true);
    expect(changed.mock.calls[0]?.[0].detail).toMatchObject({ page: 5, pageSize: 25, totalItems: 236 });
    expect(pagination.goToPage(99)).toBe(true);
    expect(pagination.page).toBe(10);
    expect(pagination.next()).toBe(false);
    pagination.totalItems = 1; await pagination.updateComplete;
    expect(pagination.page).toBe(1);
  });
});

describe('bm-table behavior', () => {
  it('coordinates a native table without owning or reordering its records', async () => {
    const element = document.createElement('bm-table') as BoldMergeTable;
    element.rowActivation = true;
    element.innerHTML = '<table><caption>Projects</caption><thead><tr><th aria-sort="none"><button data-bm-sort-key="name">Name</button></th></tr></thead><tbody><tr data-bm-row-key="atlas"><td>Atlas</td></tr></tbody></table><span slot="empty">Nothing here</span>';
    const sorted = vi.fn(); const activated = vi.fn();
    element.addEventListener('bm-table-sort', sorted); element.addEventListener('bm-table-row-activate', activated);
    document.body.append(element); await element.updateComplete;
    element.querySelector('button')?.click(); await element.updateComplete;
    expect(sorted.mock.calls[0]?.[0].detail).toMatchObject({ key: 'name', direction: 'ascending' });
    expect(element.querySelector('tbody')?.textContent).toContain('Atlas');
    element.querySelector('td')?.click();
    expect(activated.mock.calls[0]?.[0].detail.key).toBe('atlas');
    element.state = 'empty'; await element.updateComplete;
    expect(element.shadowRoot?.querySelector('[part~="empty"]')).not.toBeNull();
  });
});

describe('bm-avatar behavior', () => {
  it('derives deterministic initials and exposes group overflow semantics', async () => {
    const avatar = document.createElement('bm-avatar') as BoldMergeAvatar;
    avatar.name = 'Ada Lovelace'; avatar.availability = 'online'; avatar.availabilityLabel = 'Online';
    document.body.append(avatar); await avatar.updateComplete;
    expect(avatar.fallback).toBe('AL');
    expect(avatar.getAttribute('aria-label')).toBe('Ada Lovelace, Online');
    expect(avatar.shadowRoot?.querySelector('[part="availability"]')?.getAttribute('aria-hidden')).toBe('true');
    const group = document.createElement('bm-avatar-group') as BoldMergeAvatarGroup;
    group.label = 'Team'; group.overflowCount = 4; group.overflowLabel = '4 more people';
    document.body.append(group); await group.updateComplete;
    expect(group.shadowRoot?.querySelector('[part="overflow"]')?.textContent).toBe('+4');
    expect(group.shadowRoot?.querySelector('[part="overflow"]')?.getAttribute('aria-label')).toBe('4 more people');
  });
});

describe('bm-empty-state behavior', () => {
  it('names its region from a clamped heading and removes unused composition regions', async () => {
    const state = document.createElement('bm-empty-state') as BoldMergeEmptyState;
    state.heading = 'No projects'; state.description = 'Create the first project.'; state.headingLevel = 9;
    document.body.append(state); await state.updateComplete;
    expect(state.shadowRoot?.querySelector('[role="region"]')?.getAttribute('aria-labelledby')).toContain('bm-empty-state-title');
    expect(state.shadowRoot?.querySelector('[role="heading"]')?.getAttribute('aria-level')).toBe('6');
    expect((state.shadowRoot?.querySelector('[part="icon"]') as HTMLElement).hidden).toBe(true);
    expect((state.shadowRoot?.querySelector('[part="description"]') as HTMLElement).hidden).toBe(false);
    expect((state.shadowRoot?.querySelector('[part="action"]') as HTMLElement).hidden).toBe(true);
  });
});

describe('bm-skeleton behavior', () => {
  it('keeps the visual decorative and exposes an optional loading status', async () => {
    const skeleton = document.createElement('bm-skeleton') as BoldMergeSkeleton;
    skeleton.shape = 'circle'; skeleton.label = 'Loading account';
    document.body.append(skeleton); await skeleton.updateComplete;
    expect(skeleton.shadowRoot?.querySelector('[part~="skeleton"]')?.getAttribute('aria-hidden')).toBe('true');
    expect(skeleton.shadowRoot?.querySelector('[part~="circle"]')).not.toBeNull();
    expect(skeleton.shadowRoot?.querySelector('[role="status"]')?.textContent).toBe('Loading account');
  });
});

describe('data visualization behavior', () => {
  it('clamps exact progress while keeping segment count configurable', async () => {
    const progress = document.createElement('bm-progress') as BoldMergeProgress;
    progress.label = 'Import'; progress.value = 11; progress.max = 10; progress.segments = 5;
    document.body.append(progress); await progress.updateComplete;
    expect(progress.normalizedValue).toBe(10);
    expect(progress.shadowRoot?.querySelector('[role="progressbar"]')?.getAttribute('aria-valuenow')).toBe('10');
    expect(progress.shadowRoot?.querySelectorAll('[part~="filled"]')).toHaveLength(5);
  });

  it('renders KPI change meaning only from the explicit application tone', async () => {
    const kpi = document.createElement('bm-kpi') as BoldMergeKpi;
    kpi.label = 'Errors'; kpi.value = '12'; kpi.change = '−25%'; kpi.changeTone = 'favorable'; kpi.sparkline = [20, 18, 12];
    document.body.append(kpi); await kpi.updateComplete;
    expect(kpi.getAttribute('change-tone')).toBe('favorable');
    expect(kpi.shadowRoot?.querySelector('[part="change"]')?.textContent).toBe('−25%');
    expect(kpi.shadowRoot?.querySelector('svg')?.getAttribute('aria-hidden')).toBe('true');
  });
});

describe('bm-chart behavior', () => {
  it('keeps series data property-only and exposes category values to keyboard targets', async () => {
    const chart = document.createElement('bm-chart') as BoldMergeChart;
    chart.label = 'Deployments'; chart.categories = ['Mon', 'Tue']; chart.series = [{ label: 'Successful', values: [4, 7] }, { label: 'Failed', values: [2, 1] }];
    document.body.append(chart); await chart.updateComplete;
    expect(chart.hasAttribute('series')).toBe(false);
    expect(chart.shadowRoot?.querySelectorAll('.line')).toHaveLength(2);
    expect(chart.shadowRoot?.querySelector('button')?.getAttribute('aria-label')).toBe('Mon, Successful: 4, Failed: 2');
  });
});

afterEach(() => {
  document.body.replaceChildren();
});

describe('bm-button behavior', () => {
  it('emits a typed activation and suppresses unavailable states', async () => {
    const element = document.createElement('bm-button') as BoldMergeButton;
    element.textContent = 'Save';
    document.body.append(element);
    await element.updateComplete;
    const activated = vi.fn();
    element.addEventListener('bm-activate', activated);
    const button = element.shadowRoot?.querySelector('button') as HTMLButtonElement;

    button.click();
    expect(activated).toHaveBeenCalledOnce();
    expect(activated.mock.calls[0]?.[0].detail.originalEvent).toBeInstanceOf(MouseEvent);

    element.loading = true;
    await element.updateComplete;
    button.click();
    expect(activated).toHaveBeenCalledOnce();
    expect(button.disabled).toBe(true);
    expect(button.getAttribute('aria-busy')).toBe('true');
  });
});

describe('bm-input behavior', () => {
  it('updates value, emits value detail, and associates integrated errors', async () => {
    const element = document.createElement('bm-input') as BoldMergeInput;
    element.label = 'Email';
    element.required = true;
    document.body.append(element);
    await element.updateComplete;
    const changed = vi.fn();
    element.addEventListener('bm-input', changed);
    const input = element.shadowRoot?.querySelector('input') as HTMLInputElement;

    input.value = 'person@example.com';
    input.dispatchEvent(new InputEvent('input', { bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.value).toBe('person@example.com');
    expect(changed.mock.calls[0]?.[0].detail.value).toBe('person@example.com');
    expect(element.hasAttribute('data-filled')).toBe(true);

    element.setCustomValidity('Use a company email.');
    await element.updateComplete;
    const error = element.shadowRoot?.querySelector('[part="error"]') as HTMLElement;
    expect(error.textContent).toContain('Use a company email.');
    expect(input.getAttribute('aria-errormessage')).toBe(error.id);
    expect(input.getAttribute('aria-invalid')).toBe('true');
    expect(element.checkValidity()).toBe(false);
  });

  it('resets to its initial value', async () => {
    const element = document.createElement('bm-input') as BoldMergeInput;
    element.setAttribute('value', 'initial');
    document.body.append(element);
    await element.updateComplete;
    element.value = 'changed';
    element.formResetCallback();
    await element.updateComplete;
    expect(element.value).toBe('initial');
  });
});

describe('bm-card behavior', () => {
  it('exposes open composition regions only when their slots are populated', async () => {
    const element = document.createElement('bm-card') as BoldMergeCard;
    element.innerHTML = '<h2 slot="header">Title</h2><p>Body</p><span slot="footer">Footer</span>';
    document.body.append(element);
    await element.updateComplete;
    for (const slot of element.shadowRoot?.querySelectorAll('slot') ?? []) slot.dispatchEvent(new Event('slotchange'));
    await new Promise((resolve) => setTimeout(resolve));
    await element.updateComplete;
    expect(element.shadowRoot?.querySelector('[part="header"]')?.hasAttribute('hidden')).toBe(false);
    expect(element.shadowRoot?.querySelector('[part="footer"]')?.hasAttribute('hidden')).toBe(false);
    expect(element.shadowRoot?.querySelector('[part="card"]')?.tagName).toBe('ARTICLE');
  });
});

describe('bm-checkbox behavior', () => {
  it('tracks native checked state and emits typed change detail', async () => {
    const element = document.createElement('bm-checkbox') as BoldMergeCheckbox;
    element.textContent = 'Enable updates';
    document.body.append(element);
    await element.updateComplete;
    const changed = vi.fn();
    element.addEventListener('bm-checkbox-change', changed);
    const input = element.shadowRoot?.querySelector('input') as HTMLInputElement;

    input.checked = true;
    input.dispatchEvent(new Event('change', { bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.checked).toBe(true);
    expect(changed.mock.calls[0]?.[0].detail).toMatchObject({ checked: true, indeterminate: false, value: 'on' });
  });

  it('restores its initial state and exposes custom invalidity', async () => {
    const element = document.createElement('bm-checkbox') as BoldMergeCheckbox;
    element.setAttribute('checked', '');
    document.body.append(element);
    await element.updateComplete;
    element.checked = false;
    element.formResetCallback();
    await element.updateComplete;
    expect(element.checked).toBe(true);

    element.setCustomValidity('Selection required.');
    expect(element.checkValidity()).toBe(false);
    expect(element.validationMessage).toBe('Selection required.');
    expect(element.hasAttribute('data-invalid')).toBe(true);
  });
});

describe('bm-radio-group behavior', () => {
  it('owns exclusive selection, form value, and typed change detail', async () => {
    const group = document.createElement('bm-radio-group') as BoldMergeRadioGroup;
    group.name = 'plan';
    group.required = true;
    const first = document.createElement('bm-radio') as BoldMergeRadio;
    first.value = 'starter';
    first.textContent = 'Starter';
    const second = document.createElement('bm-radio') as BoldMergeRadio;
    second.value = 'team';
    second.textContent = 'Team';
    group.append(first, second);
    document.body.append(group);
    await group.updateComplete;
    await first.updateComplete;
    await second.updateComplete;
    const changed = vi.fn();
    group.addEventListener('bm-radio-group-change', changed);

    const input = second.shadowRoot?.querySelector('input') as HTMLInputElement;
    input.checked = true;
    input.dispatchEvent(new Event('change', { bubbles: true, composed: true }));
    await group.updateComplete;
    expect(group.value).toBe('team');
    expect(first.checked).toBe(false);
    expect(second.checked).toBe(true);
    expect(changed.mock.calls[0]?.[0].detail.value).toBe('team');
    expect(group.checkValidity()).toBe(true);
  });
});

describe('bm-switch behavior', () => {
  it('uses binary form state and emits typed change detail', async () => {
    const element = document.createElement('bm-switch') as BoldMergeSwitch;
    element.name = 'alerts';
    element.value = 'enabled';
    element.textContent = 'Deployment alerts';
    document.body.append(element);
    await element.updateComplete;
    const changed = vi.fn();
    element.addEventListener('bm-switch-change', changed);
    const input = element.shadowRoot?.querySelector('input') as HTMLInputElement;

    input.checked = true;
    input.dispatchEvent(new Event('change', { bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.checked).toBe(true);
    expect(input.getAttribute('role')).toBe('switch');
    expect(changed.mock.calls[0]?.[0].detail).toMatchObject({ checked: true, value: 'enabled' });
  });
});

describe('bm-select behavior', () => {
  it('mirrors declarative options, updates value, and emits typed detail', async () => {
    const element = document.createElement('bm-select') as BoldMergeSelect;
    element.label = 'Region';
    element.name = 'region';
    const us = document.createElement('bm-option') as BoldMergeOption;
    us.value = 'us';
    us.textContent = 'United States';
    const eu = document.createElement('bm-option') as BoldMergeOption;
    eu.value = 'eu';
    eu.textContent = 'Europe';
    element.append(us, eu);
    document.body.append(element);
    await element.updateComplete;
    await new Promise((resolve) => setTimeout(resolve));
    await element.updateComplete;
    const changed = vi.fn();
    element.addEventListener('bm-select-change', changed);
    const select = element.shadowRoot?.querySelector('select') as HTMLSelectElement;

    expect([...select.options].map((option) => [option.value, option.text])).toEqual([['us', 'United States'], ['eu', 'Europe']]);
    select.value = 'eu';
    select.dispatchEvent(new Event('change', { bubbles: true, composed: true }));
    await element.updateComplete;
    expect(element.value).toBe('eu');
    expect(changed.mock.calls[0]?.[0].detail.value).toBe('eu');
  });

  it('associates custom validation errors and resets its initial selection', async () => {
    const element = document.createElement('bm-select') as BoldMergeSelect;
    element.setAttribute('value', 'team');
    element.label = 'Plan';
    for (const value of ['starter', 'team']) {
      const option = document.createElement('bm-option') as BoldMergeOption;
      option.value = value;
      option.textContent = value;
      element.append(option);
    }
    document.body.append(element);
    await element.updateComplete;
    element.value = 'starter';
    element.formResetCallback();
    expect(element.value).toBe('team');
    element.setCustomValidity('Choose an available plan.');
    await element.updateComplete;
    const select = element.shadowRoot?.querySelector('select') as HTMLSelectElement;
    const error = element.shadowRoot?.querySelector('[part="error"]') as HTMLElement;
    expect(element.checkValidity()).toBe(false);
    expect(select.getAttribute('aria-errormessage')).toBe(error.id);
  });
});

describe('bm-icon-button behavior', () => {
  it('exposes its accessible label, activation, and unavailable states', async () => {
    const element = document.createElement('bm-icon-button') as BoldMergeIconButton;
    element.label = 'Add workspace';
    element.textContent = '+';
    document.body.append(element);
    await element.updateComplete;
    const activated = vi.fn();
    element.addEventListener('bm-activate', activated);
    const button = element.shadowRoot?.querySelector('button') as HTMLButtonElement;
    expect(button.getAttribute('aria-label')).toBe('Add workspace');
    button.click();
    expect(activated).toHaveBeenCalledOnce();
    element.loading = true;
    await element.updateComplete;
    expect(button.disabled).toBe(true);
    expect(button.getAttribute('aria-busy')).toBe('true');
  });
});

describe('bm-tag and bm-status display behavior', () => {
  it('render slotted text and keep status meaning visible', async () => {
    const tag = document.createElement('bm-tag') as BoldMergeTag;
    tag.textContent = 'Enterprise';
    const status = document.createElement('bm-status') as BoldMergeStatus;
    status.tone = 'success';
    status.textContent = 'Operational';
    document.body.append(tag, status);
    await Promise.all([tag.updateComplete, status.updateComplete]);
    const tagSlot = tag.shadowRoot?.querySelector('slot:not([name])') as HTMLSlotElement;
    const statusSlot = status.shadowRoot?.querySelector('slot') as HTMLSlotElement;
    expect(tagSlot.assignedNodes().map((node) => node.textContent).join('')).toContain('Enterprise');
    expect(statusSlot.assignedNodes().map((node) => node.textContent).join('')).toContain('Operational');
    expect(status.shadowRoot?.querySelector('[part="dot"]')?.getAttribute('aria-hidden')).toBe('true');
  });
});

describe('layout composition behavior', () => {
  it('exposes page header/content/aside and section regions', async () => {
    const layout = document.createElement('bm-page-layout') as BoldMergePageLayout;
    layout.layout = 'split';
    layout.innerHTML = '<h1 slot="header">Project</h1><p>Content</p><aside slot="aside">Details</aside>';
    const section = document.createElement('bm-section') as BoldMergeSection;
    section.label = 'Overview';
    section.separator = true;
    section.innerHTML = '<h2 slot="header">Overview</h2><p>Summary</p><span slot="footer">Updated</span>';
    layout.append(section);
    document.body.append(layout);
    await Promise.all([layout.updateComplete, section.updateComplete]);
    for (const slot of layout.shadowRoot?.querySelectorAll('slot') ?? []) slot.dispatchEvent(new Event('slotchange'));
    for (const slot of section.shadowRoot?.querySelectorAll('slot') ?? []) slot.dispatchEvent(new Event('slotchange'));
    await Promise.all([layout.updateComplete, section.updateComplete]);
    expect(layout.shadowRoot?.querySelector('[part="header"]')?.hasAttribute('hidden')).toBe(false);
    expect(layout.shadowRoot?.querySelector('[part="aside"]')?.hasAttribute('hidden')).toBe(false);
    expect(section.shadowRoot?.querySelector('section')?.getAttribute('aria-label')).toBe('Overview');
    expect(section.shadowRoot?.querySelector('[part="footer"]')?.hasAttribute('hidden')).toBe(false);
  });

  it('marks breadcrumb order and current page without inventing routing', async () => {
    const breadcrumbs = document.createElement('bm-breadcrumbs') as BoldMergeBreadcrumbs;
    breadcrumbs.label = 'Project breadcrumbs';
    const parent = document.createElement('bm-breadcrumb-item') as BoldMergeBreadcrumbItem;
    parent.href = '/projects';
    parent.textContent = 'Projects';
    const current = document.createElement('bm-breadcrumb-item') as BoldMergeBreadcrumbItem;
    current.current = true;
    current.textContent = 'Northstar';
    breadcrumbs.append(parent, current);
    document.body.append(breadcrumbs);
    await Promise.all([breadcrumbs.updateComplete, parent.updateComplete, current.updateComplete]);
    breadcrumbs.shadowRoot?.querySelector('slot')?.dispatchEvent(new Event('slotchange'));
    await Promise.all([parent.updateComplete, current.updateComplete]);
    expect(parent.hasAttribute('data-first')).toBe(true);
    expect(parent.shadowRoot?.querySelector('a')?.getAttribute('href')).toBe('/projects');
    expect(current.shadowRoot?.querySelector('[aria-current="page"]')).not.toBeNull();
    expect(current.shadowRoot?.querySelector('[part="separator"]')?.textContent).toBe('/');
  });
});

describe('tabs and segmented selection behavior', () => {
  it('associates tabs with panels and changes one selected value', async () => {
    const tabs = document.createElement('bm-tabs') as BoldMergeTabs;
    tabs.label = 'Project views';
    for (const value of ['overview', 'activity']) {
      const tab = document.createElement('bm-tab') as BoldMergeTab;
      tab.slot = 'tab'; tab.value = value; tab.textContent = value;
      const panel = document.createElement('bm-tab-panel') as BoldMergeTabPanel;
      panel.value = value; panel.textContent = `${value} panel`;
      tabs.append(tab, panel);
    }
    document.body.append(tabs);
    await tabs.updateComplete;
    for (const slot of tabs.shadowRoot?.querySelectorAll('slot') ?? []) slot.dispatchEvent(new Event('slotchange'));
    const items = [...tabs.querySelectorAll<BoldMergeTab>('bm-tab')];
    const panels = [...tabs.querySelectorAll<BoldMergeTabPanel>('bm-tab-panel')];
    await Promise.all([...items, ...panels].map((item) => item.updateComplete));
    expect(tabs.value).toBe('overview');
    expect(items[0]?.getAttribute('aria-selected')).toBe('true');
    expect(panels[0]?.hidden).toBe(false);
    items[1]?.click();
    await tabs.updateComplete;
    expect(tabs.value).toBe('activity');
    expect(panels[0]?.hidden).toBe(true);
  });

  it('keeps segmented control exclusive and emits typed detail', async () => {
    const control = document.createElement('bm-segmented-control') as BoldMergeSegmentedControl;
    control.label = 'View mode';
    const changed = vi.fn();
    control.addEventListener('bm-segmented-change', changed);
    for (const value of ['list', 'grid']) {
      const segment = document.createElement('bm-segment') as BoldMergeSegment;
      segment.value = value; segment.textContent = value; control.append(segment);
    }
    document.body.append(control);
    await control.updateComplete;
    control.shadowRoot?.querySelector('slot')?.dispatchEvent(new Event('slotchange'));
    const segments = [...control.querySelectorAll<BoldMergeSegment>('bm-segment')];
    expect(control.value).toBe('list');
    segments[1]?.click();
    await control.updateComplete;
    expect(control.value).toBe('grid');
    expect(changed.mock.calls[0]?.[0].detail.value).toBe('grid');
    expect(segments[0]?.getAttribute('aria-checked')).toBe('false');
  });
});

describe('application shell behavior', () => {
  it('propagates collapsed/open state and exposes active navigation semantics', async () => {
    const shell = document.createElement('bm-app-shell') as BoldMergeAppShell;
    const sidebar = document.createElement('bm-sidebar') as BoldMergeSidebar;
    sidebar.slot = 'sidebar'; sidebar.label = 'Main navigation';
    const item = document.createElement('bm-sidebar-item') as BoldMergeSidebarItem;
    item.href = '/overview'; item.label = 'Overview'; item.active = true; item.textContent = 'Overview';
    sidebar.append(item); shell.append(sidebar);
    document.body.append(shell);
    await Promise.all([shell.updateComplete, sidebar.updateComplete, item.updateComplete]);
    shell.shadowRoot?.querySelector('slot[name="sidebar"]')?.dispatchEvent(new Event('slotchange'));
    shell.sidebarCollapsed = true; shell.sidebarOpen = true;
    await shell.updateComplete;
    await Promise.all([sidebar.updateComplete, item.updateComplete]);
    expect(sidebar.collapsed).toBe(true);
    expect(sidebar.open).toBe(true);
    expect(item.hasAttribute('data-collapsed')).toBe(true);
    expect(item.shadowRoot?.querySelector('a')?.getAttribute('aria-current')).toBe('page');
  });
});

describe('overlay primitive behavior', () => {
  it('exposes tooltip content as a keyboard description and open state', async () => {
    const tooltip = document.createElement('bm-tooltip') as BoldMergeTooltip;
    tooltip.content = 'Create a new project';
    const trigger = document.createElement('button');
    trigger.textContent = 'Create';
    tooltip.append(trigger);
    document.body.append(tooltip);
    await tooltip.updateComplete;
    tooltip.shadowRoot?.querySelector('slot:not([name])')?.dispatchEvent(new Event('slotchange'));
    await tooltip.updateComplete;
    expect(trigger.getAttribute('aria-description')).toBe('Create a new project');
    const changed = vi.fn();
    tooltip.addEventListener('bm-tooltip-open-change', changed);
    trigger.dispatchEvent(new FocusEvent('focusin', { bubbles: true, composed: true }));
    await tooltip.updateComplete;
    expect(tooltip.open).toBe(true);
    expect(changed.mock.calls[0]?.[0].detail.open).toBe(true);
    tooltip.hide();
    await tooltip.updateComplete;
    expect(tooltip.shadowRoot?.querySelector('[role="tooltip"]')?.getAttribute('aria-hidden')).toBe('true');

    const customTooltip = document.createElement('bm-tooltip') as BoldMergeTooltip;
    customTooltip.content = 'Custom-element description';
    const customTrigger = document.createElement('bm-button') as BoldMergeButton;
    customTrigger.textContent = 'Custom trigger';
    customTooltip.append(customTrigger);
    document.body.append(customTooltip);
    await Promise.all([customTooltip.updateComplete, customTrigger.updateComplete]);
    customTooltip.shadowRoot?.querySelector('slot:not([name])')?.dispatchEvent(new Event('slotchange'));
    expect(customTrigger.shadowRoot?.querySelector('button')?.getAttribute('aria-description')).toBe('Custom-element description');
  });

  it('opens a dropdown and emits typed selection without selecting disabled items', async () => {
    const dropdown = document.createElement('bm-dropdown') as BoldMergeDropdown;
    dropdown.label = 'Project actions';
    const rename = document.createElement('bm-dropdown-item') as BoldMergeDropdownItem;
    rename.value = 'rename'; rename.textContent = 'Rename';
    const disabled = document.createElement('bm-dropdown-item') as BoldMergeDropdownItem;
    disabled.value = 'archive'; disabled.textContent = 'Archive'; disabled.disabled = true;
    dropdown.append(rename, disabled);
    document.body.append(dropdown);
    await Promise.all([dropdown.updateComplete, rename.updateComplete, disabled.updateComplete]);
    const selected = vi.fn();
    dropdown.addEventListener('bm-dropdown-select', selected);
    dropdown.shadowRoot?.querySelector<HTMLButtonElement>('[part="trigger"]')?.click();
    await dropdown.updateComplete;
    expect(dropdown.open).toBe(true);
    rename.shadowRoot?.querySelector<HTMLButtonElement>('button')?.click();
    await dropdown.updateComplete;
    expect(selected.mock.calls[0]?.[0].detail).toMatchObject({ value: 'rename', href: '' });
    expect(dropdown.open).toBe(false);
    disabled.shadowRoot?.querySelector<HTMLButtonElement>('button')?.click();
    expect(selected).toHaveBeenCalledOnce();
  });
});

describe('modal composition behavior', () => {
  it('opens a native dialog, supports cancellable closing, and returns action detail', async () => {
    const dialog = document.createElement('bm-dialog') as BoldMergeDialog;
    dialog.label = 'Delete project';
    const input = document.createElement('input');
    input.setAttribute('autofocus', '');
    const action = document.createElement('button');
    action.slot = 'actions'; action.setAttribute('data-bm-dialog-close', 'confirmed'); action.textContent = 'Delete';
    dialog.append(input, action);
    document.body.append(dialog);
    await dialog.updateComplete;
    const closed = vi.fn();
    dialog.addEventListener('bm-dialog-close', closed);
    dialog.show({ initialFocus: input });
    await dialog.updateComplete;
    expect(dialog.shadowRoot?.querySelector('dialog')?.open).toBe(true);
    expect(document.documentElement.style.overflow).toBe('hidden');

    const prevent = (event: Event) => event.preventDefault();
    dialog.addEventListener('bm-dialog-request-close', prevent, { once: true });
    expect(dialog.close('api')).toBe(false);
    expect(dialog.open).toBe(true);
    action.click();
    await dialog.updateComplete;
    expect(dialog.open).toBe(false);
    expect(closed.mock.calls[0]?.[0].detail).toMatchObject({ reason: 'action', returnValue: 'confirmed' });
    expect(document.documentElement.style.overflow).toBe('');
  });

  it('keeps push drawer non-modal and exposes content/panel composition', async () => {
    const drawer = document.createElement('bm-drawer') as BoldMergeDrawer;
    drawer.mode = 'push'; drawer.label = 'Project details';
    const content = document.createElement('main'); content.slot = 'content'; content.textContent = 'Workspace';
    const panelButton = document.createElement('button'); panelButton.textContent = 'Panel action';
    drawer.append(content, panelButton);
    document.body.append(drawer);
    await drawer.updateComplete;
    drawer.show({ initialFocus: panelButton });
    await drawer.updateComplete;
    expect(drawer.shadowRoot?.querySelector('dialog')).toBeNull();
    expect(drawer.shadowRoot?.querySelector('aside')?.hidden).toBe(false);
    expect(document.documentElement.style.overflow).toBe('');
    expect(drawer.shadowRoot?.querySelector('slot[name="content"]')).not.toBeNull();
    expect(drawer.close()).toBe(true);
  });
});
