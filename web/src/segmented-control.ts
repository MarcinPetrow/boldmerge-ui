import { BoldMergeSegment, BoldMergeSegmentedControl } from './components/segmented-control.js';
if (!customElements.get('bm-segment')) customElements.define('bm-segment', BoldMergeSegment);
if (!customElements.get('bm-segmented-control')) customElements.define('bm-segmented-control', BoldMergeSegmentedControl);
export { BoldMergeSegment, BoldMergeSegmentedControl };
export type { SegmentedChangeDetail } from './components/segmented-control.js';
