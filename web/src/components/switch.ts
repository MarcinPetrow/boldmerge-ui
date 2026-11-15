import { LitElement, css, html, type PropertyValues } from 'lit';

export interface SwitchChangeDetail { checked: boolean; value: string; originalEvent: Event; }

export class BoldMergeSwitch extends LitElement {
  static formAssociated = true;
  static properties = {
    checked: { type: Boolean, reflect: true }, disabled: { type: Boolean, reflect: true },
    required: { type: Boolean, reflect: true }, name: { type: String, reflect: true }, value: { type: String },
  };
  static styles = css`
    :host { display: inline-block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    label { position: relative; display: inline-flex; align-items: start; gap: var(--bm-switch-content-gap); line-height: var(--bm-switch-content-line-height); cursor: pointer; user-select: none; }
    input { position: absolute; overflow: hidden; inline-size: var(--bm-size-1); block-size: var(--bm-size-1); margin: 0; opacity: 0; clip-path: inset(50%); }
    .track { position: relative; flex: none; box-sizing: border-box; inline-size: var(--bm-switch-width); block-size: var(--bm-switch-height); background: var(--bm-color-surface-subtle); border: var(--bm-size-1) solid var(--bm-color-border-strong); border-radius: var(--bm-switch-radius); transition: background-color var(--bm-switch-transition-duration) var(--bm-motion-easing-standard), border-color var(--bm-switch-transition-duration) var(--bm-motion-easing-standard); }
    .thumb { position: absolute; inset-block-start: var(--bm-switch-inset); inset-inline-start: var(--bm-switch-inset); inline-size: var(--bm-switch-thumb-size); block-size: var(--bm-switch-thumb-size); background: var(--bm-color-text-secondary); border-radius: 50%; transition: inset-inline-start var(--bm-switch-transition-duration) var(--bm-motion-easing-standard), background-color var(--bm-switch-transition-duration) var(--bm-motion-easing-standard); }
    :host([checked]) .track { background: var(--bm-color-accent-solid); border-color: var(--bm-color-accent-solid); }
    :host([checked]) .thumb { inset-inline-start: calc(var(--bm-switch-width) - var(--bm-switch-thumb-size) - var(--bm-switch-inset) - var(--bm-size-2)); background: var(--bm-color-accent-on-solid); }
    input:focus-visible + .track { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    :host([data-invalid]) .track { border-color: var(--bm-color-error); }
    :host([disabled]), :host([data-form-disabled]) { opacity: var(--bm-content-opacity-disabled); }
    :host([disabled]) label, :host([data-form-disabled]) label { cursor: not-allowed; }
    @media (pointer: coarse) { label { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { .track, .thumb { transition: none; } }
  `;

  readonly #internals: ElementInternals;
  #defaultChecked = false;
  #formDisabled = false;
  #customError = '';
  checked = false; disabled = false; required = false; name = ''; value = 'on';

  constructor() { super(); this.#internals = this.attachInternals(); this.addEventListener('invalid', () => this.#setInvalid(true)); }
  override connectedCallback(): void { super.connectedCallback(); this.#defaultChecked = this.hasAttribute('checked'); }
  get form(): HTMLFormElement | null { return this.#internals.form; }
  get validity(): ValidityState { return this.#internals.validity; }
  get validationMessage(): string { return this.#internals.validationMessage; }
  get willValidate(): boolean { return this.#internals.willValidate; }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector('input')?.focus(options); }
  click(): void { this.renderRoot.querySelector('input')?.click(); }
  setCustomValidity(message: string): void { this.#customError = message; this.#sync(); this.#setInvalid(Boolean(message)); }
  checkValidity(): boolean { this.#sync(); return this.#internals.checkValidity(); }
  reportValidity(): boolean { this.#sync(); const valid = this.#internals.reportValidity(); this.#setInvalid(!valid); return valid; }
  formDisabledCallback(disabled: boolean): void { this.#formDisabled = disabled; this.toggleAttribute('data-form-disabled', disabled); this.requestUpdate(); }
  formResetCallback(): void { this.checked = this.#defaultChecked; this.#customError = ''; this.#setInvalid(false); }
  formStateRestoreCallback(state: string | File | FormData | null): void { this.checked = state === 'checked'; }
  override updated(changed: PropertyValues<this>): void { if (changed.has('checked') || changed.has('required') || changed.has('disabled') || changed.has('value')) this.#sync(); }

  #sync(): void {
    const input = this.renderRoot.querySelector('input');
    this.#internals.setFormValue(this.checked ? this.value : null, this.checked ? 'checked' : 'unchecked');
    if (this.#customError) this.#internals.setValidity({ customError: true }, this.#customError, input ?? undefined);
    else if (this.required && !this.checked) this.#internals.setValidity({ valueMissing: true }, input?.validationMessage ?? 'Enable this option.', input ?? undefined);
    else { this.#internals.setValidity({}); this.#setInvalid(false); }
  }
  #change(event: Event): void {
    this.checked = (event.currentTarget as HTMLInputElement).checked; this.#sync(); this.#setInvalid(!this.#internals.validity.valid);
    this.dispatchEvent(new CustomEvent<SwitchChangeDetail>('bm-switch-change', { detail: { checked: this.checked, value: this.value, originalEvent: event }, bubbles: true, composed: true }));
  }
  #setInvalid(invalid: boolean): void { this.toggleAttribute('data-invalid', invalid); this.renderRoot.querySelector('input')?.setAttribute('aria-invalid', invalid ? 'true' : 'false'); }
  override render() {
    const unavailable = this.disabled || this.#formDisabled;
    return html`<label part="label"><input type="checkbox" role="switch" .checked=${this.checked} .value=${this.value} ?required=${this.required} ?disabled=${unavailable} aria-invalid=${this.hasAttribute('data-invalid') ? 'true' : 'false'} @change=${this.#change} /><span class="track" part="track" aria-hidden="true"><span class="thumb" part="thumb"></span></span><span part="label-text"><slot></slot></span></label>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-switch': BoldMergeSwitch; }
  interface HTMLElementEventMap { 'bm-switch-change': CustomEvent<SwitchChangeDetail>; }
}
