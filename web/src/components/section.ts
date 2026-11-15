import { LitElement, css, html, nothing } from 'lit';

export class BoldMergeSection extends LitElement {
  static properties = {
    label: { type: String },
    separator: { type: Boolean, reflect: true },
    _hasHeader: { state: true },
    _hasActions: { state: true },
    _hasFooter: { state: true },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    section { display: grid; gap: var(--bm-section-gap); min-inline-size: 0; }
    :host([separator]) section { padding-block-end: var(--bm-section-gap); border-block-end: var(--bm-size-1) solid var(--bm-color-border); }
    .header { display: flex; align-items: start; justify-content: space-between; gap: var(--bm-size-8); min-inline-size: 0; }
    .header[hidden], .footer[hidden] { display: none; }
    .content { min-inline-size: 0; }
    slot[name='actions'] { flex: none; }
  `;

  label = '';
  separator = false;
  protected _hasHeader = false;
  protected _hasActions = false;
  protected _hasFooter = false;

  #assigned(event: Event, property: '_hasHeader' | '_hasActions' | '_hasFooter'): void {
    const slot = event.currentTarget as HTMLSlotElement;
    this[property] = slot.assignedNodes({ flatten: true }).some((node) => node.nodeType !== Node.TEXT_NODE || Boolean(node.textContent?.trim()));
  }

  override render() {
    const hasTop = this._hasHeader || this._hasActions;
    return html`
      <section part="section" aria-label=${this.label || nothing}>
        <div class="header" part="header" ?hidden=${!hasTop}>
          <slot name="header" @slotchange=${(event: Event) => this.#assigned(event, '_hasHeader')}></slot>
          <span part="actions"><slot name="actions" @slotchange=${(event: Event) => this.#assigned(event, '_hasActions')}></slot></span>
        </div>
        <div class="content" part="content"><slot></slot></div>
        <div class="footer" part="footer" ?hidden=${!this._hasFooter}><slot name="footer" @slotchange=${(event: Event) => this.#assigned(event, '_hasFooter')}></slot></div>
      </section>
    `;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-section': BoldMergeSection; } }
