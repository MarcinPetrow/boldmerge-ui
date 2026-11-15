import '../../src/styles/tokens.css';
import '../../src/styles/foundations.css';
import '../../src/button.js';
import '../../src/input.js';
import '../../src/select.js';
import '../../src/toast.js';
import '../../src/validation-summary.js';
import './feedback.css';
import type { BoldMergeValidationSummary } from '../../src/components/validation-summary.js';

const form = document.querySelector<HTMLFormElement>('#profile-form');
const summary = document.querySelector<BoldMergeValidationSummary>('#summary');
form?.addEventListener('submit', (event) => {
  event.preventDefault();
  summary?.validate();
});
