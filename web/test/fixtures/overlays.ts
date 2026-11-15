import '../../src/styles/tokens.css';
import '../../src/styles/foundations.css';
import '../../src/tooltip.js';
import '../../src/dropdown.js';
import './overlays.css';

document.querySelector('#project-actions')?.addEventListener('bm-dropdown-select', (event) => {
  const output = document.querySelector('#selection');
  const detail = (event as CustomEvent<{ value: string }>).detail;
  if (output) output.textContent = `Selected ${detail.value}.`;
});

await Promise.all(['bm-tooltip', 'bm-dropdown', 'bm-dropdown-group', 'bm-dropdown-item'].map((name) => customElements.whenDefined(name)));
(window as Window & { overlaysReady?: boolean }).overlaysReady = true;
