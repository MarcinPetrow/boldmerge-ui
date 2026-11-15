import { LitElement, css, html, nothing } from 'lit';

export class BoldMergeBreadcrumbItem extends LitElement {
  static properties = {
    href: { type: String, reflect: true },
    current: { type: Boolean, reflect: true },
    _first: { state: true },
  };

  static styles = css`
    :host { display: inline-flex; align-items: center; min-inline-size: 0; color: var(--bm-color-text-secondary); font-family: var(--bm-typography-family-ui); }
    .separator { margin-inline: var(--bm-size-8); color: var(--bm-color-text-muted); }
    :host([data-first]) .separator { display: none; }
    a { color: var(--bm-color-text-secondary); text-decoration: none; }
    a:hover { color: var(--bm-color-accent-text); text-decoration: underline; text-underline-offset: var(--bm-size-2); }
    a:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); border-radius: var(--bm-radius-2); }
    [aria-current='page'] { overflow: hidden; color: var(--bm-color-text-primary); font-weight: var(--bm-typography-weight-emphasis); text-overflow: ellipsis; white-space: nowrap; }
  `;

  href = '';
  current = false;
  protected _first = false;

  override connectedCallback(): void {
    super.connectedCallback();
    if (!this.hasAttribute('role')) this.setAttribute('role', 'listitem');
  }

  /** @internal Used by bm-breadcrumbs to hide the first separator. */
  setFirst(first: boolean): void { this._first = first; this.toggleAttribute('data-first', first); }

  override render() {
    const content = this.current
      ? html`<span part="link" aria-current="page"><slot></slot></span>`
      : this.href
        ? html`<a part="link" href=${this.href}><slot></slot></a>`
        : html`<span part="link"><slot></slot></span>`;
    return html`<span class="separator" part="separator" aria-hidden="true">/</span>${content}`;
  }
}

export class BoldMergeBreadcrumbs extends LitElement {
  static properties = { label: { type: String } };
  static styles = css`
    :host { display: block; min-inline-size: 0; }
    nav { min-inline-size: 0; }
    .list { display: flex; flex-wrap: wrap; align-items: center; min-inline-size: 0; margin: 0; padding: 0; }
  `;

  label = '';

  #slotChange(event: Event): void {
    const items = (event.currentTarget as HTMLSlotElement).assignedElements({ flatten: true }).filter((element): element is BoldMergeBreadcrumbItem => element instanceof BoldMergeBreadcrumbItem);
    items.forEach((item, index) => item.setFirst(index === 0));
  }

  override render() {
    return html`<nav part="nav" aria-label=${this.label || nothing}><div class="list" part="list" role="list"><slot @slotchange=${this.#slotChange}></slot></div></nav>`;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-breadcrumb-item': BoldMergeBreadcrumbItem; 'bm-breadcrumbs': BoldMergeBreadcrumbs; } }
