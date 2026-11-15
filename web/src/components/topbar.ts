import { LitElement, css, html } from 'lit';

export class BoldMergeTopbar extends LitElement {
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    header { display: flex; align-items: center; justify-content: space-between; gap: var(--bm-size-12); box-sizing: border-box; min-block-size: calc(var(--bm-button-height) + var(--bm-size-16)); padding-inline: var(--bm-size-16); background: var(--bm-color-surface); border-block-end: var(--bm-size-1) solid var(--bm-color-border); }
    .start, .actions { display: flex; align-items: center; gap: var(--bm-size-8); min-inline-size: 0; }
    .actions { margin-inline-start: auto; }
  `;
  override render() { return html`<header part="topbar"><div class="start" part="start"><slot name="start"></slot><slot></slot></div><div class="actions" part="actions"><slot name="search"></slot><slot name="notifications"></slot><slot name="account"></slot></div></header>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-topbar': BoldMergeTopbar; } }
