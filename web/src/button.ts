import { BoldMergeButton } from './components/button.js';

if (!customElements.get('bm-button')) customElements.define('bm-button', BoldMergeButton);

export { BoldMergeButton };
export type { ButtonActivateDetail, ButtonTreatment, ButtonType, ButtonVariant } from './components/button.js';
