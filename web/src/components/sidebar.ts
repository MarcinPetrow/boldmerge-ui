import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export interface SidebarNavigateDetail { href: string; originalEvent: MouseEvent; }

export class BoldMergeSidebarItem extends LitElement {
  static properties = { href: { type: String, reflect: true }, label: { type: String }, active: { type: Boolean, reflect: true }, disabled: { type: Boolean, reflect: true }, _collapsed: { state: true } };
  static styles = css`
    :host { position: relative; display: block; font-family: var(--bm-typography-family-ui); }
    a { position: relative; display: flex; align-items: center; box-sizing: border-box; block-size: var(--bm-sidebar-item-height); padding-inline: var(--bm-size-16); gap: var(--bm-size-12); overflow: hidden; color: var(--bm-color-text-secondary); border-radius: var(--bm-button-radius); text-decoration: none; white-space: nowrap; }
    a::before { position: absolute; inset-block: var(--bm-size-8); inset-inline-start: 0; inline-size: var(--bm-sidebar-active-indicator-width); background: transparent; border-radius: 0 var(--bm-radius-2) var(--bm-radius-2) 0; content: ''; }
    :host([active]) a { color: var(--bm-color-accent-text); }
    :host([active]) a::before { background: var(--bm-color-accent-solid); }
    a:hover { color: var(--bm-color-text-primary); }
    a:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: calc(-1 * var(--bm-size-2)); }
    :host([disabled]) { opacity: var(--bm-content-opacity-disabled); }
    :host([disabled]) a { cursor: not-allowed; }
    .icon { display: inline-grid; flex: none; place-items: center; inline-size: var(--bm-icon-size-default); block-size: var(--bm-icon-size-default); }
    ::slotted([slot='icon']) { inline-size: 100%; block-size: 100%; }
    .tooltip { position: fixed; z-index: 20; display: none; margin-inline-start: calc(var(--bm-sidebar-width-collapsed) + var(--bm-size-8)); padding: var(--bm-size-4) var(--bm-size-8); color: var(--bm-color-text-primary); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-border); border-radius: var(--bm-tooltip-radius); box-shadow: var(--bm-dialog-shadow); font-size: var(--bm-font-size-12); pointer-events: none; }
    :host([data-collapsed]) a { justify-content: center; padding-inline: 0; }
    :host([data-collapsed]) .label { position: absolute; overflow: hidden; inline-size: var(--bm-size-1); block-size: var(--bm-size-1); clip-path: inset(50%); }
    :host([data-collapsed]:hover) .tooltip, :host([data-collapsed]:focus-within) .tooltip { display: block; }
    @media (pointer: coarse) { a { min-block-size: var(--bm-interaction-target-minimum); } }
  `;
  href = '';
  label = '';
  active = false;
  disabled = false;
  protected _collapsed = false;
  get accessibleLabel(): string { return this.label || this.textContent?.trim() || ''; }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector('a')?.focus(options); }
  setCollapsed(collapsed: boolean): void { this._collapsed = collapsed; this.toggleAttribute('data-collapsed', collapsed); }
  #click(event: MouseEvent): void {
    if (this.disabled) { event.preventDefault(); return; }
    this.dispatchEvent(new CustomEvent<SidebarNavigateDetail>('bm-sidebar-navigate', { detail: { href: this.href, originalEvent: event }, bubbles: true, composed: true }));
  }
  override render() {
    const label = this.accessibleLabel;
    return html`<a part="item" href=${this.disabled ? nothing : this.href || nothing} aria-current=${this.active ? 'page' : nothing} aria-disabled=${this.disabled ? 'true' : 'false'} aria-label=${this._collapsed ? label : nothing} tabindex=${this.disabled ? -1 : nothing} @click=${this.#click}><span class="icon" part="icon"><slot name="icon"></slot></span><span class="label" part="label"><slot></slot></span></a><span class="tooltip" part="tooltip" role="tooltip">${label}</span>`;
  }
}

export class BoldMergeSidebar extends LitElement {
  static properties = { label: { type: String }, collapsed: { type: Boolean, reflect: true }, open: { type: Boolean, reflect: true }, _mobile: { state: true } };
  static styles = css`
    :host { display: block; inline-size: var(--bm-sidebar-width-expanded); block-size: 100%; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); transition: inline-size var(--bm-motion-duration-overlay) var(--bm-motion-easing-standard); }
    :host([collapsed]) { inline-size: var(--bm-sidebar-width-collapsed); }
    aside { display: grid; grid-template-rows: auto minmax(0, 1fr) auto; box-sizing: border-box; inline-size: 100%; block-size: 100%; padding: var(--bm-size-12) var(--bm-size-8); background: var(--bm-color-surface); border-inline-end: var(--bm-size-1) solid var(--bm-color-border); }
    nav { overflow-y: auto; min-block-size: 0; }
    .backdrop { display: none; }
    @media (max-width: 768px) { /* mirrors app-shell.mobile.breakpoint */
      :host { position: fixed; inset: 0 auto 0 0; z-index: 30; inline-size: min(var(--bm-sidebar-width-expanded), calc(100vw - var(--bm-size-32))); transform: translateX(-100%); transition: transform var(--bm-motion-duration-overlay) var(--bm-motion-easing-standard); }
      :host([open]) { transform: translateX(0); }
      :host([collapsed]) { inline-size: min(var(--bm-sidebar-width-expanded), calc(100vw - var(--bm-size-32))); }
      .backdrop { position: fixed; inset: 0; z-index: -1; display: block; background: color-mix(in srgb, var(--bm-color-neutral-950) 48%, transparent); opacity: 0; pointer-events: none; transition: opacity var(--bm-motion-duration-overlay) var(--bm-motion-easing-standard); }
      :host([open]) .backdrop { opacity: 1; pointer-events: auto; }
      aside { box-shadow: var(--bm-dialog-shadow); }
    }
    @media (prefers-reduced-motion: reduce) { :host, .backdrop { transition: none; } }
  `;
  label = '';
  collapsed = false;
  open = false;
  protected _mobile = false;
  #media?: MediaQueryList;
  #mediaChange = (event: MediaQueryListEvent | MediaQueryList): void => { this._mobile = event.matches; this.#syncItems(); this.requestUpdate(); };
  constructor() {
    super();
    this.addEventListener('keydown', (event) => { if (event.key === 'Escape' && this._mobile && this.open) { event.preventDefault(); this.#dismiss(event); } });
    this.addEventListener('bm-sidebar-navigate', () => { if (this._mobile) this.#dismiss(new Event('navigate')); });
  }
  override connectedCallback(): void { super.connectedCallback(); if (typeof matchMedia === 'function') { this.#media = matchMedia('(max-width: 768px)'); this.#mediaChange(this.#media); this.#media.addEventListener('change', this.#mediaChange); } }
  override disconnectedCallback(): void { this.#media?.removeEventListener('change', this.#mediaChange); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void { if (changed.has('collapsed')) this.#syncItems(); }
  focus(options?: FocusOptions): void { this.querySelector<BoldMergeSidebarItem>('bm-sidebar-item:not([disabled])')?.focus(options); }
  #syncItems(): void { this.querySelectorAll<BoldMergeSidebarItem>('bm-sidebar-item').forEach((item) => item.setCollapsed(this.collapsed && !this._mobile)); }
  #slotChange(): void { this.#syncItems(); }
  #dismiss(originalEvent: Event): void { this.open = false; this.dispatchEvent(new CustomEvent('bm-sidebar-dismiss', { detail: { originalEvent }, bubbles: true, composed: true })); }
  override render() { return html`<div class="backdrop" part="backdrop" aria-hidden="true" @click=${this.#dismiss}></div><aside part="sidebar" .inert=${this._mobile && !this.open} aria-hidden=${this._mobile && !this.open ? 'true' : nothing}><div part="header"><slot name="header"></slot></div><nav part="nav" aria-label=${this.label || nothing}><slot @slotchange=${this.#slotChange}></slot></nav><div part="footer"><slot name="footer"></slot></div></aside>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-sidebar-item': BoldMergeSidebarItem; 'bm-sidebar': BoldMergeSidebar; } interface HTMLElementEventMap { 'bm-sidebar-navigate': CustomEvent<SidebarNavigateDetail>; } }
