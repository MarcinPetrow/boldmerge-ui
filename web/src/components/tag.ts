import { LitElement, css, html } from 'lit';

export class BoldMergeTag extends LitElement {
  static styles = css`
    :host { display: inline-block; color: var(--bm-color-accent-on-subtle); font-family: var(--bm-tag-font-family); font-size: var(--bm-tag-font-size); }
    span { display: inline-flex; align-items: center; gap: var(--bm-size-4); box-sizing: border-box; padding: var(--bm-tag-padding-block) var(--bm-tag-padding-inline); background: var(--bm-color-accent-subtle); border-radius: var(--bm-tag-radius); line-height: var(--bm-line-height-compact); }
    ::slotted([slot='prefix']), ::slotted([slot='suffix']) { inline-size: var(--bm-icon-size-small); block-size: var(--bm-icon-size-small); }
  `;
  override render() { return html`<span part="tag"><slot name="prefix"></slot><span part="label"><slot></slot></span><slot name="suffix"></slot></span>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-tag': BoldMergeTag; } }
