import { BoldMergeTable } from './components/table.js';

if (!customElements.get('bm-table')) customElements.define('bm-table', BoldMergeTable);

export { BoldMergeTable };
export type { TableRowActivateDetail, TableSelectionChangeDetail, TableSortDetail, TableSortDirection, TableState } from './components/table.js';
