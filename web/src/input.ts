import { BoldMergeInput } from './components/input.js';

if (!customElements.get('bm-input')) customElements.define('bm-input', BoldMergeInput);

export { BoldMergeInput };
export type { InputType, InputValueDetail } from './components/input.js';
