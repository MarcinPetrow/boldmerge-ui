import { BoldMergeKpi } from './components/kpi.js';
if (!customElements.get('bm-kpi')) customElements.define('bm-kpi', BoldMergeKpi);
export { BoldMergeKpi };
export type { KpiChangeTone } from './components/kpi.js';
