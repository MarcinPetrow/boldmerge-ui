import { LitElement, css, html } from 'lit';

export type StatusTone = 'neutral' | 'accent' | 'success' | 'warning' | 'error';

export class BoldMergeStatus extends LitElement {
  static properties = { tone: { type: String, reflect: true } };
  static styles = css`
    :host { display: inline-block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    span { display: inline-flex; align-items: center; gap: var(--bm-size-8); line-height: var(--bm-line-height-compact); }
    i { flex: none; inline-size: var(--bm-status-dot-size); block-size: var(--bm-status-dot-size); background: var(--bm-color-text-muted); border-radius: 50%; }
    :host([tone='accent']) i { background: var(--bm-color-accent-solid); }
    :host([tone='success']) i { background: var(--bm-color-success); }
    :host([tone='warning']) i { background: var(--bm-color-warning); }
    :host([tone='error']) i { background: var(--bm-color-error); }
  `;
  tone: StatusTone = 'neutral';
  override render() { return html`<span part="status"><i part="dot" aria-hidden="true"></i><span part="label"><slot></slot></span></span>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-status': BoldMergeStatus; } }
