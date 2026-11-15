import { BoldMergeDrawer } from './components/drawer.js';
if (!customElements.get('bm-drawer')) customElements.define('bm-drawer', BoldMergeDrawer);
export { BoldMergeDrawer };
export type { DrawerCloseDetail, DrawerCloseReason, DrawerMode, DrawerOpenChangeDetail, DrawerPosition, DrawerShowOptions } from './components/drawer.js';
