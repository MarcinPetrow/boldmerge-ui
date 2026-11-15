import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export type ToastTone = 'neutral' | 'success' | 'warning' | 'error';
export type ToastCloseReason = 'api' | 'dismiss' | 'timeout';
export type ToastPlacement = 'bottom-end' | 'bottom-start' | 'top-end' | 'top-start';
export interface ToastCloseDetail { id: string; reason: ToastCloseReason; originalEvent?: Event; }
export interface ToastOpenChangeDetail { open: boolean; originalEvent?: Event; }
export interface ToastOptions { id?: string; title: string; message: string; tone?: ToastTone; duration?: number; closeLabel?: string; }

let toastSequence = 0;

export class BoldMergeToast extends LitElement {
  static properties = {
    open: { type: Boolean, reflect: true }, title: { type: String }, message: { type: String },
    tone: { type: String, reflect: true }, duration: { type: Number }, closeLabel: { type: String, attribute: 'close-label' },
  };
  static styles = css`
    :host { display: block; inline-size: min(var(--bm-toast-width), calc(100vw - var(--bm-size-32))); color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); pointer-events: auto; }
    :host(:not([open])) { display: none; }
    .toast { display: grid; grid-template-columns: auto minmax(0, 1fr) auto; align-items: start; gap: var(--bm-toast-content-gap); box-sizing: border-box; padding: var(--bm-toast-padding); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-border); border-radius: var(--bm-toast-radius); box-shadow: var(--bm-dialog-shadow); animation: reveal var(--bm-toast-transition-duration) var(--bm-motion-easing-standard); }
    .icon { display: inline-grid; place-items: center; inline-size: var(--bm-icon-size-default); block-size: var(--bm-icon-size-default); color: var(--bm-color-text-secondary); border: var(--bm-size-1) solid currentColor; border-radius: 50%; font-size: var(--bm-font-size-12); font-weight: var(--bm-typography-weight-emphasis); }
    :host([tone='success']) .icon { color: var(--bm-color-success); }
    :host([tone='warning']) .icon { color: var(--bm-color-warning); }
    :host([tone='error']) .icon { color: var(--bm-color-error); }
    .content { display: grid; gap: var(--bm-size-4); min-inline-size: 0; }
    .title { font-weight: var(--bm-typography-weight-emphasis); line-height: var(--bm-line-height-compact); }
    .message { color: var(--bm-color-text-secondary); line-height: var(--bm-line-height-body); overflow-wrap: anywhere; }
    .close { display: inline-grid; place-items: center; box-sizing: border-box; inline-size: var(--bm-button-height); block-size: var(--bm-button-height); margin: calc(-1 * var(--bm-size-8)); color: var(--bm-color-text-secondary); background: transparent; border: 0; border-radius: var(--bm-button-radius); font: inherit; cursor: pointer; }
    .close:hover { color: var(--bm-color-text-primary); background: var(--bm-color-surface-subtle); }
    .close:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    @keyframes reveal { from { opacity: 0; transform: translateY(var(--bm-size-8)); } }
    @media (prefers-reduced-motion: reduce) { .toast { animation: none; } }
    @media (pointer: coarse) { .close { min-inline-size: var(--bm-interaction-target-minimum); min-block-size: var(--bm-interaction-target-minimum); } }
  `;

  open = true;
  title = '';
  message = '';
  tone: ToastTone = 'neutral';
  duration = 5000;
  closeLabel = 'Dismiss notification';
  #remaining = 0;
  #startedAt = 0;
  #timer?: number;
  #paused = false;
  #visibility = (): void => { if (document.hidden) this.#pause(); else this.#resume(); };

  override connectedCallback(): void { super.connectedCallback(); if (!this.id) this.id = `bm-toast-${++toastSequence}`; document.addEventListener('visibilitychange', this.#visibility); }
  override disconnectedCallback(): void { this.#clearTimer(); document.removeEventListener('visibilitychange', this.#visibility); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void {
    if (changed.has('open') || changed.has('duration')) {
      this.#clearTimer();
      if (this.open) { this.#remaining = Math.max(0, this.duration); this.#resume(); }
    }
  }
  show(originalEvent?: Event): void {
    if (this.open) { this.#remaining = Math.max(0, this.duration); this.#resume(); return; }
    this.open = true;
    this.dispatchEvent(new CustomEvent<ToastOpenChangeDetail>('bm-toast-open-change', { detail: { open: true, originalEvent }, bubbles: true, composed: true }));
  }
  dismiss(reason: ToastCloseReason = 'api', originalEvent?: Event): void {
    if (!this.open) return;
    this.#clearTimer(); this.open = false;
    this.dispatchEvent(new CustomEvent<ToastCloseDetail>('bm-toast-close', { detail: { id: this.id, reason, originalEvent }, bubbles: true, composed: true }));
    this.dispatchEvent(new CustomEvent<ToastOpenChangeDetail>('bm-toast-open-change', { detail: { open: false, originalEvent }, bubbles: true, composed: true }));
  }
  #clearTimer(): void { if (this.#timer !== undefined) window.clearTimeout(this.#timer); this.#timer = undefined; }
  #pause = (): void => {
    if (this.#paused) return;
    this.#paused = true;
    if (this.#timer !== undefined) this.#remaining = Math.max(0, this.#remaining - (performance.now() - this.#startedAt));
    this.#clearTimer();
  };
  #resume = (): void => {
    if (!this.open || document.hidden) return;
    this.#paused = false;
    this.#clearTimer();
    if (this.#remaining <= 0 || this.duration <= 0) return;
    this.#startedAt = performance.now();
    this.#timer = window.setTimeout(() => this.dismiss('timeout'), this.#remaining);
  };
  #focusOut(): void { requestAnimationFrame(() => { if (!this.matches(':focus-within')) this.#resume(); }); }
  #icon(): string { return this.tone === 'success' ? '✓' : this.tone === 'warning' ? '!' : this.tone === 'error' ? '!' : 'i'; }
  override render() {
    const role = this.tone === 'warning' || this.tone === 'error' ? 'alert' : 'status';
    return html`<article class="toast" part="toast" role=${role} aria-atomic="true" @pointerenter=${this.#pause} @pointerleave=${this.#resume} @focusin=${this.#pause} @focusout=${this.#focusOut}><span class="icon" part="icon" aria-hidden="true"><slot name="icon">${this.#icon()}</slot></span><div class="content" part="content"><div class="title" part="title">${this.title}</div><div class="message" part="message">${this.message ? this.message : html`<slot></slot>`}</div></div><button class="close" part="close" type="button" aria-label=${this.closeLabel || nothing} @click=${(event: MouseEvent) => this.dismiss('dismiss', event)}>×</button></article>`;
  }
}

export class BoldMergeToastRegion extends LitElement {
  static properties = { placement: { type: String, reflect: true }, maxVisible: { type: Number, attribute: 'max-visible' }, label: { type: String } };
  static styles = css`
    :host { position: fixed; z-index: var(--bm-overlay-stack-level); inset-block-end: var(--bm-size-16); inset-inline-end: var(--bm-size-16); display: block; pointer-events: none; }
    :host([placement='bottom-start']) { inset-inline-start: var(--bm-size-16); inset-inline-end: auto; }
    :host([placement='top-end']) { inset-block-start: var(--bm-size-16); inset-block-end: auto; }
    :host([placement='top-start']) { inset-block-start: var(--bm-size-16); inset-block-end: auto; inset-inline-start: var(--bm-size-16); inset-inline-end: auto; }
    .region { display: flex; flex-direction: column; gap: var(--bm-toast-region-gap); }
  `;
  placement: ToastPlacement = 'bottom-end';
  maxVisible = 3;
  label = 'Notifications';
  #queue: Required<ToastOptions>[] = [];
  constructor() { super(); this.addEventListener('bm-toast-close', this.#toastClose as EventListener); }
  push(options: ToastOptions): string {
    const normalized: Required<ToastOptions> = { id: options.id || `bm-toast-${++toastSequence}`, title: options.title, message: options.message, tone: options.tone ?? 'neutral', duration: options.duration ?? 5000, closeLabel: options.closeLabel ?? 'Dismiss notification' };
    if (this.#visibleCount() < Math.max(1, this.maxVisible)) this.#append(normalized); else this.#queue.push(normalized);
    return normalized.id;
  }
  dismiss(id: string): boolean {
    const queued = this.#queue.findIndex((item) => item.id === id);
    if (queued >= 0) { this.#queue.splice(queued, 1); return true; }
    const toast = [...this.children].find((child): child is BoldMergeToast => child instanceof BoldMergeToast && child.id === id);
    if (!toast) return false;
    toast.dismiss('api'); return true;
  }
  clear(): void { this.#queue = []; [...this.children].forEach((child) => { if (child instanceof BoldMergeToast && child.open) child.dismiss('api'); }); }
  #visibleCount(): number { return [...this.children].filter((child) => child instanceof BoldMergeToast && child.open).length; }
  #append(options: Required<ToastOptions>): void {
    const toast = document.createElement('bm-toast') as BoldMergeToast;
    toast.id = options.id; toast.title = options.title; toast.message = options.message; toast.tone = options.tone; toast.duration = options.duration; toast.closeLabel = options.closeLabel; toast.dataset.bmRegionOwned = '';
    this.append(toast);
  }
  #drain(): void { while (this.#queue.length && this.#visibleCount() < Math.max(1, this.maxVisible)) { const next = this.#queue.shift(); if (next) this.#append(next); } }
  #toastClose = (event: CustomEvent<ToastCloseDetail>): void => {
    const toast = event.composedPath().find((node) => node instanceof BoldMergeToast) as BoldMergeToast | undefined;
    if (!toast || toast.parentElement !== this) return;
    if (toast.hasAttribute('data-bm-region-owned')) toast.remove();
    queueMicrotask(() => this.#drain());
  };
  override render() { return html`<div class="region" part="region" role="region" aria-label=${this.label || nothing}><slot @slotchange=${() => this.#drain()}></slot></div>`; }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-toast': BoldMergeToast; 'bm-toast-region': BoldMergeToastRegion; }
  interface HTMLElementEventMap { 'bm-toast-close': CustomEvent<ToastCloseDetail>; 'bm-toast-open-change': CustomEvent<ToastOpenChangeDetail>; }
}
