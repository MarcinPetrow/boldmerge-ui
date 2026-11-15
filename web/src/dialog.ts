import { BoldMergeDialog } from './components/dialog.js';
if (!customElements.get('bm-dialog')) customElements.define('bm-dialog', BoldMergeDialog);
export { BoldMergeDialog };
export type { DialogCloseDetail, DialogCloseReason, DialogOpenChangeDetail, DialogShowOptions } from './components/dialog.js';
