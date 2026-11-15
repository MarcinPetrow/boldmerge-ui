import { BoldMergePagination } from './components/pagination.js';

if (!customElements.get('bm-pagination')) customElements.define('bm-pagination', BoldMergePagination);

export { BoldMergePagination };
export type { PaginationChangeDetail } from './components/pagination.js';
