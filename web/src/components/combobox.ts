import { LitElement, css, html, nothing, type PropertyValues } from 'lit';
import { BoldMergeOption } from './select.js';

export interface ComboboxInputDetail { inputValue: string; originalEvent: Event; }
export interface ComboboxChangeDetail { value: string; label: string; originalEvent: Event; }
export interface ComboboxOpenChangeDetail { open: boolean; originalEvent?: Event; }

let comboboxSequence = 0;

export class BoldMergeCombobox extends LitElement {
  static formAssociated = true;
  static properties = {
    value: { type: String, noAccessor: true }, inputValue: { type: String, attribute: 'input-value' },
    label: { type: String }, name: { type: String, reflect: true }, placeholder: { type: String },
    required: { type: Boolean, reflect: true }, disabled: { type: Boolean, reflect: true },
    error: { type: String }, open: { type: Boolean, reflect: true }, filter: { type: Boolean },
    _nativeError: { state: true }, _optionVersion: { state: true }, _activeIndex: { state: true },
  };
  static styles = css`
    :host { position: relative; display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .shell { overflow: hidden; box-sizing: border-box; background: var(--bm-color-surface-subtle); border: var(--bm-input-border-width) solid var(--bm-color-border-strong); border-radius: var(--bm-input-radius); transition: border-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard), outline-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard); }
    .shell:focus-within { border-color: var(--bm-color-focus); outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    :host([data-invalid]) .shell { border-color: var(--bm-color-error); }
    .field { position: relative; display: grid; grid-template-columns: minmax(0, 1fr) auto; align-items: center; min-block-size: var(--bm-input-floating-height); }
    input { inline-size: 100%; block-size: 100%; box-sizing: border-box; padding: var(--bm-size-12) var(--bm-input-padding-inline) 0; color: inherit; background: transparent; border: 0; outline: 0; font: inherit; line-height: var(--bm-input-content-line-height); }
    label { position: absolute; inset-block-start: var(--bm-size-8); inset-inline: var(--bm-input-padding-inline) var(--bm-size-40); overflow: hidden; color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); line-height: var(--bm-input-content-line-height); text-overflow: ellipsis; white-space: nowrap; pointer-events: none; }
    .shell:focus-within label { color: var(--bm-color-focus); }
    :host([data-invalid]) label { color: var(--bm-color-error); }
    .toggle { display: grid; place-items: center; inline-size: var(--bm-button-height); block-size: var(--bm-button-height); margin-inline-end: var(--bm-size-4); color: var(--bm-color-text-secondary); background: transparent; border: 0; border-radius: var(--bm-button-radius); cursor: pointer; }
    .toggle:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); }
    .toggle span { transition: transform var(--bm-dropdown-transition-duration) var(--bm-motion-easing-standard); }
    :host([open]) .toggle span { transform: rotate(180deg); }
    .listbox { position: absolute; z-index: var(--bm-overlay-stack-level); inset-block-start: calc(100% + var(--bm-size-4)); inset-inline: 0; display: grid; box-sizing: border-box; max-block-size: 280px; overflow: auto; padding-block: var(--bm-dropdown-padding-block); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-border); border-radius: var(--bm-dropdown-radius); box-shadow: var(--bm-dropdown-shadow); }
    .option { display: flex; align-items: center; min-block-size: var(--bm-menu-item-height); padding-inline: var(--bm-input-padding-inline); color: var(--bm-color-text-primary); cursor: pointer; }
    .option[data-active] { background: var(--bm-color-accent-subtle); color: var(--bm-color-accent-on-subtle); }
    .option[aria-disabled='true'] { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    .empty { padding: var(--bm-size-12) var(--bm-input-padding-inline); color: var(--bm-color-text-secondary); }
    .error { display: flex; align-items: start; gap: var(--bm-size-8); padding: var(--bm-input-error-padding-block) var(--bm-input-padding-inline); color: var(--bm-color-error); border-block-start: var(--bm-input-error-divider-width) solid var(--bm-color-error); font-size: var(--bm-font-size-12); line-height: var(--bm-input-error-line-height); }
    .error-icon { display: inline-grid; flex: none; place-items: center; inline-size: var(--bm-input-error-icon-size); block-size: var(--bm-input-error-icon-size); border: var(--bm-size-1) solid currentColor; border-radius: 50%; font-weight: var(--bm-typography-weight-emphasis); }
    .options-source { display: none; }
    :host([disabled]), :host([data-form-disabled]) { opacity: var(--bm-content-opacity-disabled); }
    @media (pointer: coarse) { .field { min-block-size: var(--bm-interaction-target-minimum); } .option { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { .shell, .toggle span { transition: none; } }
  `;

  readonly #internals: ElementInternals;
  readonly #inputId = `bm-combobox-${++comboboxSequence}`;
  readonly #listboxId = `${this.#inputId}-listbox`;
  readonly #errorId = `${this.#inputId}-error`;
  readonly #observer: MutationObserver;
  #defaultValue = '';
  #formDisabled = false;
  #touched = false;
  #composing = false;

  #value = '';
  get value(): string { return this.#value; }
  set value(next: string) {
    const previous = this.#value;
    if (next === previous) return;
    this.#value = next;
    if (next) this.inputValue = this.#optionForValue(next)?.label ?? this.inputValue;
    this.requestUpdate('value', previous);
  }
  inputValue = '';
  label = '';
  name = '';
  placeholder = '';
  required = false;
  disabled = false;
  error = '';
  open = false;
  filter = true;
  protected _nativeError = '';
  protected _optionVersion = 0;
  protected _activeIndex = -1;

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
    document.addEventListener('pointerdown', this.#outside, true);
    this.#optionsChanged();
  }
  override disconnectedCallback(): void { this.#observer.disconnect(); document.removeEventListener('pointerdown', this.#outside, true); super.disconnectedCallback(); }

  get form(): HTMLFormElement | null { return this.#internals.form; }
  get validity(): ValidityState { return this.#internals.validity; }
  get validationMessage(): string { return this.#internals.validationMessage; }
  get willValidate(): boolean { return this.#internals.willValidate; }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector('input')?.focus(options); }
  setCustomValidity(message: string): void { this._nativeError = ''; this.error = message; this.#touched = Boolean(message); }
  checkValidity(): boolean { this.#syncValidity(); return this.#internals.checkValidity(); }
  reportValidity(): boolean { this.#touched = true; this.#syncValidity(); return this.#internals.reportValidity(); }
  formDisabledCallback(disabled: boolean): void { this.#formDisabled = disabled; this.toggleAttribute('data-form-disabled', disabled); this.close(); }
  formResetCallback(): void { this.value = this.#defaultValue; this.inputValue = this.#optionForValue(this.value)?.label ?? ''; this.error = ''; this._nativeError = ''; this.#touched = false; this.close(); }
  formStateRestoreCallback(state: string | File | FormData | null): void { if (typeof state === 'string') { this.value = state; this.inputValue = this.#optionForValue(state)?.label ?? ''; } }

  show(originalEvent?: Event): void { if (this.open || this.disabled || this.#formDisabled) return; this.open = true; this.#activateSelectedOrFirst(); this.#emitOpen(originalEvent); }
  close(originalEvent?: Event): void { if (!this.open) return; this.open = false; this._activeIndex = -1; this.#emitOpen(originalEvent); }

  override willUpdate(changed: PropertyValues<this>): void {
    if (changed.has('value') || changed.has('error') || changed.has('required') || changed.has('disabled')) this.#syncValidity();
  }

  #options(): BoldMergeOption[] { return [...this.querySelectorAll<BoldMergeOption>(':scope > bm-option')]; }
  #visibleOptions(): BoldMergeOption[] {
    const query = this.inputValue.trim().toLocaleLowerCase();
    return this.#options().filter((option) => !this.filter || !query || option.label.toLocaleLowerCase().includes(query));
  }
  #optionForValue(value: string): BoldMergeOption | undefined { return this.#options().find((option) => option.value === value); }
  #optionsChanged(): void {
    if (this.value) this.inputValue = this.#optionForValue(this.value)?.label ?? this.inputValue;
    this._optionVersion += 1;
    if (this.open) this.#activateSelectedOrFirst();
    this.#syncValidity();
  }
  #syncValidity(): void {
    const input = this.renderRoot.querySelector('input');
    this.#internals.setFormValue(this.value || null, this.value || null);
    if (this.error) { this.#internals.setValidity({ customError: true }, this.error, input ?? undefined); this._nativeError = ''; }
    else if (this.required && !this.value) { const message = 'Please select an option.'; this.#internals.setValidity({ valueMissing: true }, message, input ?? undefined); this._nativeError = this.#touched ? message : ''; }
    else { this.#internals.setValidity({}); this._nativeError = ''; }
    this.toggleAttribute('data-invalid', Boolean(this.error || this._nativeError));
  }
  #emitOpen(originalEvent?: Event): void { this.dispatchEvent(new CustomEvent<ComboboxOpenChangeDetail>('bm-combobox-open-change', { detail: { open: this.open, originalEvent }, bubbles: true, composed: true })); }
  #activateSelectedOrFirst(): void { const options = this.#visibleOptions(); const selected = options.findIndex((option) => option.value === this.value && !option.disabled); this._activeIndex = selected >= 0 ? selected : options.findIndex((option) => !option.disabled); }
  #move(step: number): void {
    const options = this.#visibleOptions(); if (!options.length) return;
    let index = this._activeIndex;
    for (let count = 0; count < options.length; count++) { index = (index + step + options.length) % options.length; if (!options[index]?.disabled) { this._activeIndex = index; return; } }
  }
  #boundary(end: boolean): void {
    const options = this.#visibleOptions();
    if (!end) { const index = options.findIndex((option) => !option.disabled); if (index >= 0) this._activeIndex = index; return; }
    for (let index = options.length - 1; index >= 0; index--) { if (!options[index]?.disabled) { this._activeIndex = index; return; } }
  }
  #select(option: BoldMergeOption, originalEvent: Event): void {
    if (option.disabled) return;
    this.value = option.value; this.inputValue = option.label; this.#touched = true; this.#syncValidity(); this.close(originalEvent);
    this.dispatchEvent(new CustomEvent<ComboboxChangeDetail>('bm-combobox-change', { detail: { value: option.value, label: option.label, originalEvent }, bubbles: true, composed: true }));
  }
  #input(event: InputEvent): void {
    this.inputValue = (event.currentTarget as HTMLInputElement).value; this.value = ''; this._activeIndex = -1;
    if (this.open) this.#activateSelectedOrFirst(); else this.show(event);
    if (!this.#composing) this.dispatchEvent(new CustomEvent<ComboboxInputDetail>('bm-combobox-input', { detail: { inputValue: this.inputValue, originalEvent: event }, bubbles: true, composed: true }));
  }
  #compositionEnd(event: CompositionEvent): void {
    this.#composing = false; this.inputValue = (event.currentTarget as HTMLInputElement).value; this._activeIndex = -1;
    if (this.open) this.#activateSelectedOrFirst(); else this.show(event);
    this.dispatchEvent(new CustomEvent<ComboboxInputDetail>('bm-combobox-input', { detail: { inputValue: this.inputValue, originalEvent: event }, bubbles: true, composed: true }));
  }
  #keyDown(event: KeyboardEvent): void {
    if (this.#composing || event.isComposing) return;
    if (event.key === 'ArrowDown' || event.key === 'ArrowUp') { event.preventDefault(); if (!this.open) this.show(event); else this.#move(event.key === 'ArrowDown' ? 1 : -1); }
    else if (event.key === 'Home' && this.open) { event.preventDefault(); this.#boundary(false); }
    else if (event.key === 'End' && this.open) { event.preventDefault(); this.#boundary(true); }
    else if (event.key === 'Enter' && this.open) { const option = this.#visibleOptions()[this._activeIndex]; if (option && !option.disabled) { event.preventDefault(); this.#select(option, event); } }
    else if (event.key === 'Escape' && this.open) { event.preventDefault(); this.close(event); }
    else if (event.key === 'Tab') this.close(event);
  }
  #blur(): void { this.#touched = true; this.#syncValidity(); queueMicrotask(() => { if (!this.matches(':focus-within')) this.close(); }); }
  #outside = (event: PointerEvent): void => { if (!event.composedPath().includes(this)) this.close(event); };
  #activeId(options: BoldMergeOption[]): string | undefined { return this.open && this._activeIndex >= 0 ? `${this.#inputId}-option-${options.indexOf(options[this._activeIndex]!)}` : undefined; }

  override render() {
    const message = this.error || this._nativeError;
    const unavailable = this.disabled || this.#formDisabled;
    const options = this.#visibleOptions();
    const activeId = this.#activeId(options);
    return html`<div class="shell" part="container"><div class="field" part="field"><input id=${this.#inputId} part="input" role="combobox" .value=${this.inputValue} placeholder=${this.placeholder || nothing} ?disabled=${unavailable} aria-label=${this.label || nothing} aria-expanded=${this.open ? 'true' : 'false'} aria-controls=${this.#listboxId} aria-autocomplete="list" aria-activedescendant=${activeId || nothing} aria-invalid=${message ? 'true' : 'false'} aria-errormessage=${message ? this.#errorId : nothing} @input=${this.#input} @keydown=${this.#keyDown} @blur=${this.#blur} @compositionstart=${() => { this.#composing = true; }} @compositionend=${this.#compositionEnd}><label for=${this.#inputId} part="label">${this.label}${this.required ? ' *' : ''}</label><button class="toggle" part="toggle" type="button" tabindex="-1" aria-label="Show options" ?disabled=${unavailable} @pointerdown=${(event: PointerEvent) => event.preventDefault()} @click=${(event: MouseEvent) => this.open ? this.close(event) : this.show(event)}><span aria-hidden="true">⌄</span></button></div>${message ? html`<div id=${this.#errorId} class="error" part="error"><span class="error-icon" part="error-icon" aria-hidden="true">!</span><span>${message}</span></div>` : nothing}</div>${this.open ? html`<div id=${this.#listboxId} class="listbox" part="listbox" role="listbox" aria-label=${this.label || nothing}>${options.length ? options.map((option, index) => html`<div id=${`${this.#inputId}-option-${index}`} class="option" part="option" role="option" aria-selected=${option.value === this.value ? 'true' : 'false'} aria-disabled=${option.disabled ? 'true' : 'false'} ?data-active=${index === this._activeIndex} @pointerdown=${(event: PointerEvent) => event.preventDefault()} @click=${(event: MouseEvent) => this.#select(option, event)}>${option.label}</div>`) : html`<div class="empty" part="empty"><slot name="empty">No options</slot></div>`}</div>` : nothing}<div class="options-source" aria-hidden="true"><slot></slot></div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-combobox': BoldMergeCombobox; }
  interface HTMLElementEventMap { 'bm-combobox-input': CustomEvent<ComboboxInputDetail>; 'bm-combobox-change': CustomEvent<ComboboxChangeDetail>; 'bm-combobox-open-change': CustomEvent<ComboboxOpenChangeDetail>; }
}
