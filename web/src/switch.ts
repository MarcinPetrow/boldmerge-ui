import { BoldMergeSwitch } from './components/switch.js';

if (!customElements.get('bm-switch')) customElements.define('bm-switch', BoldMergeSwitch);

export { BoldMergeSwitch };
export type { SwitchChangeDetail } from './components/switch.js';
