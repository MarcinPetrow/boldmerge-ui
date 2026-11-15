import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export type InputType = 'email' | 'password' | 'search' | 'tel' | 'text' | 'url';

export interface InputValueDetail {
  value: string;
  originalEvent: Event;
}

let inputSequence = 0;

function validityFlags(validity: ValidityState): ValidityStateFlags {
  return {
    badInput: validity.badInput,
    customError: validity.customError,
    patternMismatch: validity.patternMismatch,
    rangeOverflow: validity.rangeOverflow,
    rangeUnderflow: validity.rangeUnderflow,
    stepMismatch: validity.stepMismatch,
    tooLong: validity.tooLong,
    tooShort: validity.tooShort,
    typeMismatch: validity.typeMismatch,
    valueMissing: validity.valueMissing,
  };
}

export class BoldMergeInput extends LitElement {
  static formAssociated = true;

  static properties = {
    value: { type: String },
    label: { type: String },
    type: { type: String },
    name: { type: String, reflect: true },
    placeholder: { type: String },
    autocomplete: { type: String },
    inputMode: { type: String, attribute: 'inputmode' },
    required: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
    readonly: { type: Boolean, reflect: true },
    error: { type: String },
    _nativeError: { state: true },
  };

  static styles = css`
    :host {
      display: block;
      color: var(--bm-color-text-primary);
      font-family: var(--bm-typography-family-ui);
    }

    .shell {
      overflow: hidden;
      box-sizing: border-box;
      background: var(--bm-color-surface-subtle);
      border: var(--bm-input-border-width) solid var(--bm-color-border-strong);
      border-radius: var(--bm-input-radius);
      transition:
        border-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard),
        outline-color var(--bm-input-transition-duration) var(--bm-motion-easing-standard);
    }

    .shell:focus-within {
      border-color: var(--bm-color-focus);
      outline: var(--bm-size-2) solid var(--bm-color-focus);
      outline-offset: var(--bm-size-2);
    }

    :host([data-invalid]) .shell {
      border-color: var(--bm-color-error);
    }

    .field {
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      align-items: center;
      block-size: var(--bm-input-floating-height);
      min-inline-size: 0;
      padding-inline: var(--bm-input-padding-inline);
      gap: var(--bm-input-content-gap);
    }

    .input-wrap {
      position: relative;
      align-self: stretch;
      min-inline-size: 0;
    }

    input {
      inline-size: 100%;
      block-size: 100%;
      box-sizing: border-box;
      padding: var(--bm-size-12) 0 0;
      color: var(--bm-color-text-primary);
      background: transparent;
      border: 0;
      outline: 0;
      font: inherit;
      line-height: var(--bm-input-content-line-height);
    }

    input::placeholder {
      color: transparent;
    }

    label {
      position: absolute;
      inset-block-start: 50%;
      inset-inline-start: 0;
      overflow: hidden;
      max-inline-size: 100%;
      color: var(--bm-color-text-secondary);
      line-height: var(--bm-input-content-line-height);
      text-overflow: ellipsis;
      white-space: nowrap;
      pointer-events: none;
      transform: translateY(-50%);
      transform-origin: inline-start center;
      transition:
        color var(--bm-input-transition-duration) var(--bm-motion-easing-standard),
        font-size var(--bm-input-transition-duration) var(--bm-motion-easing-standard),
        inset-block-start var(--bm-input-transition-duration) var(--bm-motion-easing-standard),
        transform var(--bm-input-transition-duration) var(--bm-motion-easing-standard);
    }

    .shell:focus-within label,
    :host([data-filled]) label {
      inset-block-start: var(--bm-size-8);
      color: var(--bm-color-text-secondary);
      font-size: var(--bm-font-size-12);
      transform: none;
    }

    .shell:focus-within label {
      color: var(--bm-color-focus);
    }

    :host([data-invalid]) label {
      color: var(--bm-color-error);
    }

    .error {
      display: flex;
      align-items: start;
      gap: var(--bm-size-8);
      padding: var(--bm-input-error-padding-block) var(--bm-input-padding-inline);
      color: var(--bm-color-error);
      border-block-start: var(--bm-input-error-divider-width) solid var(--bm-color-error);
      font-size: var(--bm-font-size-12);
      line-height: var(--bm-input-error-line-height);
    }

    .error-icon {
      display: inline-grid;
      flex: none;
      place-items: center;
      inline-size: var(--bm-input-error-icon-size);
      block-size: var(--bm-input-error-icon-size);
      border: var(--bm-size-1) solid currentColor;
      border-radius: 50%;
      font-weight: var(--bm-typography-weight-emphasis);
      line-height: var(--bm-input-content-line-height);
    }

    :host([disabled]) {
      opacity: var(--bm-content-opacity-disabled);
    }

    :host([disabled]) .shell {
      cursor: not-allowed;
    }

    ::slotted([slot='leading']),
    ::slotted([slot='trailing']) {
      inline-size: var(--bm-icon-size-default);
      block-size: var(--bm-icon-size-default);
      color: var(--bm-color-text-secondary);
    }

    @media (pointer: coarse) {
      .field { min-block-size: var(--bm-interaction-target-minimum); }
    }
  `;

  readonly #internals: ElementInternals;
  readonly #inputId = `bm-input-${++inputSequence}`;
  readonly #errorId = `${this.#inputId}-error`;
  #defaultValue = '';
  #formDisabled = false;
  #touched = false;

  value = '';
  label = '';
  type: InputType = 'text';
  name = '';
  placeholder = '';
  autocomplete = '';
  inputMode = '';
  required = false;
  disabled = false;
  readonly = false;
  error = '';
  protected _nativeError = '';

  constructor() {
    super();
    this.#internals = this.attachInternals();
    this.addEventListener('invalid', this.#invalid);
  }

  override connectedCallback(): void {
    super.connectedCallback();
    this.#defaultValue = this.getAttribute('value') ?? this.value;
  }

  get form(): HTMLFormElement | null {
    return this.#internals.form;
  }

  get validity(): ValidityState {
    return this.#internals.validity;
  }

  get validationMessage(): string {
    return this.#internals.validationMessage;
  }

  get willValidate(): boolean {
    return this.#internals.willValidate;
  }

  focus(options?: FocusOptions): void {
    this.renderRoot.querySelector('input')?.focus(options);
  }

  select(): void {
    this.renderRoot.querySelector('input')?.select();
  }

  setCustomValidity(message: string): void {
    this._nativeError = '';
    this.error = message;
    this.#touched = Boolean(message);
  }

  checkValidity(): boolean {
    this.#syncValidity();
    return this.#internals.checkValidity();
  }

  reportValidity(): boolean {
    this.#touched = true;
    this.#syncValidity();
    return this.#internals.reportValidity();
  }

  formDisabledCallback(disabled: boolean): void {
    this.#formDisabled = disabled;
    this.requestUpdate();
  }

  formResetCallback(): void {
    this.value = this.#defaultValue;
    this.error = '';
    this._nativeError = '';
    this.#touched = false;
  }

  formStateRestoreCallback(state: string | File | FormData | null): void {
    if (typeof state === 'string') this.value = state;
  }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('value') || changed.has('error') || changed.has('required') || changed.has('type') || changed.has('disabled')) {
      this.toggleAttribute('data-filled', Boolean(this.value));
      this.#syncValidity();
    }
  }

  #syncValidity(): void {
    const input = this.renderRoot.querySelector('input');
    this.#internals.setFormValue(this.value);
    if (this.error) {
      this.#internals.setValidity({ customError: true }, this.error, input ?? undefined);
      this._nativeError = '';
    } else if (input && !input.validity.valid) {
      this.#internals.setValidity(validityFlags(input.validity), input.validationMessage, input);
      this._nativeError = this.#touched ? input.validationMessage : '';
    } else {
      this.#internals.setValidity({});
      this._nativeError = '';
    }
    this.toggleAttribute('data-invalid', Boolean(this.error || this._nativeError));
  }

  #input(event: InputEvent): void {
    const target = event.currentTarget as HTMLInputElement;
    this.value = target.value;
    if (target.validity.valid) this._nativeError = '';
    else if (this.#touched) this.#syncValidity();
    this.dispatchEvent(new CustomEvent<InputValueDetail>('bm-input', {
      detail: { value: this.value, originalEvent: event },
      bubbles: true,
      composed: true,
    }));
  }

  #change(event: Event): void {
    this.dispatchEvent(new CustomEvent<InputValueDetail>('bm-change', {
      detail: { value: this.value, originalEvent: event },
      bubbles: true,
      composed: true,
    }));
  }

  #blur(): void {
    this.#touched = true;
    this.#syncValidity();
  }

  readonly #invalid = (): void => {
    this.#touched = true;
    this.#syncValidity();
  };

  override render() {
    const message = this.error || this._nativeError;
    const disabled = this.disabled || this.#formDisabled;
    return html`
      <div class="shell" part="container">
        <div class="field" part="field">
          <slot name="leading"></slot>
          <div class="input-wrap">
            <input
              id=${this.#inputId}
              part="input"
              .value=${this.value}
              type=${this.type}
              name=${this.name}
              placeholder=${this.placeholder || ' '}
              autocomplete=${this.autocomplete || nothing}
              inputmode=${this.inputMode || nothing}
              ?required=${this.required}
              ?disabled=${disabled}
              ?readonly=${this.readonly}
              aria-invalid=${message ? 'true' : 'false'}
              aria-describedby=${message ? this.#errorId : nothing}
              aria-errormessage=${message ? this.#errorId : nothing}
              @input=${this.#input}
              @change=${this.#change}
              @blur=${this.#blur}
            />
            <label part="label" for=${this.#inputId}>${this.label}${this.required ? html`<span aria-hidden="true"> *</span>` : null}</label>
          </div>
          <slot name="trailing"></slot>
        </div>
        ${message ? html`
          <div class="error" id=${this.#errorId} part="error" role="alert">
            <span class="error-icon" part="error-icon" aria-hidden="true"><slot name="error-icon">!</slot></span>
            <span>${message}</span>
          </div>
        ` : null}
      </div>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'bm-input': BoldMergeInput;
  }

  interface HTMLElementEventMap {
    'bm-input': CustomEvent<InputValueDetail>;
    'bm-change': CustomEvent<InputValueDetail>;
  }
}
