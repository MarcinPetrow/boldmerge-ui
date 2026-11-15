import { LitElement, css, html } from 'lit';

export type PageLayoutVariant = 'fluid' | 'contained' | 'split';

export class BoldMergePageLayout extends LitElement {
  static properties = {
    layout: { type: String, reflect: true },
    _hasHeader: { state: true },
    _hasAside: { state: true },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .layout { display: grid; gap: var(--bm-layout-gap); inline-size: 100%; margin-inline: auto; }
    :host([layout='contained']) .layout, :host([layout='split']) .layout { max-inline-size: var(--bm-page-layout-content-max-width); }
    .body { min-inline-size: 0; }
    :host([layout='split']) .body { display: grid; grid-template-columns: minmax(0, 1fr) var(--bm-page-layout-split-aside-width); gap: var(--bm-layout-gap); align-items: start; }
    header[hidden], aside[hidden] { display: none; }
    .content, aside { min-inline-size: 0; }
  `;

  layout: PageLayoutVariant = 'fluid';
  protected _hasHeader = false;
  protected _hasAside = false;

  #assigned(event: Event, property: '_hasHeader' | '_hasAside'): void {
    const slot = event.currentTarget as HTMLSlotElement;
    this[property] = slot.assignedNodes({ flatten: true }).some((node) => node.nodeType !== Node.TEXT_NODE || Boolean(node.textContent?.trim()));
  }

  override render() {
    const showAside = this.layout === 'split' && this._hasAside;
    return html`
      <div class="layout" part="layout">
        <header part="header" ?hidden=${!this._hasHeader}><slot name="header" @slotchange=${(event: Event) => this.#assigned(event, '_hasHeader')}></slot></header>
        <div class="body" part="body">
          <div class="content" part="content"><slot></slot></div>
          <aside part="aside" ?hidden=${!showAside}><slot name="aside" @slotchange=${(event: Event) => this.#assigned(event, '_hasAside')}></slot></aside>
        </div>
      </div>
    `;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-page-layout': BoldMergePageLayout; } }
