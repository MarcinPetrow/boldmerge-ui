import { BoldMergeTooltip } from './components/tooltip.js';
if (!customElements.get('bm-tooltip')) customElements.define('bm-tooltip', BoldMergeTooltip);
export { BoldMergeTooltip };
export type { TooltipOpenChangeDetail, TooltipPlacement } from './components/tooltip.js';
