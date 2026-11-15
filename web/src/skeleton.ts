import { BoldMergeSkeleton } from './components/skeleton.js';
if (!customElements.get('bm-skeleton')) customElements.define('bm-skeleton', BoldMergeSkeleton);
export { BoldMergeSkeleton };
export type { SkeletonShape } from './components/skeleton.js';
