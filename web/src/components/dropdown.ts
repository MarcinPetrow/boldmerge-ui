import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export type DropdownPlacement = 'auto' | 'bottom-start' | 'bottom-end' | 'top-start' | 'top-end';
export interface DropdownOpenChangeDetail { open: boolean; originalEvent?: Event; }
export interface DropdownSelectDetail { value: string; href: string; originalEvent: Event; }

let dropdownSequence = 0;

export class BoldMergeDropdownItem extends LitElement {
  static properties = { value: { type: String, reflect: true }, href: { type: String, reflect: true }, disabled: { type: Boolean, reflect: true } };
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .item { display: flex; align-items: center; box-sizing: border-box; inline-size: 100%; min-inline-size: max-content; block-size: var(--bm-menu-item-height); padding-inline: var(--bm-size-12); gap: var(--bm-dropdown-item-gap); color: inherit; background: transparent; border: 0; border-radius: var(--bm-radius-6); font: inherit; text-align: start; text-decoration: none; cursor: pointer; outline: none; white-space: nowrap; }
    .item:hover, .item:focus-visible { background: var(--bm-color-surface-subtle); }
    .item:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: calc(-1 * var(--bm-size-2)); }
    :host([disabled]) { opacity: var(--bm-content-opacity-disabled); }
    :host([disabled]) .item { cursor: not-allowed; }
    .label { flex: 1; }
    .prefix, .suffix { display: inline-grid; place-items: center; flex: none; min-inline-size: var(--bm-icon-size-small); }
    @media (pointer: coarse) { .item { min-block-size: var(--bm-interaction-target-minimum); } }
  `;
  value = '';
  href = '';
  disabled = false;
  get accessibleLabel(): string {
    const slot = this.renderRoot.querySelector<HTMLSlotElement>('slot:not([name])');
    return slot?.assignedNodes({ flatten: true }).map((node) => node.textContent ?? '').join(' ').replace(/\s+/g, ' ').trim()
      ?? this.textContent?.replace(/\s+/g, ' ').trim()
      ?? '';
  }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector<HTMLElement>('.item')?.focus(options); }
  setTabStop(active: boolean): void { const item = this.renderRoot.querySelector<HTMLElement>('.item'); if (item) item.tabIndex = active && !this.disabled ? 0 : -1; }
  #activate(event: Event): void {
    if (this.disabled) { event.preventDefault(); event.stopImmediatePropagation(); return; }
    this.dispatchEvent(new CustomEvent('bm-dropdown-item-request', { detail: { originalEvent: event }, bubbles: true, composed: true }));
  }
  #keyDown(event: KeyboardEvent): void { if (event.key === ' ' && this.href && !this.disabled) { event.preventDefault(); this.renderRoot.querySelector<HTMLElement>('.item')?.click(); } }
  override render() {
    const content = html`<span class="prefix" part="prefix"><slot name="prefix"></slot></span><span class="label" part="label"><slot></slot></span><span class="suffix" part="suffix"><slot name="suffix"></slot></span>`;
    return this.href
      ? html`<a class="item" part="item" role="menuitem" href=${this.disabled ? nothing : this.href} aria-disabled=${this.disabled ? 'true' : 'false'} tabindex="-1" @click=${this.#activate} @keydown=${this.#keyDown}>${content}</a>`
      : html`<button class="item" part="item" role="menuitem" type="button" ?disabled=${this.disabled} aria-disabled=${this.disabled ? 'true' : 'false'} tabindex="-1" @click=${this.#activate}>${content}</button>`;
  }
}

export class BoldMergeDropdownGroup extends LitElement {
  static properties = { label: { type: String }, destructive: { type: Boolean, reflect: true } };
  static styles = css`
    :host { display: block; padding-block: var(--bm-size-2); }
    :host([destructive]) { margin-block-start: var(--bm-size-4); padding-block-start: var(--bm-size-4); border-block-start: var(--bm-size-1) solid var(--bm-color-border); color: var(--bm-color-error); }
  `;
  label = '';
  destructive = false;
  override render() { return html`<div part="group" role="group" aria-label=${this.label || nothing}><slot></slot></div>`; }
}

export class BoldMergeDropdown extends LitElement {
  static properties = {
    label: { type: String },
    open: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
    placement: { type: String, reflect: true },
  };
  static styles = css`
    :host { display: inline-block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .trigger { display: inline-flex; align-items: center; justify-content: center; box-sizing: border-box; block-size: var(--bm-button-height); padding-inline: var(--bm-button-padding-inline); gap: var(--bm-button-content-gap); color: var(--bm-color-accent-text); background: transparent; border: var(--bm-button-border-width) solid var(--bm-color-border-strong); border-radius: var(--bm-button-radius); font: inherit; font-weight: var(--bm-typography-weight-emphasis); cursor: pointer; }
    .trigger:hover:not(:disabled) { background: var(--bm-color-surface-subtle); }
    .trigger:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    .trigger:disabled { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    .indicator { font-size: var(--bm-font-size-12); transition: transform var(--bm-dropdown-transition-duration) var(--bm-motion-easing-standard); }
    :host([open]) .indicator { transform: rotate(180deg); }
    .menu { position: fixed; z-index: var(--bm-overlay-stack-level); display: none; box-sizing: border-box; overflow: auto; max-inline-size: calc(100vw - var(--bm-size-16)); max-block-size: calc(100vh - var(--bm-size-32)); padding-block: var(--bm-dropdown-padding-block); color: var(--bm-color-text-primary); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-border); border-radius: var(--bm-dropdown-radius); box-shadow: var(--bm-dropdown-shadow); outline: none; }
    :host([open]) .menu { display: block; animation: reveal var(--bm-dropdown-transition-duration) var(--bm-motion-easing-standard); }
    @keyframes reveal { from { opacity: 0; transform: translateY(calc(-1 * var(--bm-size-4))); } }
    @media (prefers-reduced-motion: reduce) { .indicator { transition: none; } :host([open]) .menu { animation: none; } }
    @media (pointer: coarse) { .trigger { min-block-size: var(--bm-interaction-target-minimum); } }
  `;
  label = '';
  open = false;
  disabled = false;
  placement: DropdownPlacement = 'auto';
  readonly #menuId = `bm-dropdown-menu-${++dropdownSequence}`;
  readonly #observer = new MutationObserver(() => this.#syncItems());
  readonly #resizeObserver = new ResizeObserver(() => this.#position());
  #activeIndex = 0;
  #typeahead = '';
  #typeaheadTimer?: number;

  constructor() {
    super();
    this.addEventListener('bm-dropdown-item-request', this.#itemRequest as EventListener);
    this.addEventListener('keydown', this.#menuKeyDown);
  }
  override connectedCallback(): void { super.connectedCallback(); this.#observer.observe(this, { childList: true, subtree: true, attributes: true, attributeFilter: ['disabled'] }); }
  override disconnectedCallback(): void { this.#observer.disconnect(); this.#stopPositioning(); this.#clearTypeahead(); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void {
    if (changed.has('open') || changed.has('placement')) {
      if (this.open) { this.#startPositioning(); this.#syncItems(); }
      else this.#stopPositioning();
    }
  }
  show(options: { focus?: 'first' | 'last' | 'none'; originalEvent?: Event } = {}): void {
    if (this.disabled || this.open) return;
    this.open = true;
    this.dispatchEvent(new CustomEvent<DropdownOpenChangeDetail>('bm-dropdown-open-change', { detail: { open: true, originalEvent: options.originalEvent }, bubbles: true, composed: true }));
    void this.updateComplete.then(() => {
      this.#position();
      if (options.focus === 'last') this.#focusItem(this.#enabledItems().length - 1);
      else if (options.focus !== 'none') this.#focusItem(0);
    });
  }
  hide(options: { restoreFocus?: boolean; originalEvent?: Event } = {}): void {
    if (!this.open) return;
    this.open = false;
    this.dispatchEvent(new CustomEvent<DropdownOpenChangeDetail>('bm-dropdown-open-change', { detail: { open: false, originalEvent: options.originalEvent }, bubbles: true, composed: true }));
    if (options.restoreFocus !== false) void this.updateComplete.then(() => this.#trigger()?.focus());
  }
  toggle(originalEvent?: Event): void { if (this.open) this.hide({ originalEvent }); else this.show({ focus: 'none', originalEvent }); }
  #trigger(): HTMLButtonElement | null { return this.renderRoot.querySelector('.trigger'); }
  #menu(): HTMLElement | null { return this.renderRoot.querySelector('.menu'); }
  #items(): BoldMergeDropdownItem[] { return [...this.querySelectorAll<BoldMergeDropdownItem>('bm-dropdown-item')]; }
  #enabledItems(): BoldMergeDropdownItem[] { return this.#items().filter((item) => !item.disabled); }
  #syncItems(): void { const items = this.#enabledItems(); if (this.#activeIndex >= items.length) this.#activeIndex = 0; items.forEach((item, index) => item.setTabStop(this.open && index === this.#activeIndex)); }
  #focusItem(index: number): void { const items = this.#enabledItems(); if (!items.length) return; this.#activeIndex = (index + items.length) % items.length; this.#syncItems(); items[this.#activeIndex]?.focus(); }
  #triggerClick(event: MouseEvent): void { this.toggle(event); }
  #triggerKeyDown(event: KeyboardEvent): void {
    if (event.key === 'ArrowDown' || event.key === 'ArrowUp') {
      event.preventDefault();
      if (this.open) this.#focusItem(event.key === 'ArrowUp' ? this.#enabledItems().length - 1 : 0);
      else this.show({ focus: event.key === 'ArrowUp' ? 'last' : 'first', originalEvent: event });
    }
    else if (event.key === 'Escape' && this.open) { event.preventDefault(); this.hide({ originalEvent: event }); }
  }
  #itemRequest = (event: CustomEvent<{ originalEvent: Event }>): void => {
    const item = event.composedPath().find((node) => node instanceof BoldMergeDropdownItem) as BoldMergeDropdownItem | undefined;
    if (!item || !this.#items().includes(item) || item.disabled) return;
    this.dispatchEvent(new CustomEvent<DropdownSelectDetail>('bm-dropdown-select', { detail: { value: item.value, href: item.href, originalEvent: event.detail.originalEvent }, bubbles: true, composed: true }));
    this.hide({ originalEvent: event.detail.originalEvent });
  };
  #menuKeyDown = (event: KeyboardEvent): void => {
    if (!this.open || event.composedPath().includes(this.#trigger() as EventTarget)) return;
    const items = this.#enabledItems();
    if (!items.length) return;
    if (event.key === 'ArrowDown') { event.preventDefault(); this.#focusItem(this.#activeIndex + 1); }
    else if (event.key === 'ArrowUp') { event.preventDefault(); this.#focusItem(this.#activeIndex - 1); }
    else if (event.key === 'Home') { event.preventDefault(); this.#focusItem(0); }
    else if (event.key === 'End') { event.preventDefault(); this.#focusItem(items.length - 1); }
    else if (event.key === 'Escape') { event.preventDefault(); this.hide({ originalEvent: event }); }
    else if (event.key === 'Tab') this.hide({ restoreFocus: false, originalEvent: event });
    else if (event.key.length === 1 && !event.ctrlKey && !event.metaKey && !event.altKey) {
      this.#typeahead += event.key.toLocaleLowerCase();
      window.clearTimeout(this.#typeaheadTimer);
      this.#typeaheadTimer = window.setTimeout(() => this.#clearTypeahead(), 500);
      const match = items.findIndex((item) => item.accessibleLabel.toLocaleLowerCase().startsWith(this.#typeahead));
      if (match >= 0) { event.preventDefault(); this.#focusItem(match); }
    }
  };
  #clearTypeahead(): void { if (this.#typeaheadTimer !== undefined) window.clearTimeout(this.#typeaheadTimer); this.#typeaheadTimer = undefined; this.#typeahead = ''; }
  #outsidePointerDown = (event: PointerEvent): void => { if (!event.composedPath().includes(this)) this.hide({ restoreFocus: false, originalEvent: event }); };
  #startPositioning(): void {
    document.addEventListener('pointerdown', this.#outsidePointerDown, true);
    window.addEventListener('resize', this.#position);
    window.addEventListener('scroll', this.#position, true);
    const trigger = this.#trigger(); const menu = this.#menu();
    if (trigger) this.#resizeObserver.observe(trigger); if (menu) this.#resizeObserver.observe(menu);
    void this.updateComplete.then(() => this.#position());
  }
  #stopPositioning(): void {
    document.removeEventListener('pointerdown', this.#outsidePointerDown, true);
    window.removeEventListener('resize', this.#position);
    window.removeEventListener('scroll', this.#position, true);
    this.#resizeObserver.disconnect();
  }
  #position = (): void => {
    if (!this.open) return;
    const trigger = this.#trigger(); const menu = this.#menu();
    if (!trigger || !menu) return;
    const anchor = trigger.getBoundingClientRect(); const popup = menu.getBoundingClientRect();
    const gap = 4; const edge = 8; const rtl = getComputedStyle(this).direction === 'rtl';
    let resolved: Exclude<DropdownPlacement, 'auto'> = this.placement === 'auto' ? 'bottom-start' : this.placement;
    if (this.placement === 'auto' && innerHeight - anchor.bottom < popup.height + gap && anchor.top > innerHeight - anchor.bottom) resolved = 'top-start';
    const top = resolved.startsWith('top') ? anchor.top - popup.height - gap : anchor.bottom + gap;
    const useEnd = resolved.endsWith('end');
    const alignRight = rtl ? !useEnd : useEnd;
    const preferredLeft = alignRight ? anchor.right - popup.width : anchor.left;
    menu.style.minWidth = `${anchor.width}px`;
    menu.style.left = `${Math.min(Math.max(edge, preferredLeft), Math.max(edge, innerWidth - popup.width - edge))}px`;
    menu.style.top = `${Math.min(Math.max(edge, top), Math.max(edge, innerHeight - popup.height - edge))}px`;
    menu.dataset.placement = resolved;
  };
  override render() {
    return html`<button class="trigger" part="trigger" type="button" ?disabled=${this.disabled} aria-label=${this.label || nothing} aria-haspopup="menu" aria-expanded=${String(this.open)} aria-controls=${this.#menuId} @click=${this.#triggerClick} @keydown=${this.#triggerKeyDown}><slot name="trigger">${this.label}</slot><span class="indicator" part="indicator" aria-hidden="true">⌄</span></button><div id=${this.#menuId} class="menu" part="menu" role="menu" aria-label=${this.label || nothing}><slot @slotchange=${this.#syncItems}></slot></div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-dropdown': BoldMergeDropdown; 'bm-dropdown-group': BoldMergeDropdownGroup; 'bm-dropdown-item': BoldMergeDropdownItem; }
  interface HTMLElementEventMap { 'bm-dropdown-open-change': CustomEvent<DropdownOpenChangeDetail>; 'bm-dropdown-select': CustomEvent<DropdownSelectDetail>; }
}
