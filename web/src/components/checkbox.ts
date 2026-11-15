import { LitElement, css, html, type PropertyValues } from 'lit';

export interface CheckboxChangeDetail {
  checked: boolean;
  indeterminate: boolean;
  value: string;
  originalEvent: Event;
}

export class BoldMergeCheckbox extends LitElement {
  static formAssociated = true;

  static properties = {
    checked: { type: Boolean, reflect: true },
    indeterminate: { type: Boolean, reflect: true },
    disabled: { type: Boolean, reflect: true },
    required: { type: Boolean, reflect: true },
    name: { type: String, reflect: true },
    value: { type: String },
  };

  static styles = css`
    :host {
      display: inline-block;
      color: var(--bm-color-text-primary);
      font-family: var(--bm-typography-family-ui);
    }

    label {
      position: relative;
      display: inline-flex;
      align-items: start;
      gap: var(--bm-checkbox-content-gap);
      color: var(--bm-color-text-primary);
      line-height: var(--bm-checkbox-content-line-height);
      cursor: pointer;
      user-select: none;
    }

    input {
      position: absolute;
      overflow: hidden;
      inline-size: var(--bm-size-1);
      block-size: var(--bm-size-1);
      margin: 0;
      opacity: 0;
      clip-path: inset(50%);
    }

    .control {
      display: inline-grid;
      flex: none;
      place-items: center;
      box-sizing: border-box;
      inline-size: var(--bm-checkbox-size);
      block-size: var(--bm-checkbox-size);
      margin-block-start: var(--bm-size-1);
      color: var(--bm-color-accent-on-solid);
      background: var(--bm-color-surface-subtle);
      border: var(--bm-checkbox-border-width) solid var(--bm-color-border-strong);
      border-radius: var(--bm-checkbox-radius);
      transition:
        color var(--bm-checkbox-transition-duration) var(--bm-motion-easing-standard),
        background-color var(--bm-checkbox-transition-duration) var(--bm-motion-easing-standard),
        border-color var(--bm-checkbox-transition-duration) var(--bm-motion-easing-standard);
    }

    .indicator {
      display: block;
      box-sizing: border-box;
      inline-size: var(--bm-checkbox-mark-width);
      block-size: var(--bm-checkbox-mark-height);
      border: solid currentColor;
      border-width: 0 var(--bm-checkbox-mark-stroke) var(--bm-checkbox-mark-stroke) 0;
      opacity: 0;
      transform: rotate(var(--bm-checkbox-mark-rotation));
    }

    :host([checked]) .control,
    :host([indeterminate]) .control {
      background: var(--bm-color-accent-solid);
      border-color: var(--bm-color-accent-solid);
    }

    :host([checked]) .indicator,
    :host([indeterminate]) .indicator {
      opacity: 1;
    }

    :host([indeterminate]) .indicator {
      inline-size: var(--bm-checkbox-indeterminate-width);
      block-size: var(--bm-checkbox-mark-stroke);
      background: currentColor;
      border: 0;
      transform: none;
    }

    :host(:not([disabled]):not([data-form-disabled])) label:hover .control {
      border-color: var(--bm-color-accent-text);
    }

    :host(:not([disabled]):not([data-form-disabled])[checked]) label:hover .control,
    :host(:not([disabled]):not([data-form-disabled])[indeterminate]) label:hover .control {
      background: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-checkbox-state-hover-mix));
      border-color: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-checkbox-state-hover-mix));
    }

    input:focus-visible + .control {
      outline: var(--bm-size-2) solid var(--bm-color-focus);
      outline-offset: var(--bm-size-2);
    }

    :host([data-invalid]) .control {
      border-color: var(--bm-color-error);
    }

    :host([disabled]),
    :host([data-form-disabled]) {
      opacity: var(--bm-content-opacity-disabled);
    }

    :host([disabled]) label,
    :host([data-form-disabled]) label {
      cursor: not-allowed;
    }

    @media (pointer: coarse) {
      label { min-block-size: var(--bm-interaction-target-minimum); }
    }
  `;

  readonly #internals: ElementInternals;
  #defaultChecked = false;
  #formDisabled = false;
  #customError = '';

  checked = false;
  indeterminate = false;
  disabled = false;
  required = false;
  name = '';
  value = 'on';

  constructor() {
    super();
    this.#internals = this.attachInternals();
    this.addEventListener('invalid', this.#invalid);
  }

  override connectedCallback(): void {
    super.connectedCallback();
    this.#defaultChecked = this.hasAttribute('checked');
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

  click(): void {
    this.renderRoot.querySelector('input')?.click();
  }

  setCustomValidity(message: string): void {
    this.#customError = message;
    this.#syncValidity();
    this.#setInvalid(Boolean(message));
  }

  checkValidity(): boolean {
    this.#syncValidity();
    return this.#internals.checkValidity();
  }

  reportValidity(): boolean {
    this.#syncValidity();
    const valid = this.#internals.reportValidity();
    this.#setInvalid(!valid);
    return valid;
  }

  formDisabledCallback(disabled: boolean): void {
    this.#formDisabled = disabled;
    this.toggleAttribute('data-form-disabled', disabled);
    this.requestUpdate();
  }

  formResetCallback(): void {
    this.checked = this.#defaultChecked;
    this.indeterminate = false;
    this.#customError = '';
    this.#setInvalid(false);
  }

  formStateRestoreCallback(state: string | File | FormData | null): void {
    this.checked = state === 'checked';
    this.indeterminate = false;
  }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('checked') || changed.has('indeterminate') || changed.has('required') || changed.has('disabled') || changed.has('value')) {
      this.#syncValidity();
    }
  }

  #syncValidity(): void {
    const input = this.renderRoot.querySelector('input');
    this.#internals.setFormValue(this.checked ? this.value : null, this.checked ? 'checked' : 'unchecked');
    if (this.#customError) {
      this.#internals.setValidity({ customError: true }, this.#customError, input ?? undefined);
    } else if (this.required && !this.checked) {
      const message = input?.validationMessage ?? '';
      this.#internals.setValidity({ valueMissing: true }, message, input ?? undefined);
    } else {
      this.#internals.setValidity({});
      this.#setInvalid(false);
    }
  }

  #change(event: Event): void {
    const input = event.currentTarget as HTMLInputElement;
    this.checked = input.checked;
    this.indeterminate = input.indeterminate;
    this.#syncValidity();
    this.#setInvalid(!this.#internals.validity.valid);
    this.dispatchEvent(new CustomEvent<CheckboxChangeDetail>('bm-checkbox-change', {
      detail: {
        checked: this.checked,
        indeterminate: this.indeterminate,
        value: this.value,
        originalEvent: event,
      },
      bubbles: true,
      composed: true,
    }));
  }

  readonly #invalid = (): void => {
    this.#setInvalid(true);
  };

  #setInvalid(invalid: boolean): void {
    this.toggleAttribute('data-invalid', invalid);
    this.renderRoot.querySelector('input')?.setAttribute('aria-invalid', invalid ? 'true' : 'false');
  }

  override render() {
    const disabled = this.disabled || this.#formDisabled;
    return html`
      <label part="label">
        <input
          type="checkbox"
          .checked=${this.checked}
          .indeterminate=${this.indeterminate}
          .value=${this.value}
          ?required=${this.required}
          ?disabled=${disabled}
          aria-invalid=${this.hasAttribute('data-invalid') ? 'true' : 'false'}
          @change=${this.#change}
        />
        <span class="control" part="control" aria-hidden="true">
          <span class="indicator" part="indicator"></span>
        </span>
        <span><slot></slot></span>
      </label>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'bm-checkbox': BoldMergeCheckbox;
  }

  interface HTMLElementEventMap {
    'bm-checkbox-change': CustomEvent<CheckboxChangeDetail>;
  }
}
