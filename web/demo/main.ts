import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/index.js';
import './styles.css';

import type { BoldMergeTheme, DensityPreference, ThemePreference } from '../src/foundation.js';

const root = document.documentElement;
const theme = document.querySelector<HTMLSelectElement>('#theme');
const density = document.querySelector<HTMLSelectElement>('#density');
const accent = document.querySelector<HTMLSelectElement>('#accent');
const nested = document.querySelector<BoldMergeTheme>('#nested');

theme?.addEventListener('change', () => { root.dataset.bmTheme = theme.value as Exclude<ThemePreference, 'inherit' | 'system'>; });
density?.addEventListener('change', () => { root.dataset.bmDensity = density.value as Exclude<DensityPreference, 'inherit'>; });
accent?.addEventListener('change', () => {
  if (accent.value.startsWith('#')) nested?.setCustomAccent(accent.value);
  else root.dataset.bmAccent = accent.value;
});

function showGap(scope: Element | null, output: HTMLOutputElement | null): void {
  if (scope && output) output.value = getComputedStyle(scope).getPropertyValue('--bm-layout-gap').trim();
}

function updateMeasurements(): void {
  showGap(root, document.querySelector('#root-gap'));
  showGap(nested, document.querySelector('#nested-gap'));
}

for (const select of [theme, density, accent]) select?.addEventListener('change', () => requestAnimationFrame(updateMeasurements));
window.addEventListener('resize', updateMeasurements);
customElements.whenDefined('bm-theme').then(updateMeasurements);
