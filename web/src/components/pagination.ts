import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export interface PaginationChangeDetail {
  page: number;
  pageSize: number;
  totalItems: number;
  originalEvent?: Event;
}

type PageItem = number | 'start-ellipsis' | 'end-ellipsis';

export class BoldMergePagination extends LitElement {
  static properties = {
    page: { type: Number, reflect: true },
    pageSize: { type: Number, attribute: 'page-size', reflect: true },
    totalItems: { type: Number, attribute: 'total-items', reflect: true },
    siblingCount: { type: Number, attribute: 'sibling-count' },
    disabled: { type: Boolean, reflect: true },
    label: { type: String },
    previousLabel: { type: String, attribute: 'previous-label' },
    nextLabel: { type: String, attribute: 'next-label' },
    pageLabel: { type: String, attribute: 'page-label' },
    ofLabel: { type: String, attribute: 'of-label' },
  };

  static styles = css`
    :host { display: block; min-inline-size: 0; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    nav { display: flex; flex-wrap: wrap; align-items: center; min-inline-size: 0; gap: var(--bm-size-12); }
    .pages { display: flex; align-items: center; min-inline-size: 0; max-inline-size: 100%; margin: 0; padding: 0; overflow-x: auto; list-style: none; }
    button { box-sizing: border-box; min-inline-size: var(--bm-button-height); block-size: var(--bm-button-height); padding-inline: var(--bm-size-8); color: var(--bm-color-text-primary); font: inherit; font-weight: var(--bm-typography-weight-emphasis); background: transparent; border: var(--bm-button-border-width) solid transparent; border-radius: var(--bm-button-radius); cursor: pointer; transition: background-color var(--bm-motion-duration-interaction), color var(--bm-motion-duration-interaction), border-color var(--bm-motion-duration-interaction); }
    button:hover:not(:disabled) { background: color-mix(in oklch, var(--bm-color-text-primary) var(--bm-button-state-hover-mix), transparent); }
    button:active:not(:disabled) { background: color-mix(in oklch, var(--bm-color-text-primary) var(--bm-button-state-press-mix), transparent); }
    button:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    button[aria-current='page'] { color: var(--bm-color-accent-on-solid); background: var(--bm-color-accent-solid); }
    button:disabled { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    .ellipsis { display: grid; min-inline-size: var(--bm-button-height); block-size: var(--bm-button-height); place-items: center; color: var(--bm-color-text-secondary); }
    .summary { color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); white-space: nowrap; }
    ::slotted([slot='page-size']) { margin-inline-start: auto; }
    @media (pointer: coarse) { button { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { button { transition: none; } }
  `;

  page = 1;
  pageSize = 25;
  totalItems = 0;
  siblingCount = 1;
  disabled = false;
  label = 'Pagination';
  previousLabel = 'Previous';
  nextLabel = 'Next';
  pageLabel = 'Page';
  ofLabel = 'of';

  get pageCount(): number {
    return Math.max(1, Math.ceil(Math.max(0, this.totalItems) / Math.max(1, this.pageSize)));
  }

  protected override willUpdate(changed: PropertyValues<this>): void {
    if (changed.has('pageSize') && (!Number.isFinite(this.pageSize) || this.pageSize < 1)) this.pageSize = 25;
    if (changed.has('totalItems') && (!Number.isFinite(this.totalItems) || this.totalItems < 0)) this.totalItems = 0;
    if (changed.has('siblingCount') && (!Number.isFinite(this.siblingCount) || this.siblingCount < 0)) this.siblingCount = 1;
    const normalized = Math.min(this.pageCount, Math.max(1, Math.trunc(this.page) || 1));
    if (normalized !== this.page) this.page = normalized;
  }

  goToPage(page: number, originalEvent?: Event): boolean {
    if (this.disabled) return false;
    const next = Math.min(this.pageCount, Math.max(1, Math.trunc(page) || 1));
    if (next === this.page) return false;
    this.page = next;
    this.dispatchEvent(new CustomEvent<PaginationChangeDetail>('bm-page-change', {
      detail: { page: next, pageSize: this.pageSize, totalItems: this.totalItems, originalEvent },
      bubbles: true,
      composed: true,
    }));
    return true;
  }

  next(originalEvent?: Event): boolean { return this.goToPage(this.page + 1, originalEvent); }
  previous(originalEvent?: Event): boolean { return this.goToPage(this.page - 1, originalEvent); }

  #items(): PageItem[] {
    const count = this.pageCount;
    const siblings = Math.max(0, Math.trunc(this.siblingCount));
    if (count <= siblings * 2 + 5) return Array.from({ length: count }, (_, index) => index + 1);
    const start = Math.max(2, this.page - siblings);
    const end = Math.min(count - 1, this.page + siblings);
    const items: PageItem[] = [1];
    if (start > 2) items.push('start-ellipsis');
    for (let value = start; value <= end; value += 1) items.push(value);
    if (end < count - 1) items.push('end-ellipsis');
    items.push(count);
    return items;
  }

  #pageButton(page: number) {
    const current = page === this.page;
    return html`<li><button part=${current ? 'page current' : 'page'} aria-current=${current ? 'page' : nothing} aria-label=${`${this.pageLabel} ${page}`} @click=${(event: MouseEvent) => this.goToPage(page, event)}>${page}</button></li>`;
  }

  override render() {
    return html`
      <nav part="nav" aria-label=${this.label}>
        <button part="previous" ?disabled=${this.disabled || this.page <= 1} @click=${(event: MouseEvent) => this.previous(event)}>${this.previousLabel}</button>
        <ol class="pages" part="pages">
          ${this.#items().map((item) => typeof item === 'number' ? this.#pageButton(item) : html`<li class="ellipsis" part="ellipsis" aria-hidden="true">…</li>`)}
        </ol>
        <button part="next" ?disabled=${this.disabled || this.page >= this.pageCount} @click=${(event: MouseEvent) => this.next(event)}>${this.nextLabel}</button>
        <span class="summary" part="summary" aria-live="polite">${this.pageLabel} <bdi>${this.page}</bdi> ${this.ofLabel} <bdi>${this.pageCount}</bdi></span>
        <slot name="page-size" part="page-size"></slot>
      </nav>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-pagination': BoldMergePagination; }
  interface HTMLElementEventMap { 'bm-page-change': CustomEvent<PaginationChangeDetail>; }
}
