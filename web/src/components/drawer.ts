import { LitElement, css, html, nothing, type PropertyValues } from 'lit';
import { lockDocumentScroll, unlockDocumentScroll } from '../utilities/modal-scroll-lock.js';

export type DrawerMode = 'overlay' | 'push';
export type DrawerPosition = 'start' | 'end';
export type DrawerCloseReason = 'api' | 'action' | 'escape' | 'backdrop';
export interface DrawerOpenChangeDetail { open: boolean; originalEvent?: Event; }
export interface DrawerCloseDetail { reason: DrawerCloseReason; returnValue: string; originalEvent?: Event; }
export interface DrawerShowOptions { trigger?: HTMLElement; initialFocus?: HTMLElement; originalEvent?: Event; }

export class BoldMergeDrawer extends LitElement {
  static properties = {
    open: { type: Boolean, reflect: true }, label: { type: String },
    dismissible: { type: Boolean, reflect: true }, mode: { type: String, reflect: true },
    position: { type: String, reflect: true }, _mobile: { state: true },
    _hasHeader: { state: true }, _hasFooter: { state: true }, _hasActions: { state: true },
  };
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .layout { display: grid; min-inline-size: 0; }
    :host([mode='push'][open]) .layout { grid-template-columns: minmax(0, 1fr) var(--bm-drawer-width); }
    :host([mode='push'][open][position='start']) .layout { grid-template-columns: var(--bm-drawer-width) minmax(0, 1fr); }
    .content { min-inline-size: 0; }
    :host([mode='push'][open][position='start']) .content { grid-column: 2; }
    .panel { box-sizing: border-box; color: inherit; background: var(--bm-color-surface); }
    aside.panel { inline-size: 100%; min-block-size: 100%; border-inline-start: var(--bm-size-1) solid var(--bm-color-border); }
    :host([position='start']) aside.panel { grid-column: 1; grid-row: 1; border-inline-start: 0; border-inline-end: var(--bm-size-1) solid var(--bm-color-border); }
    aside[hidden] { display: none; }
    dialog.panel { position: fixed; inset-block: 0; inset-inline-start: auto; inset-inline-end: 0; inline-size: min(var(--bm-drawer-width), calc(100vw - var(--bm-size-32))); max-inline-size: none; block-size: 100dvh; max-block-size: none; margin: 0; padding: 0; border: 0; border-radius: 0; box-shadow: var(--bm-drawer-shadow); overflow: auto; }
    :host([position='start']) dialog.panel { inset-inline-start: 0; inset-inline-end: auto; }
    dialog::backdrop { background: var(--bm-color-overlay); }
    dialog[open] { animation: drawer-end var(--bm-drawer-transition-duration) var(--bm-motion-easing-standard); }
    :host([position='start']) dialog[open] { animation-name: drawer-start; }
    .frame { display: grid; grid-template-rows: auto minmax(0, 1fr) auto; gap: var(--bm-dialog-content-gap); box-sizing: border-box; min-block-size: 100%; padding: var(--bm-drawer-padding); }
    .header[hidden], .footer[hidden] { display: none; }
    .body, .header, .footer { min-inline-size: 0; }
    .footer { display: grid; gap: var(--bm-dialog-actions-gap); }
    .actions { display: flex; flex-wrap: wrap; justify-content: end; gap: var(--bm-dialog-actions-gap); }
    @keyframes drawer-end { from { transform: translateX(100%); } }
    @keyframes drawer-start { from { transform: translateX(-100%); } }
    @media (max-width: 768px) { /* mirrors app-shell.mobile.breakpoint */
      :host([mode='push'][open]) .layout, :host([mode='push'][open][position='start']) .layout { grid-template-columns: minmax(0, 1fr); }
      :host([mode='push'][open][position='start']) .content { grid-column: 1; }
    }
    @media (prefers-reduced-motion: reduce) { dialog[open] { animation: none; } }
  `;

  open = false;
  label = '';
  dismissible = true;
  mode: DrawerMode = 'overlay';
  position: DrawerPosition = 'end';
  protected _mobile = false;
  protected _hasHeader = false;
  protected _hasFooter = false;
  protected _hasActions = false;
  #media?: MediaQueryList;
  #modalActive = false;
  #restoreTarget?: HTMLElement;
  #pendingInitialFocus?: HTMLElement;
  #returnValue = '';
  #mediaChange = (event: MediaQueryListEvent | MediaQueryList): void => { this._mobile = event.matches; };

  constructor() { super(); this.addEventListener('click', this.#actionClick); }
  override connectedCallback(): void {
    super.connectedCallback();
    if (typeof matchMedia === 'function') { this.#media = matchMedia('(max-width: 768px)'); this.#mediaChange(this.#media); this.#media.addEventListener('change', this.#mediaChange); }
  }
  override disconnectedCallback(): void {
    this.#media?.removeEventListener('change', this.#mediaChange);
    if (this.#modalActive) { unlockDocumentScroll(); this.#modalActive = false; }
    super.disconnectedCallback();
  }
  override updated(changed: PropertyValues<this>): void { if (changed.has('open') || changed.has('mode') || changed.has('_mobile' as never)) this.#syncOpen(); }

  show(options: DrawerShowOptions = {}): void {
    if (this.open) return;
    this.#restoreTarget = options.trigger ?? this.#activeElement();
    this.#pendingInitialFocus = options.initialFocus;
    this.open = true;
    this.dispatchEvent(new CustomEvent<DrawerOpenChangeDetail>('bm-drawer-open-change', { detail: { open: true, originalEvent: options.originalEvent }, bubbles: true, composed: true }));
  }
  close(reason: DrawerCloseReason = 'api', returnValue = '', originalEvent?: Event): boolean { return this.#requestClose(reason, returnValue, originalEvent); }
  #overlay(): boolean { return this.mode === 'overlay' || this._mobile; }
  #dialog(): HTMLDialogElement | null { return this.renderRoot.querySelector('dialog'); }
  #activeElement(): HTMLElement | undefined { return document.activeElement instanceof HTMLElement ? document.activeElement : undefined; }
  #syncOpen(): void {
    const overlay = this.#overlay();
    const dialog = this.#dialog();
    if (this.open && overlay && dialog) {
      if (!this.#restoreTarget) this.#restoreTarget = this.#activeElement();
      if (!dialog.open) dialog.showModal();
      if (!this.#modalActive) { lockDocumentScroll(); this.#modalActive = true; }
      const requested = this.#pendingInitialFocus; this.#pendingInitialFocus = undefined;
      requestAnimationFrame(() => this.#focusInitial(requested));
    } else {
      if (dialog?.open) dialog.close(this.#returnValue);
      if (this.#modalActive) { unlockDocumentScroll(); this.#modalActive = false; }
      if (this.open && !overlay) {
        const requested = this.#pendingInitialFocus; this.#pendingInitialFocus = undefined;
        if (requested) requestAnimationFrame(() => this.#focusInitial(requested));
      }
      if (!this.open) {
        const restore = this.#restoreTarget; this.#restoreTarget = undefined;
        if (restore?.isConnected) requestAnimationFrame(() => restore.focus());
      }
    }
  }
  #panelElements(): HTMLElement[] {
    return [...this.children].filter((child): child is HTMLElement => child instanceof HTMLElement && child.slot !== 'content');
  }
  #focusInitial(requested?: HTMLElement): void {
    if (requested?.isConnected) { requested.focus(); return; }
    const selectors = '[autofocus], button:not([disabled]), a[href], input:not([disabled]), select:not([disabled]), textarea:not([disabled]), [tabindex]:not([tabindex="-1"]), bm-button:not([disabled]), bm-input:not([disabled]), bm-select:not([disabled]), bm-checkbox:not([disabled]), bm-switch:not([disabled]), bm-icon-button:not([disabled]), bm-dropdown:not([disabled])';
    for (const root of this.#panelElements()) {
      const candidate = root.matches(selectors) ? root : root.querySelector<HTMLElement>(selectors);
      if (candidate) { candidate.focus(); return; }
    }
    this.#dialog()?.focus();
  }
  #requestClose(reason: DrawerCloseReason, returnValue: string, originalEvent?: Event): boolean {
    if (!this.open) return false;
    const detail: DrawerCloseDetail = { reason, returnValue, originalEvent };
    const request = new CustomEvent<DrawerCloseDetail>('bm-drawer-request-close', { detail, bubbles: true, composed: true, cancelable: true });
    if (!this.dispatchEvent(request)) return false;
    this.#returnValue = returnValue;
    this.open = false;
    this.dispatchEvent(new CustomEvent<DrawerCloseDetail>('bm-drawer-close', { detail, bubbles: true, composed: true }));
    this.dispatchEvent(new CustomEvent<DrawerOpenChangeDetail>('bm-drawer-open-change', { detail: { open: false, originalEvent }, bubbles: true, composed: true }));
    return true;
  }
  #cancel(event: Event): void { event.preventDefault(); if (this.dismissible) this.#requestClose('escape', '', event); }
  #backdropClick(event: MouseEvent): void { if (event.target === event.currentTarget && this.dismissible) this.#requestClose('backdrop', '', event); }
  #actionClick = (event: MouseEvent): void => {
    const action = event.composedPath().find((node) => node instanceof HTMLElement && node.hasAttribute('data-bm-drawer-close')) as HTMLElement | undefined;
    if (action) this.#requestClose('action', action.getAttribute('data-bm-drawer-close') ?? '', event);
  };
  #assigned(event: Event, property: '_hasHeader' | '_hasFooter' | '_hasActions'): void {
    const slot = event.currentTarget as HTMLSlotElement;
    this[property] = slot.assignedNodes({ flatten: true }).some((node) => node.nodeType !== Node.TEXT_NODE || Boolean(node.textContent?.trim()));
  }
  #panelContent() {
    return html`<div class="frame" part="frame"><div class="header" part="header" ?hidden=${!this._hasHeader}><slot name="header" @slotchange=${(event: Event) => this.#assigned(event, '_hasHeader')}></slot></div><div class="body" part="body"><slot></slot></div><div class="footer" part="footer" ?hidden=${!this._hasFooter && !this._hasActions}><slot name="footer" @slotchange=${(event: Event) => this.#assigned(event, '_hasFooter')}></slot><div class="actions" part="actions"><slot name="actions" @slotchange=${(event: Event) => this.#assigned(event, '_hasActions')}></slot></div></div></div>`;
  }
  override render() {
    const panel = this.#overlay()
      ? html`<dialog class="panel" part="panel overlay" aria-label=${this.label || nothing} @cancel=${this.#cancel} @click=${this.#backdropClick}>${this.#panelContent()}</dialog>`
      : html`<aside class="panel" part="panel push" aria-label=${this.label || nothing} ?hidden=${!this.open}>${this.#panelContent()}</aside>`;
    return html`<div class="layout" part="layout"><div class="content" part="content"><slot name="content"></slot></div>${panel}</div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-drawer': BoldMergeDrawer; }
  interface HTMLElementEventMap { 'bm-drawer-open-change': CustomEvent<DrawerOpenChangeDetail>; 'bm-drawer-request-close': CustomEvent<DrawerCloseDetail>; 'bm-drawer-close': CustomEvent<DrawerCloseDetail>; }
}
