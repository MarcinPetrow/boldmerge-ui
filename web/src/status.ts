import { BoldMergeStatus } from './components/status.js';
if (!customElements.get('bm-status')) customElements.define('bm-status', BoldMergeStatus);
export { BoldMergeStatus };
export type { StatusTone } from './components/status.js';
