import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/tooltip.js';
import '../src/dropdown.js';
import './overlays-demo.css';

document.querySelector('#actions')?.addEventListener('bm-dropdown-select', (event) => {
  const result = document.querySelector('#result');
  const detail = (event as CustomEvent<{ value: string }>).detail;
  if (result) result.textContent = `Selected “${detail.value}”.`;
});
