import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/table.js';
import './component-demo.css';

const table = document.querySelector('#projects');
const result = document.querySelector('#result');
table?.addEventListener('bm-table-sort', (event) => { const detail = (event as CustomEvent<{ key: string; direction: string }>).detail; if (result) result.textContent = `Sort ${detail.key} ${detail.direction}.`; });
table?.addEventListener('bm-table-row-activate', (event) => { const detail = (event as CustomEvent<{ key: string }>).detail; if (result) result.textContent = `Open ${detail.key}.`; });
table?.addEventListener('bm-table-selection-change', (event) => { const detail = (event as CustomEvent<{ selectedKeys: string[] }>).detail; if (result) result.textContent = `Selected: ${detail.selectedKeys.join(', ') || 'none'}.`; });
