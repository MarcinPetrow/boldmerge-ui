import { BoldMergeDropdown, BoldMergeDropdownGroup, BoldMergeDropdownItem } from './components/dropdown.js';
if (!customElements.get('bm-dropdown-item')) customElements.define('bm-dropdown-item', BoldMergeDropdownItem);
if (!customElements.get('bm-dropdown-group')) customElements.define('bm-dropdown-group', BoldMergeDropdownGroup);
if (!customElements.get('bm-dropdown')) customElements.define('bm-dropdown', BoldMergeDropdown);
export { BoldMergeDropdown, BoldMergeDropdownGroup, BoldMergeDropdownItem };
export type { DropdownOpenChangeDetail, DropdownPlacement, DropdownSelectDetail } from './components/dropdown.js';
