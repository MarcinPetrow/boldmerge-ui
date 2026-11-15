import { BoldMergeCombobox } from './components/combobox.js';
import { BoldMergeOption } from './components/select.js';
if (!customElements.get('bm-option')) customElements.define('bm-option', BoldMergeOption);
if (!customElements.get('bm-combobox')) customElements.define('bm-combobox', BoldMergeCombobox);
export { BoldMergeCombobox, BoldMergeOption };
export type { ComboboxChangeDetail, ComboboxInputDetail, ComboboxOpenChangeDetail } from './components/combobox.js';
