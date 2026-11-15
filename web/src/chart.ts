import { BoldMergeChart, BoldMergeChartTooltip } from './components/chart.js';
if (!customElements.get('bm-chart-tooltip')) customElements.define('bm-chart-tooltip', BoldMergeChartTooltip);
if (!customElements.get('bm-chart')) customElements.define('bm-chart', BoldMergeChart);
export { BoldMergeChart, BoldMergeChartTooltip };
export type { ChartSeries, ChartTooltipRow } from './components/chart.js';
