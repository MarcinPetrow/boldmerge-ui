import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export interface SelectChangeDetail {
  value: string;
  originalEvent: Event;
}

let selectSequence = 0;

export class BoldMergeOption extends LitElement {
  static properties = {
    value: { type: String, reflect: true },
    disabled: { type: Boolean, reflect: true },
    selected: { type: Boolean, reflect: true },
  };

  static styles = css`:host { display: none !important; }`;

  value = '';
  disabled = false;
  selected = false;

  get label(): string { return this.textContent?.trim() ?? ''; }

  override render() { return html`<slot></slot>`; }
}

export class BoldMergeSelect extends LitElement {
  static formAssociated = true;

  static properties = {
    value: { type: String },
    label: { type: String },
    name: { type: String, reflect: true },
    placeholder: { type: String },
    autocomplete: { type: String },
    required: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
    error: { type: String },
    _nativeError: { state: true },
    _optionVersion: { state: true },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .shell { overflow: hidden; box-sizing: border-box; background: var(--bm-color-surface-subtle); border: var(--bm-input-border-width) solid var(--bm-color-border-strong); border-radius: var(--bm-input-radius); transition: border-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard), outline-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard); }
    .shell:focus-within { border-color: var(--bm-color-focus); outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    :host([data-invalid]) .shell { border-color: var(--bm-color-error); }
    .field { position: relative; block-size: var(--bm-input-floating-height); min-inline-size: 0; }
    select { inline-size: 100%; block-size: 100%; box-sizing: border-box; padding: var(--bm-size-12) var(--bm-input-padding-inline) 0; color: var(--bm-color-text-primary); background: transparent; border: 0; outline: 0; font: inherit; line-height: var(--bm-input-content-line-height); cursor: pointer; }
    label { position: absolute; inset-block-start: var(--bm-size-8); inset-inline: var(--bm-input-padding-inline) var(--bm-size-32); overflow: hidden; color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); line-height: var(--bm-input-content-line-height); text-overflow: ellipsis; white-space: nowrap; pointer-events: none; }
    .shell:focus-within label { color: var(--bm-color-focus); }
    :host([data-invalid]) label { color: var(--bm-color-error); }
    .error { display: flex; align-items: start; gap: var(--bm-size-8); padding: var(--bm-input-error-padding-block) var(--bm-input-padding-inline); color: var(--bm-color-error); border-block-start: var(--bm-input-error-divider-width) solid var(--bm-color-error); font-size: var(--bm-font-size-12); line-height: var(--bm-input-error-line-height); }
    .error-icon { display: inline-grid; flex: none; place-items: center; inline-size: var(--bm-input-error-icon-size); block-size: var(--bm-input-error-icon-size); border: var(--bm-size-1) solid currentColor; border-radius: 50%; font-weight: var(--bm-typography-weight-emphasis); }
    .options-source { display: none; }
    :host([disabled]), :host([data-form-disabled]) { opacity: var(--bm-content-opacity-disabled); }
    :host([disabled]) select, :host([data-form-disabled]) select { cursor: not-allowed; }
    @media (pointer: coarse) { .field { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { .shell { transition: none; } }
  `;

  readonly #internals: ElementInternals;
  readonly #selectId = `bm-select-${++selectSequence}`;
  readonly #errorId = `${this.#selectId}-error`;
  readonly #observer: MutationObserver;
  #defaultValue = '';
  #formDisabled = false;
  #touched = false;

  value = '';
  label = '';
  name = '';
  placeholder = '';
  autocomplete = '';
  required = false;
  disabled = false;
  error = '';
  protected _nativeError = '';
  protected _optionVersion = 0;

  constructor() {
    super();
    this.#internals = this.attachInternals();
    this.#observer = new MutationObserver(() => this.#optionsChanged());
    this.addEventListener('invalid', () => { this.#touched = true; this.#syncValidity(); });
  }

  override connectedCallback(): void {
    super.connectedCallback();
    this.#defaultValue = this.getAttribute('value') ?? '';
    this.#observer.observe(this, { attributes: true, attributeFilter: ['value', 'disabled', 'selected'], childList: true, characterData: true, subtree: true });
  }

  override disconnectedCallback(): void { this.#observer.disconnect(); super.disconnectedCallback(); }

  get form(): HTMLFormElement | null { return this.#internals.form; }
  get validity(): ValidityState { return this.#internals.validity; }
  get validationMessage(): string { return this.#internals.validationMessage; }
  get willValidate(): boolean { return this.#internals.willValidate; }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector('select')?.focus(options); }
  setCustomValidity(message: string): void { this._nativeError = ''; this.error = message; this.#touched = Boolean(message); }
  checkValidity(): boolean { this.#syncValidity(); return this.#internals.checkValidity(); }
  reportValidity(): boolean { this.#touched = true; this.#syncValidity(); return this.#internals.reportValidity(); }
  formDisabledCallback(disabled: boolean): void { this.#formDisabled = disabled; this.toggleAttribute('data-form-disabled', disabled); this.requestUpdate(); }
  formResetCallback(): void { this.value = this.#defaultValue; this.error = ''; this._nativeError = ''; this.#touched = false; }
  formStateRestoreCallback(state: string | File | FormData | null): void { if (typeof state === 'string') this.value = state; }

  override updated(changed: PropertyValues<this>): void {
    const select = this.renderRoot.querySelector('select');
    if (select && select.value !== this.value) select.value = this.value;
    if (changed.has('value') || changed.has('error') || changed.has('required') || changed.has('disabled')) this.#syncValidity();
  }

  #options(): BoldMergeOption[] { return [...this.querySelectorAll<BoldMergeOption>(':scope > bm-option')]; }

  #optionsChanged(): void {
    const options = this.#options();
    if (this.value && !options.some((option) => option.value === this.value && !option.disabled)) this.value = '';
    if (!this.value && !this.placeholder) this.value = options.find((option) => option.selected && !option.disabled)?.value ?? options.find((option) => !option.disabled)?.value ?? '';
    if (!this.#defaultValue) this.#defaultValue = this.value;
    this._optionVersion += 1;
  }

  #syncValidity(): void {
    const select = this.renderRoot.querySelector('select');
    this.#internals.setFormValue(this.value || null, this.value || null);
    if (this.error) { this.#internals.setValidity({ customError: true }, this.error, select ?? undefined); this._nativeError = ''; }
    else if (select && !select.validity.valid) { this.#internals.setValidity({ valueMissing: select.validity.valueMissing }, select.validationMessage, select); this._nativeError = this.#touched ? select.validationMessage : ''; }
    else { this.#internals.setValidity({}); this._nativeError = ''; }
    this.toggleAttribute('data-invalid', Boolean(this.error || this._nativeError));
  }

  #change(event: Event): void {
    this.value = (event.currentTarget as HTMLSelectElement).value;
    this.#touched = true;
    this.#syncValidity();
    this.dispatchEvent(new CustomEvent<SelectChangeDetail>('bm-select-change', { detail: { value: this.value, originalEvent: event }, bubbles: true, composed: true }));
  }

  #blur(): void { this.#touched = true; this.#syncValidity(); }

  override render() {
    const message = this.error || this._nativeError;
    const unavailable = this.disabled || this.#formDisabled;
    const options = this.#options();
    return html`
      <div class="shell" part="container">
        <div class="field" part="field">
          <select id=${this.#selectId} part="select" .value=${this.value} ?required=${this.required} ?disabled=${unavailable} autocomplete=${this.autocomplete || nothing} aria-invalid=${message ? 'true' : 'false'} aria-describedby=${message ? this.#errorId : nothing} aria-errormessage=${message ? this.#errorId : nothing} @change=${this.#change} @blur=${this.#blur}>
            ${this.placeholder ? html`<option value="" ?disabled=${this.required} ?selected=${!this.value}>${this.placeholder}</option>` : null}
            ${options.map((option) => html`<option .value=${option.value} ?disabled=${option.disabled} ?selected=${option.value === this.value}>${option.label}</option>`)}
          </select>
          <label part="label" for=${this.#selectId}>${this.label}${this.required ? html`<span aria-hidden="true"> *</span>` : null}</label>
        </div>
        ${message ? html`<div class="error" id=${this.#errorId} part="error" role="alert"><span class="error-icon" part="error-icon" aria-hidden="true"><slot name="error-icon">!</slot></span><span>${message}</span></div>` : null}
      </div>
      <slot class="options-source" @slotchange=${this.#optionsChanged}></slot>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-option': BoldMergeOption; 'bm-select': BoldMergeSelect; }
  interface HTMLElementEventMap { 'bm-select-change': CustomEvent<SelectChangeDetail>; }
}
