import { BoldMergeOption, BoldMergeSelect } from './components/select.js';

if (!customElements.get('bm-option')) customElements.define('bm-option', BoldMergeOption);
if (!customElements.get('bm-select')) customElements.define('bm-select', BoldMergeSelect);

export { BoldMergeOption, BoldMergeSelect };
export type { SelectChangeDetail } from './components/select.js';
