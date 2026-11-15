import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/theme.js';
import '../src/combobox.js';
import './combobox-demo.css';
document.querySelector('#region')?.addEventListener('bm-combobox-change', (event) => { const detail = (event as CustomEvent<{ label: string }>).detail; const result = document.querySelector('#result'); if (result) result.textContent = `Selected ${detail.label}.`; });
