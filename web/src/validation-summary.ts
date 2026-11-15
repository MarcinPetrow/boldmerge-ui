import { BoldMergeValidationSummary } from './components/validation-summary.js';
if (!customElements.get('bm-validation-summary')) customElements.define('bm-validation-summary', BoldMergeValidationSummary);
export { BoldMergeValidationSummary };
export type { ValidatableControl, ValidationFocusTarget, ValidationSummaryChangeDetail, ValidationSummaryError } from './components/validation-summary.js';
