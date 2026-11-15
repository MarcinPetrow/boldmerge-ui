import { LitElement, css, html, nothing, type PropertyValues } from 'lit';
import { lockDocumentScroll, unlockDocumentScroll } from '../utilities/modal-scroll-lock.js';

export type DialogCloseReason = 'api' | 'action' | 'escape' | 'backdrop';
export interface DialogOpenChangeDetail { open: boolean; originalEvent?: Event; }
export interface DialogCloseDetail { reason: DialogCloseReason; returnValue: string; originalEvent?: Event; }
export interface DialogShowOptions { trigger?: HTMLElement; initialFocus?: HTMLElement; originalEvent?: Event; }

export class BoldMergeDialog extends LitElement {
  static properties = {
    open: { type: Boolean, reflect: true },
    label: { type: String },
    dismissible: { type: Boolean, reflect: true },
    _hasHeader: { state: true },
    _hasFooter: { state: true },
    _hasActions: { state: true },
  };

  static styles = css`
    :host { color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    dialog { box-sizing: border-box; inline-size: min(var(--bm-dialog-width-max), calc(100vw - var(--bm-size-32))); max-block-size: calc(100dvh - var(--bm-size-32)); padding: 0; color: inherit; background: var(--bm-color-surface); border: 0; border-radius: var(--bm-dialog-radius); box-shadow: var(--bm-dialog-shadow); overflow: auto; }
    dialog::backdrop { background: var(--bm-color-overlay); }
    dialog[open] { animation: reveal var(--bm-dialog-transition-duration) var(--bm-motion-easing-standard); }
    .frame { display: grid; gap: var(--bm-dialog-content-gap); padding: var(--bm-dialog-padding); }
    .header, .footer { min-inline-size: 0; }
    .header[hidden], .footer[hidden] { display: none; }
    .footer { display: flex; align-items: end; gap: var(--bm-dialog-actions-gap); }
    .footer-content { flex: 1; min-inline-size: 0; }
    .actions { display: flex; flex-wrap: wrap; justify-content: end; gap: var(--bm-dialog-actions-gap); margin-inline-start: auto; }
    .content { min-inline-size: 0; }
    @keyframes reveal { from { opacity: 0; transform: translateY(var(--bm-size-8)); } }
    @media (prefers-reduced-motion: reduce) { dialog[open] { animation: none; } }
  `;

  open = false;
  label = '';
  dismissible = true;
  protected _hasHeader = false;
  protected _hasFooter = false;
  protected _hasActions = false;
  #modalActive = false;
  #restoreTarget?: HTMLElement;
  #pendingInitialFocus?: HTMLElement;
  #returnValue = '';

  constructor() {
    super();
    this.addEventListener('click', this.#actionClick);
  }

  override disconnectedCallback(): void {
    if (this.#modalActive) { unlockDocumentScroll(); this.#modalActive = false; }
    super.disconnectedCallback();
  }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('open')) this.#syncOpen();
  }

  show(options: DialogShowOptions = {}): void {
    if (this.open) return;
    this.#restoreTarget = options.trigger ?? this.#activeElement();
    this.#pendingInitialFocus = options.initialFocus;
    this.open = true;
    this.dispatchEvent(new CustomEvent<DialogOpenChangeDetail>('bm-dialog-open-change', { detail: { open: true, originalEvent: options.originalEvent }, bubbles: true, composed: true }));
  }

  close(reason: DialogCloseReason = 'api', returnValue = '', originalEvent?: Event): boolean {
    return this.#requestClose(reason, returnValue, originalEvent);
  }

  #dialog(): HTMLDialogElement | null { return this.renderRoot.querySelector('dialog'); }

  #activeElement(): HTMLElement | undefined {
    const active = document.activeElement;
    return active instanceof HTMLElement ? active : undefined;
  }

  #syncOpen(): void {
    const dialog = this.#dialog();
    if (!dialog || !this.isConnected) return;
    if (this.open) {
      if (!this.#restoreTarget) this.#restoreTarget = this.#activeElement();
      if (!dialog.open) dialog.showModal();
      if (!this.#modalActive) { lockDocumentScroll(); this.#modalActive = true; }
      const requested = this.#pendingInitialFocus;
      this.#pendingInitialFocus = undefined;
      requestAnimationFrame(() => this.#focusInitial(requested));
    } else {
      if (dialog.open) dialog.close(this.#returnValue);
      if (this.#modalActive) { unlockDocumentScroll(); this.#modalActive = false; }
      const restore = this.#restoreTarget;
      this.#restoreTarget = undefined;
      if (restore?.isConnected) requestAnimationFrame(() => restore.focus());
    }
  }

  #focusInitial(requested?: HTMLElement): void {
    if (requested?.isConnected) { requested.focus(); return; }
    const candidate = this.querySelector<HTMLElement>('[autofocus], button:not([disabled]), a[href], input:not([disabled]), select:not([disabled]), textarea:not([disabled]), [tabindex]:not([tabindex="-1"]), bm-button:not([disabled]), bm-input:not([disabled]), bm-select:not([disabled]), bm-checkbox:not([disabled]), bm-switch:not([disabled]), bm-icon-button:not([disabled]), bm-dropdown:not([disabled])');
    (candidate ?? this.#dialog())?.focus();
  }

  #requestClose(reason: DialogCloseReason, returnValue: string, originalEvent?: Event): boolean {
    if (!this.open) return false;
    const detail: DialogCloseDetail = { reason, returnValue, originalEvent };
    const request = new CustomEvent<DialogCloseDetail>('bm-dialog-request-close', { detail, bubbles: true, composed: true, cancelable: true });
    if (!this.dispatchEvent(request)) return false;
    this.#returnValue = returnValue;
    this.open = false;
    this.dispatchEvent(new CustomEvent<DialogCloseDetail>('bm-dialog-close', { detail, bubbles: true, composed: true }));
    this.dispatchEvent(new CustomEvent<DialogOpenChangeDetail>('bm-dialog-open-change', { detail: { open: false, originalEvent }, bubbles: true, composed: true }));
    return true;
  }

  #cancel(event: Event): void {
    event.preventDefault();
    if (this.dismissible) this.#requestClose('escape', '', event);
  }

  #backdropClick(event: MouseEvent): void {
    if (event.target === event.currentTarget && this.dismissible) this.#requestClose('backdrop', '', event);
  }

  #actionClick = (event: MouseEvent): void => {
    const action = event.composedPath().find((node) => node instanceof HTMLElement && node.hasAttribute('data-bm-dialog-close')) as HTMLElement | undefined;
    if (action) this.#requestClose('action', action.getAttribute('data-bm-dialog-close') ?? '', event);
  };

  #assigned(event: Event, property: '_hasHeader' | '_hasFooter' | '_hasActions'): void {
    const slot = event.currentTarget as HTMLSlotElement;
    this[property] = slot.assignedNodes({ flatten: true }).some((node) => node.nodeType !== Node.TEXT_NODE || Boolean(node.textContent?.trim()));
  }

  override render() {
    return html`<dialog part="dialog" aria-label=${this.label || nothing} @cancel=${this.#cancel} @click=${this.#backdropClick}><div class="frame" part="frame"><div class="header" part="header" ?hidden=${!this._hasHeader}><slot name="header" @slotchange=${(event: Event) => this.#assigned(event, '_hasHeader')}></slot></div><div class="content" part="content"><slot></slot></div><div class="footer" part="footer" ?hidden=${!this._hasFooter && !this._hasActions}><div class="footer-content"><slot name="footer" @slotchange=${(event: Event) => this.#assigned(event, '_hasFooter')}></slot></div><div class="actions" part="actions"><slot name="actions" @slotchange=${(event: Event) => this.#assigned(event, '_hasActions')}></slot></div></div></div></dialog>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-dialog': BoldMergeDialog; }
  interface HTMLElementEventMap { 'bm-dialog-open-change': CustomEvent<DialogOpenChangeDetail>; 'bm-dialog-request-close': CustomEvent<DialogCloseDetail>; 'bm-dialog-close': CustomEvent<DialogCloseDetail>; }
}
