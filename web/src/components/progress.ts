import { LitElement, css, html, nothing } from 'lit';

export class BoldMergeProgress extends LitElement {
  static properties = {
    value: { type: Number }, min: { type: Number }, max: { type: Number },
    segments: { type: Number }, label: { type: String },
    valueLabel: { type: String, attribute: 'value-label' },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .header { display: flex; justify-content: space-between; gap: var(--bm-size-12); margin-block-end: var(--bm-size-8); line-height: var(--bm-line-height-compact); }
    .label { color: var(--bm-color-text-secondary); }
    .value { font-family: var(--bm-typography-family-data); font-variant-numeric: tabular-nums; unicode-bidi: isolate; }
    .track { display: grid; grid-template-columns: repeat(var(--bm-progress-segments), minmax(0, 1fr)); gap: var(--bm-progress-gap); }
    .segment { block-size: var(--bm-progress-height); background: var(--bm-color-surface-subtle); border-radius: var(--bm-progress-radius); transition: background-color var(--bm-motion-duration-interaction) var(--bm-motion-easing-standard); }
    .segment[data-filled] { background: var(--bm-color-accent-solid); }
    @media (forced-colors: active) {
      .segment { background: Canvas; border: 1px solid GrayText; }
      .segment[data-filled] { background: CanvasText; border-color: CanvasText; forced-color-adjust: none; }
    }
  `;

  value = 0;
  min = 0;
  max = 100;
  segments = 20;
  label = '';
  valueLabel = '';

  get normalizedValue(): number {
    const min = Number.isFinite(this.min) ? this.min : 0;
    const max = Number.isFinite(this.max) && this.max > min ? this.max : min + 1;
    return Math.min(max, Math.max(min, Number.isFinite(this.value) ? this.value : min));
  }

  override render() {
    const min = Number.isFinite(this.min) ? this.min : 0;
    const max = Number.isFinite(this.max) && this.max > min ? this.max : min + 1;
    const value = this.normalizedValue;
    const count = Math.min(100, Math.max(1, Math.trunc(this.segments) || 20));
    const filled = Math.round(((value - min) / (max - min)) * count);
    return html`<div class="header" part="header" ?hidden=${!this.label && !this.valueLabel}>
      <span class="label" part="label">${this.label}</span><bdi class="value" part="value">${this.valueLabel}</bdi>
    </div><div class="track" part="track" role="progressbar" aria-label=${this.label || nothing} aria-valuemin=${min} aria-valuemax=${max} aria-valuenow=${value} aria-valuetext=${this.valueLabel || nothing} style=${`--bm-progress-segments: ${count}`}>
      ${Array.from({ length: count }, (_, index) => html`<span class="segment" part="segment ${index < filled ? 'filled' : 'empty'}" ?data-filled=${index < filled} aria-hidden="true"></span>`)}
    </div>`;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-progress': BoldMergeProgress; } }
