import { LitElement, css, html, nothing } from 'lit';

export class BoldMergeCard extends LitElement {
  static properties = {
    label: { type: String },
    _hasHeader: { state: true },
    _hasActions: { state: true },
    _hasFooter: { state: true },
  };

  static styles = css`
    :host {
      display: block;
      color: var(--bm-color-text-primary);
      font-family: var(--bm-typography-family-ui);
    }

    article {
      display: grid;
      gap: var(--bm-card-section-gap);
      box-sizing: border-box;
      padding: var(--bm-card-padding);
      background: var(--bm-color-surface);
      border: var(--bm-card-border-width) solid transparent;
      border-radius: var(--bm-card-radius);
      box-shadow: none;
    }

    .header {
      display: flex;
      align-items: start;
      justify-content: space-between;
      gap: var(--bm-size-8);
      min-inline-size: 0;
    }

    .header[hidden],
    .footer[hidden] {
      display: none;
    }

    .content {
      min-inline-size: 0;
    }

    slot[name='actions'] {
      flex: none;
    }
  `;

  label = '';
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
      <article part="card" aria-label=${this.label || nothing}>
        <div class="header" part="header" ?hidden=${!hasTop}>
          <slot name="header" @slotchange=${(event: Event) => this.#assigned(event, '_hasHeader')}></slot>
          <span part="actions"><slot name="actions" @slotchange=${(event: Event) => this.#assigned(event, '_hasActions')}></slot></span>
        </div>
        <div class="content" part="content"><slot></slot></div>
        <div class="footer" part="footer" ?hidden=${!this._hasFooter}>
          <slot name="footer" @slotchange=${(event: Event) => this.#assigned(event, '_hasFooter')}></slot>
        </div>
      </article>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'bm-card': BoldMergeCard;
  }
}
