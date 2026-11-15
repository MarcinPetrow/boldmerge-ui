import { BoldMergeCard } from './components/card.js';

if (!customElements.get('bm-card')) customElements.define('bm-card', BoldMergeCard);

export { BoldMergeCard };
