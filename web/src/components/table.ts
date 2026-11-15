import { LitElement, css, html, type PropertyValues } from 'lit';

export type TableState = 'ready' | 'loading' | 'empty';
export type TableSortDirection = 'none' | 'ascending' | 'descending';

export interface TableSortDetail { key: string; direction: Exclude<TableSortDirection, 'none'>; originalEvent: Event; }
export interface TableRowActivateDetail { key: string; row: HTMLTableRowElement; originalEvent: Event; }
export interface TableSelectionChangeDetail { selectedKeys: string[]; allSelected: boolean; originalEvent: Event; }

const interactiveSelector = 'a,button,input,select,textarea,summary,[contenteditable="true"],[data-bm-row-action]';

export class BoldMergeTable extends LitElement {
  static properties = {
    state: { type: String, reflect: true },
    sortKey: { type: String, attribute: 'sort-key', reflect: true },
    sortDirection: { type: String, attribute: 'sort-direction', reflect: true },
    rowActivation: { type: Boolean, attribute: 'row-activation', reflect: true },
    loadingLabel: { type: String, attribute: 'loading-label' },
    emptyLabel: { type: String, attribute: 'empty-label' },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .viewport { overflow: auto; inline-size: 100%; }
    .state { display: grid; min-block-size: calc(var(--bm-table-row-height) * 3); place-items: center; padding: var(--bm-size-24); color: var(--bm-color-text-secondary); background: var(--bm-color-surface); border-block-end: var(--bm-table-divider-width) solid var(--bm-color-border); text-align: center; }
  `;

  readonly #observer = new MutationObserver(() => this.#sync());
  state: TableState = 'ready';
  sortKey = '';
  sortDirection: TableSortDirection = 'none';
  rowActivation = false;
  loadingLabel = 'Loading data';
  emptyLabel = 'No data';

  constructor() {
    super();
    this.addEventListener('click', this.#click);
    this.addEventListener('input', this.#input);
    this.addEventListener('change', this.#change);
    this.addEventListener('keydown', this.#keydown);
  }

  override connectedCallback(): void {
    super.connectedCallback();
    this.#observer.observe(this, { subtree: true, childList: true, attributes: true, attributeFilter: ['data-bm-row-key', 'data-bm-sort-key', 'disabled', 'checked'] });
  }

  override disconnectedCallback(): void { this.#observer.disconnect(); super.disconnectedCallback(); }

  protected override updated(changed: PropertyValues<this>): void {
    if (changed.has('sortKey') || changed.has('sortDirection') || changed.has('rowActivation') || changed.has('state')) this.#sync();
  }

  get selectedKeys(): string[] {
    return this.#rowCheckboxes().filter((checkbox) => checkbox.checked).map((checkbox) => checkbox.closest<HTMLTableRowElement>('tr[data-bm-row-key]')?.dataset.bmRowKey ?? checkbox.value).filter(Boolean);
  }

  #table(): HTMLTableElement | null { return this.querySelector(':scope > table'); }
  #rows(): HTMLTableRowElement[] { return [...(this.#table()?.querySelectorAll<HTMLTableRowElement>('tbody tr[data-bm-row-key]') ?? [])]; }
  #rowCheckboxes(): HTMLInputElement[] { return this.#rows().map((row) => row.querySelector<HTMLInputElement>('input[type="checkbox"][data-bm-select]')).filter((item): item is HTMLInputElement => Boolean(item)); }
  #selectAll(): HTMLInputElement | null { return this.#table()?.querySelector<HTMLInputElement>('thead input[type="checkbox"][data-bm-select-all]') ?? null; }

  #sync(): void {
    const table = this.#table();
    if (!table) return;
    table.setAttribute('aria-busy', String(this.state === 'loading'));
    for (const header of table.querySelectorAll<HTMLTableCellElement>('th[aria-sort]')) header.setAttribute('aria-sort', 'none');
    if (this.sortKey && this.sortDirection !== 'none') {
      const button = [...table.querySelectorAll<HTMLElement>('[data-bm-sort-key]')].find((item) => item.dataset.bmSortKey === this.sortKey);
      button?.closest('th')?.setAttribute('aria-sort', this.sortDirection);
    }
    for (const row of this.#rows()) {
      if (this.rowActivation) { row.tabIndex = 0; row.setAttribute('data-bm-activatable', ''); }
      else { row.removeAttribute('tabindex'); row.removeAttribute('data-bm-activatable'); }
    }
    this.#syncSelectAll();
  }

  #syncSelectAll(): void {
    const control = this.#selectAll();
    if (!control) return;
    const enabled = this.#rowCheckboxes().filter((checkbox) => !checkbox.disabled);
    const selected = enabled.filter((checkbox) => checkbox.checked).length;
    control.checked = enabled.length > 0 && selected === enabled.length;
    control.indeterminate = selected > 0 && selected < enabled.length;
    control.disabled = enabled.length === 0;
  }

  #owns(target: Element): boolean { return target.closest('bm-table') === this; }

  #click = (event: MouseEvent): void => {
    const target = event.composedPath()[0];
    if (!(target instanceof Element) || !this.#owns(target)) return;
    const sort = target.closest<HTMLElement>('[data-bm-sort-key]');
    if (sort) {
      const key = sort.dataset.bmSortKey ?? '';
      if (!key) return;
      const direction = this.sortKey === key && this.sortDirection === 'ascending' ? 'descending' : 'ascending';
      this.sortKey = key; this.sortDirection = direction;
      this.dispatchEvent(new CustomEvent<TableSortDetail>('bm-table-sort', { detail: { key, direction, originalEvent: event }, bubbles: true, composed: true }));
      return;
    }
    if (!this.rowActivation || target.closest(interactiveSelector)) return;
    const row = target.closest<HTMLTableRowElement>('tbody tr[data-bm-row-key]');
    if (row) this.#activate(row, event);
  };

  #keydown = (event: KeyboardEvent): void => {
    if (!this.rowActivation || (event.key !== 'Enter' && event.key !== ' ')) return;
    const target = event.composedPath()[0];
    if (!(target instanceof HTMLTableRowElement) || !this.#owns(target) || !target.matches('tbody tr[data-bm-row-key]')) return;
    event.preventDefault(); this.#activate(target, event);
  };

  #activate(row: HTMLTableRowElement, originalEvent: Event): void {
    const key = row.dataset.bmRowKey ?? '';
    if (key) this.dispatchEvent(new CustomEvent<TableRowActivateDetail>('bm-table-row-activate', { detail: { key, row, originalEvent }, bubbles: true, composed: true }));
  }

  #input = (event: Event): void => {
    const target = event.composedPath()[0];
    if (!(target instanceof HTMLInputElement) || target.type !== 'checkbox' || !target.matches('[data-bm-select-all]') || !this.#owns(target)) return;
    for (const checkbox of this.#rowCheckboxes()) if (!checkbox.disabled) checkbox.checked = target.checked;
    this.#emitSelection(event);
  };

  #change = (event: Event): void => {
    const target = event.composedPath()[0];
    if (!(target instanceof HTMLInputElement) || target.type !== 'checkbox' || !target.matches('[data-bm-select]') || !this.#owns(target)) return;
    this.#emitSelection(event);
  };

  #emitSelection(originalEvent: Event): void {
    this.#syncSelectAll();
    const enabled = this.#rowCheckboxes().filter((checkbox) => !checkbox.disabled);
    this.dispatchEvent(new CustomEvent<TableSelectionChangeDetail>('bm-table-selection-change', { detail: { selectedKeys: this.selectedKeys, allSelected: enabled.length > 0 && enabled.every((checkbox) => checkbox.checked), originalEvent }, bubbles: true, composed: true }));
  }

  override render() {
    if (this.state === 'loading') return html`<div class="state" part="state loading" role="status"><slot name="loading">${this.loadingLabel}</slot></div>`;
    if (this.state === 'empty') return html`<div class="state" part="state empty"><slot name="empty">${this.emptyLabel}</slot></div>`;
    return html`<div class="viewport" part="viewport"><slot @slotchange=${this.#sync}></slot></div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-table': BoldMergeTable; }
  interface HTMLElementEventMap {
    'bm-table-sort': CustomEvent<TableSortDetail>;
    'bm-table-row-activate': CustomEvent<TableRowActivateDetail>;
    'bm-table-selection-change': CustomEvent<TableSelectionChangeDetail>;
  }
}
