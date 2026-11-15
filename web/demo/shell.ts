import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/app-shell.js';
import '../src/sidebar.js';
import '../src/topbar.js';
import '../src/icon-button.js';
import '../src/button.js';
import '../src/page-layout.js';
import '../src/section.js';
import '../src/card.js';
import '../src/status.js';
import '../src/theme.js';
import './shell-demo.css';
import type { BoldMergeAppShell } from '../src/components/app-shell.js';

const shell = document.querySelector<BoldMergeAppShell>('#shell');
const menu = document.querySelector<HTMLElement>('#menu');

menu?.addEventListener('bm-activate', () => shell?.toggleSidebar(menu));
document.querySelector('#collapse')?.addEventListener('bm-activate', () => {
  if (shell) shell.sidebarCollapsed = !shell.sidebarCollapsed;
});
