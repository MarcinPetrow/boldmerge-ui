import { BoldMergeCheckbox } from './components/checkbox.js';

if (!customElements.get('bm-checkbox')) customElements.define('bm-checkbox', BoldMergeCheckbox);

export { BoldMergeCheckbox };
export type { CheckboxChangeDetail } from './components/checkbox.js';
