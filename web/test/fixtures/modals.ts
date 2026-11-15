import '../../src/styles/tokens.css';
import '../../src/styles/foundations.css';
import '../../src/dialog.js';
import '../../src/drawer.js';
import './modals.css';
import type { BoldMergeDialog } from '../../src/components/dialog.js';
import type { BoldMergeDrawer } from '../../src/components/drawer.js';

const dialog = document.querySelector<BoldMergeDialog>('#edit-dialog');
const drawer = document.querySelector<BoldMergeDrawer>('#activity-drawer');
const dialogTrigger = document.querySelector<HTMLElement>('#open-dialog');
const drawerTrigger = document.querySelector<HTMLElement>('#open-drawer');

dialogTrigger?.addEventListener('click', (event) => dialog?.show({ trigger: dialogTrigger, initialFocus: document.querySelector<HTMLElement>('#project-name') ?? undefined, originalEvent: event }));
drawerTrigger?.addEventListener('click', (event) => drawer?.show({ trigger: drawerTrigger, initialFocus: document.querySelector<HTMLElement>('#activity-filter') ?? undefined, originalEvent: event }));

for (const element of [dialog, drawer]) element?.addEventListener(element === dialog ? 'bm-dialog-close' : 'bm-drawer-close', (event) => {
  const detail = (event as CustomEvent<{ reason: string; returnValue: string }>).detail;
  const result = document.querySelector('#result');
  if (result) result.textContent = `${detail.reason}:${detail.returnValue}`;
});

await Promise.all(['bm-dialog', 'bm-drawer'].map((name) => customElements.whenDefined(name)));
(window as Window & { modalsReady?: boolean }).modalsReady = true;
