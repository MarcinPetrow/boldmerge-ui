import '@boldmerge/web';
import '@boldmerge/web/styles.css';
import type { BoldMergeTheme } from '@boldmerge/web/theme';
import type { BoldMergeAppShell } from '@boldmerge/web/app-shell';
import type { BoldMergeDialog } from '@boldmerge/web/dialog';
import type { BoldMergeDrawer } from '@boldmerge/web/drawer';
import type { BoldMergeToastRegion } from '@boldmerge/web/toast';
import type { BoldMergeValidationSummary } from '@boldmerge/web/validation-summary';
import type { BoldMergeCommandPalette } from '@boldmerge/web/command-palette';
import './styles.css';

const appTheme = document.querySelector<BoldMergeTheme>('#app-theme');

function bindDimension(id: string, property: 'theme' | 'density' | 'accent'): void {
  const select = document.querySelector<HTMLSelectElement>(`#${id}`);
  select?.addEventListener('change', () => {
    if (appTheme) appTheme[property] = select.value as never;
  });
}

bindDimension('theme', 'theme');
bindDimension('density', 'density');
bindDimension('accent', 'accent');

const catalogShell = document.querySelector<BoldMergeAppShell>('#catalog-app-shell');
document.querySelector('#catalog-collapse')?.addEventListener('bm-activate', () => {
  if (catalogShell) catalogShell.sidebarCollapsed = !catalogShell.sidebarCollapsed;
});

document.querySelector('#catalog-actions')?.addEventListener('bm-dropdown-select', (event) => {
  const result = document.querySelector('#catalog-action-result');
  const detail = (event as CustomEvent<{ value: string }>).detail;
  if (result) result.textContent = `Selected “${detail.value}”.`;
});

const catalogDialog = document.querySelector<BoldMergeDialog>('#catalog-dialog');
const catalogDrawer = document.querySelector<BoldMergeDrawer>('#catalog-drawer');
const catalogDialogTrigger = document.querySelector<HTMLElement>('#catalog-dialog-trigger');
const catalogDrawerTrigger = document.querySelector<HTMLElement>('#catalog-drawer-trigger');
catalogDialogTrigger?.addEventListener('bm-activate', (event) => catalogDialog?.show({ trigger: catalogDialogTrigger, initialFocus: document.querySelector<HTMLElement>('#catalog-dialog-name') ?? undefined, originalEvent: event }));
catalogDrawerTrigger?.addEventListener('bm-activate', (event) => catalogDrawer?.show({ trigger: catalogDrawerTrigger, initialFocus: document.querySelector<HTMLElement>('#catalog-drawer-filter') ?? undefined, originalEvent: event }));
for (const [element, eventName] of [[catalogDialog, 'bm-dialog-close'], [catalogDrawer, 'bm-drawer-close']] as const) element?.addEventListener(eventName, (event) => {
  const detail = (event as CustomEvent<{ reason: string; returnValue: string }>).detail;
  const result = document.querySelector('#catalog-modal-result');
  if (result) result.textContent = `${detail.reason}: ${detail.returnValue || 'closed'}`;
});

const form = document.querySelector<HTMLFormElement>('#workspace-form');
const result = document.querySelector<HTMLElement>('#result');
const notifications = document.querySelector<BoldMergeToastRegion>('#catalog-notifications');
const summary = document.querySelector<BoldMergeValidationSummary>('#workspace-summary');

document.querySelector('#catalog-toast-success')?.addEventListener('bm-activate', () => notifications?.push({ title: 'Saved', message: 'The queued update completed.', tone: 'success' }));
document.querySelector('#catalog-toast-error')?.addEventListener('bm-activate', () => notifications?.push({ title: 'Action required', message: 'This notification stays until dismissed.', tone: 'error', duration: 0 }));
document.querySelector('#catalog-combobox')?.addEventListener('bm-combobox-change', (event) => {
  const detail = (event as CustomEvent<{ label: string }>).detail;
  const output = document.querySelector('#catalog-combobox-result');
  if (output) output.textContent = `Selected “${detail.label}”.`;
});
const commandPalette = document.querySelector<BoldMergeCommandPalette>('#catalog-command-palette');
const commandTrigger = document.querySelector<HTMLElement>('#catalog-command-trigger');
commandTrigger?.addEventListener('bm-activate', (event) => commandPalette?.show({ trigger: commandTrigger, originalEvent: event }));
commandPalette?.addEventListener('bm-command-palette-select', (event) => {
  const detail = (event as CustomEvent<{ value: string; label: string }>).detail;
  const output = document.querySelector('#catalog-command-result');
  if (output) output.textContent = `Selected “${detail.label}”.`;
  if (detail.value === 'toggle-theme' && appTheme) appTheme.theme = appTheme.theme === 'dark' ? 'light' : 'dark';
  else document.querySelector(`#${detail.value}`)?.scrollIntoView({ behavior: 'smooth' });
});
document.querySelector('#catalog-pagination')?.addEventListener('bm-page-change', (event) => {
  const detail = (event as CustomEvent<{ page: number }>).detail;
  const output = document.querySelector('#catalog-pagination-result');
  if (output) output.textContent = `Showing page ${detail.page} of 10.`;
});
const tableResult = document.querySelector('#catalog-table-result');
document.querySelector('#catalog-table')?.addEventListener('bm-table-sort', (event) => { const detail = (event as CustomEvent<{ key: string; direction: string }>).detail; if (tableResult) tableResult.textContent = `Sort ${detail.key} ${detail.direction}.`; });
document.querySelector('#catalog-table')?.addEventListener('bm-table-row-activate', (event) => { const detail = (event as CustomEvent<{ key: string }>).detail; if (tableResult) tableResult.textContent = `Open ${detail.key}.`; });
document.querySelector('#catalog-table')?.addEventListener('bm-table-selection-change', (event) => { const detail = (event as CustomEvent<{ selectedKeys: string[] }>).detail; if (tableResult) tableResult.textContent = `Selected: ${detail.selectedKeys.join(', ') || 'none'}.`; });
const setSparkline = (id: string, values: number[]) => { const element = document.querySelector(id) as HTMLElement & { sparkline: number[] }; if (element) element.sparkline = values; };
setSparkline('#catalog-revenue', [72, 74, 73, 79, 82, 86, 91]);
setSparkline('#catalog-errors', [26, 23, 25, 19, 18, 15, 12]);
setSparkline('#catalog-latency', [210, 218, 214, 228, 231, 242, 248]);
type CatalogChart = HTMLElement & { categories: string[]; series: Array<{ label: string; values: number[]; formattedValues?: string[] }> };
const setChart = (id: string, categories: string[], series: CatalogChart['series']) => { const chart = document.querySelector(id) as CatalogChart; if (chart) { chart.categories = categories; chart.series = series; } };
setChart('#catalog-deployments', ['Mon', 'Tue', 'Wed', 'Thu', 'Fri'], [{ label: 'Successful', values: [18, 24, 22, 31, 36] }, { label: 'Failed', values: [4, 3, 5, 2, 1] }]);
setChart('#catalog-chart-latency', ['09:00', '10:00', '11:00', '12:00', '13:00'], [{ label: 'P50', values: [112, 108, 116, 110, 106], formattedValues: ['112 ms', '108 ms', '116 ms', '110 ms', '106 ms'] }, { label: 'P95', values: [240, 226, 251, 232, 219], formattedValues: ['240 ms', '226 ms', '251 ms', '232 ms', '219 ms'] }]);

form?.addEventListener('submit', (event) => {
  event.preventDefault();
  if (!summary?.validate()) return;
  const data = new FormData(form);
  if (result) result.textContent = `Created “${String(data.get('workspace'))}”.`;
});

form?.addEventListener('reset', () => {
  if (result) result.textContent = 'Ready.';
});
