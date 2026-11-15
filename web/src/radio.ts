import { BoldMergeRadio, BoldMergeRadioGroup } from './components/radio.js';

if (!customElements.get('bm-radio')) customElements.define('bm-radio', BoldMergeRadio);
if (!customElements.get('bm-radio-group')) customElements.define('bm-radio-group', BoldMergeRadioGroup);

export { BoldMergeRadio, BoldMergeRadioGroup };
export type { RadioChangeDetail, RadioGroupChangeDetail } from './components/radio.js';
