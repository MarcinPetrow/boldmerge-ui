import { LitElement, css, html, type PropertyValues } from 'lit';

export interface SegmentedChangeDetail { value: string; originalEvent: Event; }

export class BoldMergeSegment extends LitElement {
  static properties = { value: { type: String, reflect: true }, disabled: { type: Boolean, reflect: true } };
  static styles = css`
    :host { display: inline-flex; align-items: center; justify-content: center; box-sizing: border-box; block-size: var(--bm-button-height); padding-inline: var(--bm-button-padding-inline); color: var(--bm-color-text-secondary); font-family: var(--bm-typography-family-ui); font-weight: var(--bm-typography-weight-emphasis); cursor: pointer; outline: none; user-select: none; }
    :host([aria-checked='true']) { color: var(--bm-color-accent-on-subtle); background: var(--bm-color-accent-subtle); }
    :host(:focus-visible) { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: calc(-1 * var(--bm-size-2)); border-radius: var(--bm-button-radius); }
    :host([disabled]), :host([data-group-disabled]) { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    @media (pointer: coarse) { :host { min-block-size: var(--bm-interaction-target-minimum); } }
  `;
  value = '';
  disabled = false;
  constructor() {
    super();
    this.addEventListener('click', (event) => {
      if (this.disabled || this.hasAttribute('data-group-disabled')) { event.preventDefault(); return; }
      this.dispatchEvent(new CustomEvent('bm-segment-request', { detail: { originalEvent: event }, bubbles: true, composed: true }));
    });
  }
  override connectedCallback(): void { super.connectedCallback(); this.setAttribute('role', 'radio'); }
  setSelection(selected: boolean, tabStop: boolean, groupDisabled: boolean): void {
    this.setAttribute('aria-checked', String(selected));
    this.setAttribute('aria-disabled', String(this.disabled || groupDisabled));
    this.toggleAttribute('data-group-disabled', groupDisabled);
    this.tabIndex = tabStop && !this.disabled && !groupDisabled ? 0 : -1;
  }
  override render() { return html`<slot></slot>`; }
}

export class BoldMergeSegmentedControl extends LitElement {
  static formAssociated = true;
  static properties = { value: { type: String, reflect: true }, name: { type: String, reflect: true }, label: { type: String }, disabled: { type: Boolean, reflect: true } };
  static styles = css`
    :host { display: inline-block; max-inline-size: 100%; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .control { box-sizing: border-box; display: inline-flex; max-inline-size: 100%; overflow-x: auto; padding: var(--bm-size-2); background: var(--bm-color-surface-subtle); border: var(--bm-button-border-width) solid var(--bm-color-border); border-radius: var(--bm-button-radius); scrollbar-width: thin; }
    ::slotted(bm-segment) { flex: 0 0 auto; }
  `;
  readonly #internals: ElementInternals;
  readonly #observer: MutationObserver;
  #defaultValue = '';
  #formDisabled = false;
  value = '';
  name = '';
  label = '';
  disabled = false;

  constructor() {
    super();
    this.#internals = this.attachInternals();
    this.#observer = new MutationObserver(() => this.#sync());
    this.addEventListener('bm-segment-request', this.#request as EventListener);
    this.addEventListener('keydown', this.#keydown);
  }
  override connectedCallback(): void { super.connectedCallback(); this.#defaultValue = this.getAttribute('value') ?? ''; this.#observer.observe(this, { attributes: true, attributeFilter: ['value', 'disabled'], childList: true, subtree: true }); }
  override disconnectedCallback(): void { this.#observer.disconnect(); super.disconnectedCallback(); }
  get form(): HTMLFormElement | null { return this.#internals.form; }
  override updated(changed: PropertyValues<this>): void { if (changed.has('value') || changed.has('disabled')) this.#sync(); }
  formDisabledCallback(disabled: boolean): void { this.#formDisabled = disabled; this.#sync(); }
  formResetCallback(): void { this.value = this.#defaultValue; }
  formStateRestoreCallback(state: string | File | FormData | null): void { if (typeof state === 'string') this.value = state; }
  #segments(): BoldMergeSegment[] { return [...this.querySelectorAll<BoldMergeSegment>(':scope > bm-segment')]; }
  #sync(): void {
    const segments = this.#segments();
    const unavailable = this.disabled || this.#formDisabled;
    const enabled = segments.filter((segment) => !segment.disabled && !unavailable);
    if (!enabled.some((segment) => segment.value === this.value)) this.value = enabled[0]?.value ?? '';
    segments.forEach((segment) => segment.setSelection(segment.value === this.value, segment.value === this.value || (!this.value && segment === enabled[0]), unavailable));
    this.#internals.setFormValue(this.value || null, this.value || null);
  }
  #select(segment: BoldMergeSegment, originalEvent: Event): void {
    if (segment.disabled || this.disabled || this.#formDisabled || segment.value === this.value) return;
    this.value = segment.value;
    this.#sync();
    this.dispatchEvent(new CustomEvent<SegmentedChangeDetail>('bm-segmented-change', { detail: { value: this.value, originalEvent }, bubbles: true, composed: true }));
  }
  #request = (event: CustomEvent<{ originalEvent: Event }>): void => { const segment = event.target as BoldMergeSegment; if (this.#segments().includes(segment)) this.#select(segment, event.detail.originalEvent); };
  #keydown = (event: KeyboardEvent): void => {
    const current = event.composedPath().find((node) => node instanceof BoldMergeSegment) as BoldMergeSegment | undefined;
    if (!current) return;
    const segments = this.#segments().filter((segment) => !segment.disabled && !this.disabled && !this.#formDisabled);
    let index = segments.indexOf(current);
    const rtl = getComputedStyle(this).direction === 'rtl';
    if (event.key === 'Home') index = 0;
    else if (event.key === 'End') index = segments.length - 1;
    else if (event.key === 'ArrowRight') index = (index + (rtl ? -1 : 1) + segments.length) % segments.length;
    else if (event.key === 'ArrowLeft') index = (index + (rtl ? 1 : -1) + segments.length) % segments.length;
    else if (event.key === ' ' || event.key === 'Enter') { event.preventDefault(); this.#select(current, event); return; }
    else return;
    event.preventDefault();
    const next = segments[index];
    if (next) { this.#select(next, event); next.focus(); }
  };
  override render() { return html`<div class="control" part="control" role="radiogroup" aria-label=${this.label}><slot @slotchange=${this.#sync}></slot></div>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-segment': BoldMergeSegment; 'bm-segmented-control': BoldMergeSegmentedControl; } interface HTMLElementEventMap { 'bm-segmented-change': CustomEvent<SegmentedChangeDetail>; } }
