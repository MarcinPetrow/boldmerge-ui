import { BoldMergeTheme } from './foundations/theme-scope.js';

if (!customElements.get('bm-theme')) customElements.define('bm-theme', BoldMergeTheme);

export { BoldMergeTheme };
export type { DensityPreference, MotionPreference, ThemePreference } from './foundations/theme-scope.js';
