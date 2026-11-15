import { LitElement, css, html, nothing, svg } from 'lit';

export interface ChartSeries {
  label: string;
  values: readonly number[];
  formattedValues?: readonly string[];
}

export interface ChartTooltipRow { label: string; value: string; series: number; }

export class BoldMergeChartTooltip extends LitElement {
  static properties = { heading: { type: String }, rows: { attribute: false }, open: { type: Boolean, reflect: true } };
  static styles = css`
    :host { display: none; position: absolute; z-index: 2; inline-size: max-content; min-inline-size: 160px; max-inline-size: 240px; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); pointer-events: none; transform: translate(-50%, calc(-100% - var(--bm-size-12))); }
    :host([open]) { display: block; }
    .tooltip { padding: var(--bm-size-12); background: var(--bm-color-surface); border: var(--bm-chart-grid-width) solid var(--bm-color-border); border-radius: var(--bm-radius-6); box-shadow: var(--bm-dialog-shadow); }
    .heading { margin-block-end: var(--bm-size-8); font-weight: var(--bm-typography-weight-emphasis); }
    .row { display: grid; grid-template-columns: auto minmax(0, 1fr) auto; align-items: center; gap: var(--bm-size-8); font-size: var(--bm-font-size-12); line-height: var(--bm-line-height-compact); }
    .dot { inline-size: var(--bm-size-7); block-size: var(--bm-size-7); border-radius: 50%; background: var(--bm-chart-series-color); }
    .value { font-family: var(--bm-typography-family-data); font-variant-numeric: tabular-nums; text-align: end; unicode-bidi: isolate; }
    .series-0 { --bm-chart-series-color: var(--bm-color-accent-text); } .series-1 { --bm-chart-series-color: var(--bm-color-success); } .series-2 { --bm-chart-series-color: var(--bm-color-warning); } .series-3 { --bm-chart-series-color: var(--bm-color-error); } .series-4 { --bm-chart-series-color: var(--bm-color-text-secondary); }
  `;
  heading = '';
  rows: readonly ChartTooltipRow[] = [];
  open = false;
  override render() { return html`<div class="tooltip" part="tooltip"><div class="heading" part="heading">${this.heading}</div>${this.rows.map((row) => html`<div class=${`row series-${row.series % 5}`} part="row"><span class="dot" part="dot" aria-hidden="true"></span><span part="series-label">${row.label}</span><bdi class="value" part="value">${row.value}</bdi></div>`)}</div>`; }
}

export class BoldMergeChart extends LitElement {
  static properties = {
    label: { type: String }, summary: { type: String }, categories: { attribute: false }, series: { attribute: false },
    activeIndex: { type: Number, attribute: 'active-index' },
  };
  static styles = css`
    :host { display: block; min-inline-size: 0; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .chart { display: grid; gap: var(--bm-size-12); }
    .heading { display: grid; gap: var(--bm-size-4); }
    .label { font-weight: var(--bm-typography-weight-emphasis); }
    .summary { color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); line-height: var(--bm-line-height-compact); }
    .plot { position: relative; min-inline-size: 0; min-block-size: 240px; }
    svg { display: block; inline-size: 100%; block-size: 240px; overflow: visible; }
    :host(:dir(rtl)) svg { transform: scaleX(-1); }
    .grid { stroke: var(--bm-color-border); stroke-width: var(--bm-chart-grid-width); vector-effect: non-scaling-stroke; }
    .line { fill: none; stroke: var(--bm-chart-series-color); stroke-width: var(--bm-chart-line-width); stroke-linecap: round; stroke-linejoin: round; vector-effect: non-scaling-stroke; }
    .point { fill: var(--bm-color-surface); stroke: var(--bm-chart-series-color); stroke-width: var(--bm-chart-line-width); vector-effect: non-scaling-stroke; }
    .series-0 { --bm-chart-series-color: var(--bm-color-accent-text); } .series-1 { --bm-chart-series-color: var(--bm-color-success); } .series-2 { --bm-chart-series-color: var(--bm-color-warning); } .series-3 { --bm-chart-series-color: var(--bm-color-error); } .series-4 { --bm-chart-series-color: var(--bm-color-text-secondary); }
    .target { position: absolute; inset-block: 0; inline-size: min(24px, 8%); padding: 0; cursor: crosshair; opacity: 0; transform: translateX(-50%); }
    .target:focus-visible { opacity: 1; background: transparent; border: 0; border-inline-start: var(--bm-size-2) solid var(--bm-color-focus); outline: 0; }
    .legend { display: flex; flex-wrap: wrap; gap: var(--bm-size-8) var(--bm-size-16); margin: 0; padding: 0; list-style: none; color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); }
    .legend li { display: inline-flex; align-items: center; gap: var(--bm-size-6); }
    .legend-dot { inline-size: var(--bm-size-7); block-size: var(--bm-size-7); border-radius: 50%; background: var(--bm-chart-series-color); }
    .alternative { max-inline-size: 100%; margin-block-start: var(--bm-size-8); overflow-x: auto; }
    @media (prefers-reduced-motion: reduce) { * { scroll-behavior: auto; } }
    @media (forced-colors: active) {
      .grid { stroke: GrayText; }
      .line, .point { stroke: CanvasText; }
      .series-1 .line { stroke-dasharray: 8 4; }
      .series-2 .line { stroke-dasharray: 2 3; }
      .series-3 .line { stroke-dasharray: 10 3 2 3; }
      .legend-dot { background: CanvasText; }
    }
  `;
  label = '';
  summary = '';
  categories: readonly string[] = [];
  series: readonly ChartSeries[] = [];
  activeIndex = -1;

  #values(): number[] { return this.series.flatMap((series) => series.values.filter(Number.isFinite)); }
  #range(): { min: number; max: number } { const values = this.#values(); const min = Math.min(...values, 0); const max = Math.max(...values, 1); return { min, max: max === min ? min + 1 : max }; }
  #x(index: number): number { return this.categories.length < 2 ? 300 : 24 + (index / (this.categories.length - 1)) * 552; }
  #y(value: number): number { const { min, max } = this.#range(); return 216 - ((value - min) / (max - min)) * 192; }
  #points(series: ChartSeries): string { return this.categories.map((_, index) => series.values[index]).filter(Number.isFinite).map((value, index) => `${this.#x(index)},${this.#y(value as number)}`).join(' '); }
  #rows(index: number): ChartTooltipRow[] { return this.series.flatMap((series, seriesIndex) => { const value = series.values[index]; return Number.isFinite(value) ? [{ label: series.label, value: series.formattedValues?.[index] ?? String(value), series: seriesIndex }] : []; }); }
  #categoryLabel(index: number): string { return [this.categories[index], ...this.#rows(index).map((row) => `${row.label}: ${row.value}`)].join(', '); }
  #key(event: KeyboardEvent, index: number): void {
    let next = index;
    const rtl = getComputedStyle(this).direction === 'rtl';
    if (event.key === 'ArrowRight') next = Math.min(this.categories.length - 1, Math.max(0, index + (rtl ? -1 : 1)));
    else if (event.key === 'ArrowLeft') next = Math.min(this.categories.length - 1, Math.max(0, index + (rtl ? 1 : -1)));
    else if (event.key === 'Home') next = 0;
    else if (event.key === 'End') next = this.categories.length - 1;
    else if (event.key === 'Escape') { this.activeIndex = -1; (event.currentTarget as HTMLElement).blur(); return; }
    else return;
    event.preventDefault(); this.activeIndex = next;
    this.shadowRoot?.querySelector<HTMLElement>(`button[data-index='${next}']`)?.focus();
  }
  override render() {
    const active = this.activeIndex >= 0 && this.activeIndex < this.categories.length ? this.activeIndex : -1;
    return html`<section class="chart" part="chart" aria-label=${this.label || nothing}>
      <div class="heading" part="heading"><span class="label" part="label">${this.label}</span><span class="summary" part="summary">${this.summary}</span></div>
      <div class="plot" part="plot" @mouseleave=${() => { if (!this.shadowRoot?.activeElement?.matches('.target')) this.activeIndex = -1; }}>
        <svg viewBox="0 0 600 240" preserveAspectRatio="none" aria-hidden="true">
          ${[0, 1, 2, 3, 4].map((step) => svg`<line class="grid" x1="24" x2="576" y1=${24 + step * 48} y2=${24 + step * 48}></line>`)}
          ${this.series.map((series, seriesIndex) => svg`<g class=${`series-${seriesIndex % 5}`}><polyline class="line" points=${this.#points(series)}></polyline>${this.categories.map((_, index) => Number.isFinite(series.values[index]) ? svg`<circle class="point" cx=${this.#x(index)} cy=${this.#y(series.values[index] as number)} r="2.5"></circle>` : nothing)}</g>`)}
        </svg>
        ${this.categories.map((category, index) => html`<button class="target" data-index=${index} style=${`inset-inline-start: ${(this.#x(index) / 600) * 100}%`} aria-label=${this.#categoryLabel(index)} @focus=${() => { this.activeIndex = index; }} @mouseenter=${() => { this.activeIndex = index; }} @keydown=${(event: KeyboardEvent) => this.#key(event, index)}></button>`)}
        <bm-chart-tooltip style=${active >= 0 ? `inset-inline-start: ${(this.#x(active) / 600) * 100}%; inset-block-start: ${this.#y(this.series[0]?.values[active] ?? 0)}px` : ''} heading=${active >= 0 ? this.categories[active] : ''} .rows=${active >= 0 ? this.#rows(active) : []} ?open=${active >= 0}></bm-chart-tooltip>
      </div>
      <ul class="legend" part="legend">${this.series.map((series, index) => html`<li class=${`series-${index % 5}`}><span class="legend-dot" aria-hidden="true"></span>${series.label}</li>`)}</ul>
      <div class="alternative" part="alternative"><slot name="table"></slot></div>
    </section>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-chart': BoldMergeChart; 'bm-chart-tooltip': BoldMergeChartTooltip; }
}
