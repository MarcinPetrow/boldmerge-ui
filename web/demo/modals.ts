import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/button.js';
import '../src/input.js';
import '../src/dialog.js';
import '../src/drawer.js';
import './modals-demo.css';
import type { BoldMergeDialog } from '../src/components/dialog.js';
import type { BoldMergeDrawer } from '../src/components/drawer.js';

const dialog = document.querySelector<BoldMergeDialog>('#dialog');
const drawer = document.querySelector<BoldMergeDrawer>('#drawer');
const dialogTrigger = document.querySelector<HTMLElement>('#dialog-trigger');
const drawerTrigger = document.querySelector<HTMLElement>('#drawer-trigger');
dialogTrigger?.addEventListener('bm-activate', (event) => dialog?.show({ trigger: dialogTrigger, initialFocus: document.querySelector<HTMLElement>('#name') ?? undefined, originalEvent: event }));
drawerTrigger?.addEventListener('bm-activate', (event) => drawer?.show({ trigger: drawerTrigger, initialFocus: document.querySelector<HTMLElement>('#filter') ?? undefined, originalEvent: event }));
for (const [element, name] of [[dialog, 'bm-dialog-close'], [drawer, 'bm-drawer-close']] as const) element?.addEventListener(name, (event) => {
  const detail = (event as CustomEvent<{ reason: string; returnValue: string }>).detail;
  const output = document.querySelector('#result');
  if (output) output.textContent = `${detail.reason}: ${detail.returnValue || 'closed'}`;
});
