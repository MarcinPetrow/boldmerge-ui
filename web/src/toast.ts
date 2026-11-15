import { BoldMergeToast, BoldMergeToastRegion } from './components/toast.js';
if (!customElements.get('bm-toast')) customElements.define('bm-toast', BoldMergeToast);
if (!customElements.get('bm-toast-region')) customElements.define('bm-toast-region', BoldMergeToastRegion);
export { BoldMergeToast, BoldMergeToastRegion };
export type { ToastCloseDetail, ToastCloseReason, ToastOpenChangeDetail, ToastOptions, ToastPlacement, ToastTone } from './components/toast.js';
