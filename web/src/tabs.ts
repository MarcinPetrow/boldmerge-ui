import { BoldMergeTab, BoldMergeTabPanel, BoldMergeTabs } from './components/tabs.js';
if (!customElements.get('bm-tab')) customElements.define('bm-tab', BoldMergeTab);
if (!customElements.get('bm-tab-panel')) customElements.define('bm-tab-panel', BoldMergeTabPanel);
if (!customElements.get('bm-tabs')) customElements.define('bm-tabs', BoldMergeTabs);
export { BoldMergeTab, BoldMergeTabPanel, BoldMergeTabs };
export type { TabChangeDetail } from './components/tabs.js';
