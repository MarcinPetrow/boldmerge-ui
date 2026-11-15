import { LitElement, css, html, nothing } from 'lit';

export type KpiChangeTone = 'neutral' | 'favorable' | 'unfavorable';

export class BoldMergeKpi extends LitElement {
  static properties = {
    label: { type: String }, value: { type: String }, comparison: { type: String },
    change: { type: String }, changeTone: { type: String, attribute: 'change-tone', reflect: true },
    sparkline: { attribute: false },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .kpi { display: grid; grid-template-columns: minmax(0, 1fr) minmax(96px, 36%); align-items: end; gap: var(--bm-size-16); }
    .copy { min-inline-size: 0; }
    .label { color: var(--bm-color-text-secondary); line-height: var(--bm-line-height-compact); }
    .value { display: block; margin-block-start: var(--bm-size-4); font-family: var(--bm-typography-family-data); font-size: 2rem; font-weight: var(--bm-typography-weight-emphasis); font-variant-numeric: tabular-nums; line-height: 1.1; unicode-bidi: isolate; }
    .context { display: flex; flex-wrap: wrap; gap: var(--bm-size-4) var(--bm-size-8); margin-block-start: var(--bm-size-8); color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); line-height: var(--bm-line-height-compact); }
    .change { font-weight: var(--bm-typography-weight-emphasis); }
    :host([change-tone='favorable']) .change { color: var(--bm-color-success); }
    :host([change-tone='unfavorable']) .change { color: var(--bm-color-error); }
    svg { inline-size: 100%; block-size: 48px; overflow: visible; color: var(--bm-color-accent-text); }
    polyline { fill: none; stroke: currentColor; stroke-width: var(--bm-kpi-sparkline-width); stroke-linecap: round; stroke-linejoin: round; vector-effect: non-scaling-stroke; }
  `;

  label = '';
  value = '';
  comparison = '';
  change = '';
  changeTone: KpiChangeTone = 'neutral';
  sparkline: readonly number[] = [];

  #points(): string {
    const values = this.sparkline.filter(Number.isFinite);
    if (values.length < 2) return '';
    const low = Math.min(...values);
    const range = Math.max(...values) - low || 1;
    return values.map((item, index) => `${(index / (values.length - 1)) * 100},${44 - ((item - low) / range) * 40}`).join(' ');
  }

  override render() {
    const points = this.#points();
    return html`<article class="kpi" part="kpi" aria-label=${this.label || nothing}>
      <div class="copy" part="copy"><div class="label" part="label">${this.label}</div><bdi class="value" part="value">${this.value}</bdi>
        <div class="context" part="context" ?hidden=${!this.change && !this.comparison}><bdi class="change" part="change">${this.change}</bdi><span part="comparison">${this.comparison}</span></div>
      </div>
      ${points ? html`<svg part="sparkline" viewBox="0 0 100 48" preserveAspectRatio="none" aria-hidden="true"><polyline points=${points}></polyline></svg>` : nothing}
    </article>`;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-kpi': BoldMergeKpi; } }
