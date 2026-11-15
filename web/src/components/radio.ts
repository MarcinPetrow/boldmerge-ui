import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export interface RadioChangeDetail {
  checked: boolean;
  value: string;
  originalEvent: Event;
}

export interface RadioGroupChangeDetail {
  value: string;
  originalEvent: Event;
}

export class BoldMergeRadio extends LitElement {
  static properties = {
    checked: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
    value: { type: String },
    _groupDisabled: { state: true },
    _managed: { state: true },
    _tabStop: { state: true },
  };

  static styles = css`
    :host { display: inline-block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    label { position: relative; display: inline-flex; align-items: start; gap: var(--bm-radio-content-gap); line-height: var(--bm-radio-content-line-height); cursor: pointer; user-select: none; }
    input { position: absolute; overflow: hidden; inline-size: var(--bm-size-1); block-size: var(--bm-size-1); margin: 0; opacity: 0; clip-path: inset(50%); }
    .control { display: inline-grid; flex: none; place-items: center; box-sizing: border-box; inline-size: var(--bm-radio-size); block-size: var(--bm-radio-size); margin-block-start: var(--bm-size-1); background: var(--bm-color-surface-subtle); border: var(--bm-radio-border-width) solid var(--bm-color-border-strong); border-radius: 50%; transition: border-color var(--bm-radio-transition-duration) var(--bm-motion-easing-standard), background-color var(--bm-radio-transition-duration) var(--bm-motion-easing-standard); }
    .indicator { inline-size: var(--bm-radio-indicator-size); block-size: var(--bm-radio-indicator-size); background: var(--bm-color-accent-on-solid); border-radius: 50%; opacity: 0; transform: scale(.5); transition: opacity var(--bm-radio-transition-duration) var(--bm-motion-easing-standard), transform var(--bm-radio-transition-duration) var(--bm-motion-easing-standard); }
    :host([checked]) .control { background: var(--bm-color-accent-solid); border-color: var(--bm-color-accent-solid); }
    :host([checked]) .indicator { opacity: 1; transform: scale(1); }
    :host(:not([disabled]):not([data-group-disabled])) label:hover .control { border-color: var(--bm-color-accent-text); }
    input:focus-visible + .control { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    :host([disabled]), :host([data-group-disabled]) { opacity: var(--bm-content-opacity-disabled); }
    :host([disabled]) label, :host([data-group-disabled]) label { cursor: not-allowed; }
    @media (pointer: coarse) { label { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { .control, .indicator { transition: none; } }
  `;

  checked = false;
  disabled = false;
  value = 'on';
  protected _groupDisabled = false;
  protected _managed = false;
  protected _tabStop = false;

  get unavailable(): boolean { return this.disabled || this._groupDisabled; }

  focus(options?: FocusOptions): void { this.renderRoot.querySelector('input')?.focus(options); }
  click(): void { this.renderRoot.querySelector('input')?.click(); }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('disabled') || changed.has('value')) {
      queueMicrotask(() => this.dispatchEvent(new CustomEvent('bm-radio-state-change', { bubbles: true, composed: true })));
    }
  }

  /** @internal Used by bm-radio-group to establish one roving tab stop. */
  setGroupState(disabled: boolean, tabStop: boolean): void {
    this._groupDisabled = disabled;
    this._managed = true;
    this._tabStop = tabStop;
    this.toggleAttribute('data-group-disabled', disabled);
  }

  #change(event: Event): void {
    this.checked = (event.currentTarget as HTMLInputElement).checked;
    this.dispatchEvent(new CustomEvent<RadioChangeDetail>('bm-radio-change', {
      detail: { checked: this.checked, value: this.value, originalEvent: event },
      bubbles: true,
      composed: true,
    }));
  }

  override render() {
    return html`
      <label part="label">
        <input type="radio" .checked=${this.checked} .value=${this.value} ?disabled=${this.unavailable} tabindex=${this._managed ? (this._tabStop && !this.unavailable ? 0 : -1) : nothing} @change=${this.#change} />
        <span class="control" part="control" aria-hidden="true"><span class="indicator" part="indicator"></span></span>
        <span part="label-text"><slot></slot></span>
      </label>
    `;
  }
}

export class BoldMergeRadioGroup extends LitElement {
  static formAssociated = true;

  static properties = {
    value: { type: String },
    name: { type: String, reflect: true },
    label: { type: String },
    required: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
  };

  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    fieldset { display: grid; gap: var(--bm-size-8); min-inline-size: 0; margin: 0; padding: 0; border: 0; }
    legend { margin-block-end: var(--bm-size-8); padding: 0; color: var(--bm-color-text-primary); font-weight: var(--bm-typography-weight-emphasis); }
    :host([data-invalid]) fieldset { outline: var(--bm-size-1) solid var(--bm-color-error); outline-offset: var(--bm-size-4); }
  `;

  readonly #internals: ElementInternals;
  #defaultValue = '';
  #formDisabled = false;
  #customError = '';

  value = '';
  name = '';
  label = '';
  required = false;
  disabled = false;

  constructor() {
    super();
    this.#internals = this.attachInternals();
    this.addEventListener('bm-radio-change', this.#radioChange as EventListener);
    this.addEventListener('bm-radio-state-change', () => this.#sync());
    this.addEventListener('keydown', this.#keydown);
    this.addEventListener('invalid', () => this.toggleAttribute('data-invalid', true));
  }

  override connectedCallback(): void {
    super.connectedCallback();
    this.#defaultValue = this.getAttribute('value') ?? '';
  }

  get form(): HTMLFormElement | null { return this.#internals.form; }
  get validity(): ValidityState { return this.#internals.validity; }
  get validationMessage(): string { return this.#internals.validationMessage; }
  get willValidate(): boolean { return this.#internals.willValidate; }

  focus(options?: FocusOptions): void { (this.#selectedRadio() ?? this.#enabledRadios()[0])?.focus(options); }
  setCustomValidity(message: string): void { this.#customError = message; this.#sync(); this.toggleAttribute('data-invalid', Boolean(message)); }
  checkValidity(): boolean { this.#sync(); return this.#internals.checkValidity(); }
  reportValidity(): boolean { this.#sync(); const valid = this.#internals.reportValidity(); this.toggleAttribute('data-invalid', !valid); return valid; }
  formDisabledCallback(disabled: boolean): void { this.#formDisabled = disabled; this.#sync(); }
  formResetCallback(): void { this.value = this.#defaultValue; this.#customError = ''; this.toggleAttribute('data-invalid', false); }
  formStateRestoreCallback(state: string | File | FormData | null): void { if (typeof state === 'string') this.value = state; }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('value') || changed.has('required') || changed.has('disabled')) this.#sync();
  }

  #radios(): BoldMergeRadio[] { return [...this.querySelectorAll<BoldMergeRadio>('bm-radio')]; }
  #enabledRadios(): BoldMergeRadio[] { return this.#radios().filter((radio) => !radio.disabled && !this.disabled && !this.#formDisabled); }
  #selectedRadio(): BoldMergeRadio | undefined { return this.#radios().find((radio) => radio.value === this.value && radio.checked); }

  #sync(): void {
    const radios = this.#radios();
    if (!this.value) this.value = radios.find((radio) => radio.checked)?.value ?? '';
    const unavailable = this.disabled || this.#formDisabled;
    const enabled = radios.filter((radio) => !radio.disabled && !unavailable);
    let selected = radios.find((radio) => radio.value === this.value);
    if (selected?.disabled) selected = undefined;
    radios.forEach((radio) => {
      radio.checked = radio === selected;
      radio.setGroupState(unavailable, radio === (selected ?? enabled[0]));
    });
    const selectedValue = selected?.value ?? '';
    this.#internals.setFormValue(selectedValue || null, selectedValue || null);
    const anchor = this.renderRoot.querySelector('fieldset') ?? undefined;
    if (this.#customError) this.#internals.setValidity({ customError: true }, this.#customError, anchor);
    else if (this.required && !selectedValue) this.#internals.setValidity({ valueMissing: true }, 'Select an option.', anchor);
    else { this.#internals.setValidity({}); this.toggleAttribute('data-invalid', false); }
  }

  #radioChange = (event: CustomEvent<RadioChangeDetail>): void => {
    const radio = event.target as BoldMergeRadio;
    if (!event.detail.checked || !this.#radios().includes(radio)) return;
    this.#select(radio, event.detail.originalEvent);
  };

  #select(radio: BoldMergeRadio, originalEvent: Event): void {
    if (radio.disabled || this.disabled || this.#formDisabled) return;
    this.value = radio.value;
    this.#sync();
    this.dispatchEvent(new CustomEvent<RadioGroupChangeDetail>('bm-radio-group-change', {
      detail: { value: this.value, originalEvent }, bubbles: true, composed: true,
    }));
  }

  #keydown = (event: KeyboardEvent): void => {
    const current = event.composedPath().find((node) => node instanceof BoldMergeRadio) as BoldMergeRadio | undefined;
    if (!current) return;
    const radios = this.#enabledRadios();
    if (!radios.length) return;
    let index = radios.indexOf(current);
    if (event.key === 'Home') index = 0;
    else if (event.key === 'End') index = radios.length - 1;
    else if (event.key === 'ArrowRight' || event.key === 'ArrowDown') index = (index + 1) % radios.length;
    else if (event.key === 'ArrowLeft' || event.key === 'ArrowUp') index = (index - 1 + radios.length) % radios.length;
    else return;
    event.preventDefault();
    const next = radios[index];
    if (next) { this.#select(next, event); next.focus(); }
  };

  #slotChange(): void {
    if (!this.#defaultValue) this.#defaultValue = this.value || this.#radios().find((radio) => radio.checked)?.value || '';
    this.#sync();
  }

  override render() {
    return html`<fieldset part="group" role="radiogroup" aria-label=${this.label || nothing} aria-invalid=${this.hasAttribute('data-invalid') ? 'true' : 'false'} ?disabled=${this.disabled || this.#formDisabled}><legend part="label" ?hidden=${!this.label}>${this.label}${this.required ? html`<span aria-hidden="true"> *</span>` : null}</legend><slot @slotchange=${this.#slotChange}></slot></fieldset>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-radio': BoldMergeRadio; 'bm-radio-group': BoldMergeRadioGroup; }
  interface HTMLElementEventMap { 'bm-radio-change': CustomEvent<RadioChangeDetail>; 'bm-radio-group-change': CustomEvent<RadioGroupChangeDetail>; }
}
