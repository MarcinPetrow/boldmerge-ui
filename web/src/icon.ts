import { BoldMergeIcon } from './components/icon.js';

if (!customElements.get('bm-icon')) customElements.define('bm-icon', BoldMergeIcon);

export { BOLDMERGE_ICON_NAMES, BoldMergeIcon } from './components/icon.js';
export type { BoldMergeIconName, BoldMergeIconSize } from './components/icon.js';

