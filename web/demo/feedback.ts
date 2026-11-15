import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/theme.js';
import '../src/button.js';
import '../src/input.js';
import '../src/select.js';
import '../src/toast.js';
import '../src/validation-summary.js';
import './feedback-demo.css';
import type { BoldMergeToastRegion } from '../src/components/toast.js';
import type { BoldMergeValidationSummary } from '../src/components/validation-summary.js';

const notifications = document.querySelector<BoldMergeToastRegion>('#notifications');
document.querySelector('#success')?.addEventListener('bm-activate', () => notifications?.push({ title: 'Saved', message: 'Profile settings updated.', tone: 'success' }));
document.querySelector('#error')?.addEventListener('bm-activate', () => notifications?.push({ title: 'Connection interrupted', message: 'Try again in a moment.', tone: 'error', duration: 0 }));
document.querySelector<HTMLFormElement>('#demo-form')?.addEventListener('submit', (event) => {
  event.preventDefault();
  const summary = document.querySelector<BoldMergeValidationSummary>('#summary');
  if (summary?.validate()) return;
  notifications?.push({ title: 'Check the form', message: 'Correct the listed fields and try again.', tone: 'error', duration: 0 });
});
