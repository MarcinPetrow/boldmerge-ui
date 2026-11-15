import '../../src/styles/tokens.css';
import '../../src/styles/foundations.css';
import '../../src/pagination.js';
import './pagination.css';

document.querySelector('#pagination')?.addEventListener('bm-page-change', (event) => {
  const result = document.querySelector('#result');
  if (result) result.textContent = String((event as CustomEvent<{ page: number }>).detail.page);
});
