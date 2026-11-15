import '../src/styles/tokens.css';
import '../src/styles/foundations.css';
import '../src/theme.js';
import '../src/pagination.js';
import './component-demo.css';
import type { BoldMergePagination } from '../src/components/pagination.js';

const pagination = document.querySelector<BoldMergePagination>('#pagination');
const result = document.querySelector('#result');
pagination?.addEventListener('bm-page-change', (event) => {
  const { page } = (event as CustomEvent<{ page: number }>).detail;
  if (result) result.textContent = `Showing page ${page}.`;
});
document.querySelector<HTMLSelectElement>('#page-size')?.addEventListener('change', (event) => {
  if (!pagination) return;
  pagination.pageSize = Number((event.currentTarget as HTMLSelectElement).value);
  pagination.page = 1;
});
