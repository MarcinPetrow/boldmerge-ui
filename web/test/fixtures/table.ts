import '../../src/styles/tokens.css';
import '../../src/styles/foundations.css';
import '../../src/table.js';
import './table.css';
const table = document.querySelector('#table'); const output = document.querySelector('#result');
table?.addEventListener('bm-table-sort', (event) => { const detail = (event as CustomEvent<{ key: string; direction: string }>).detail; if (output) output.textContent = `sort:${detail.key}:${detail.direction}`; });
table?.addEventListener('bm-table-row-activate', (event) => { if (output) output.textContent = `row:${(event as CustomEvent<{ key: string }>).detail.key}`; });
table?.addEventListener('bm-table-selection-change', (event) => { if (output) output.textContent = `selected:${(event as CustomEvent<{ selectedKeys: string[] }>).detail.selectedKeys.join(',')}`; });
