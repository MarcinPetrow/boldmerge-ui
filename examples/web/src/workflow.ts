import '@boldmerge/web';
import '@boldmerge/web/styles.css';
import type { BoldMergeAppShell } from '@boldmerge/web/app-shell';
import type { BoldMergeChart } from '@boldmerge/web/chart';
import type { BoldMergeDialog } from '@boldmerge/web/dialog';
import type { BoldMergeKpi } from '@boldmerge/web/kpi';
import type { BoldMergeStatus } from '@boldmerge/web/status';
import type { BoldMergeToastRegion } from '@boldmerge/web/toast';
import './workflow.css';

const shell = document.querySelector<BoldMergeAppShell>('#workflow-shell');
const menu = document.querySelector<HTMLElement>('#workflow-menu');
const approvalTrigger = document.querySelector<HTMLElement>('#approve-release');
const approvalDialog = document.querySelector<BoldMergeDialog>('#approval-dialog');
const notifications = document.querySelector<BoldMergeToastRegion>('#workflow-notifications');
const result = document.querySelector<HTMLElement>('#workflow-result');
const confirmation = document.querySelector('#approval-confirmation');
const confirmApproval = document.querySelector<HTMLElement & { disabled: boolean }>('#confirm-approval');

menu?.addEventListener('bm-activate', () => shell?.toggleSidebar(menu));
document.querySelector('#workflow-theme')?.addEventListener('bm-activate', () => {
  document.documentElement.dataset.bmTheme = document.documentElement.dataset.bmTheme === 'dark' ? 'light' : 'dark';
});

approvalTrigger?.addEventListener('bm-activate', (event) => approvalDialog?.show({
  trigger: approvalTrigger,
  initialFocus: document.querySelector<HTMLElement>('#approval-confirmation') ?? undefined,
  originalEvent: event,
}));
confirmation?.addEventListener('bm-checkbox-change', (event) => {
  if (confirmApproval) confirmApproval.disabled = !(event as CustomEvent<{ checked: boolean }>).detail.checked;
});

approvalDialog?.addEventListener('bm-dialog-close', (event) => {
  const detail = (event as CustomEvent<{ returnValue: string }>).detail;
  if (detail.returnValue !== 'approved') return;
  const status = document.querySelector<BoldMergeStatus>('#candidate-status');
  if (status) { status.tone = 'accent'; status.textContent = 'Deploying'; }
  if (result) result.textContent = 'v2.14.0 was approved and the production rollout started.';
  notifications?.push({ title: 'Release approved', message: 'v2.14.0 is deploying to production.', tone: 'success', duration: 0 });
  if (confirmApproval) confirmApproval.disabled = true;
});

document.querySelector('#deployment-table')?.addEventListener('bm-table-sort', (event) => {
  const detail = (event as CustomEvent<{ key: string; direction: string }>).detail;
  if (result) result.textContent = `Sorted ${detail.key} ${detail.direction}.`;
});
document.querySelector('#deployment-table')?.addEventListener('bm-table-row-activate', (event) => {
  const detail = (event as CustomEvent<{ key: string }>).detail;
  if (result) result.textContent = `Opened deployment ${detail.key}.`;
});
document.querySelector('#deployment-pagination')?.addEventListener('bm-page-change', (event) => {
  const detail = (event as CustomEvent<{ page: number }>).detail;
  if (result) result.textContent = `Showing deployment history page ${detail.page}.`;
});
document.querySelector('#view-runbook')?.addEventListener('bm-activate', () => {
  if (result) result.textContent = 'Runbook opened for candidate v2.14.0.';
  document.querySelector('#releases')?.scrollIntoView();
});

const setSparkline = (selector: string, values: number[]): void => {
  const kpi = document.querySelector<BoldMergeKpi>(selector);
  if (kpi) kpi.sparkline = values;
};
setSparkline('#success-kpi', [96.4, 97.1, 96.8, 97.9, 98.2, 98.1, 98.7]);
setSparkline('#latency-kpi', [248, 241, 245, 232, 228, 224, 219]);

const chart = document.querySelector<BoldMergeChart>('#deployment-chart');
if (chart) {
  chart.categories = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri'];
  chart.series = [
    { label: 'Successful', values: [18, 24, 22, 31, 36] },
    { label: 'Failed', values: [4, 3, 5, 2, 1] },
  ];
}

await Promise.all(['bm-app-shell', 'bm-chart', 'bm-dialog', 'bm-table', 'bm-toast-region'].map((name) => customElements.whenDefined(name)));
(window as Window & { workflowReady?: boolean }).workflowReady = true;
