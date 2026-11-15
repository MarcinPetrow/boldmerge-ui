import { BoldMergeSidebar, BoldMergeSidebarItem } from './components/sidebar.js';
if (!customElements.get('bm-sidebar-item')) customElements.define('bm-sidebar-item', BoldMergeSidebarItem);
if (!customElements.get('bm-sidebar')) customElements.define('bm-sidebar', BoldMergeSidebar);
export { BoldMergeSidebar, BoldMergeSidebarItem };
export type { SidebarNavigateDetail } from './components/sidebar.js';
