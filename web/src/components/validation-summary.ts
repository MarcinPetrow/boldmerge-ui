import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export interface ValidatableControl extends HTMLElement {
  willValidate?: boolean;
  validity?: ValidityState;
  validationMessage?: string;
  reportValidity?: () => boolean;
  label?: string;
  name?: string;
}

export interface ValidationSummaryError { control: ValidatableControl; label: string; message: string; }
export interface ValidationSummaryChangeDetail { valid: boolean; count: number; errors: readonly ValidationSummaryError[]; }
export type ValidationFocusTarget = 'first' | 'summary' | 'none';

export class BoldMergeValidationSummary extends LitElement {
  static properties = {
    formId: { type: String, attribute: 'for' }, heading: { type: String }, fallbackLabel: { type: String, attribute: 'fallback-label' },
    _errors: { state: true },
  };
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    :host([hidden]) { display: none; }
    .summary { padding: var(--bm-validation-summary-padding); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-error); border-radius: var(--bm-validation-summary-radius); outline: none; }
    .summary:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); }
    .heading { color: var(--bm-color-error); font-weight: var(--bm-typography-weight-emphasis); }
    ul { display: grid; gap: var(--bm-validation-summary-item-gap); margin: var(--bm-size-12) 0 0; padding: 0; list-style: none; }
    button { padding: 0; color: var(--bm-color-error); background: transparent; border: 0; font: inherit; text-align: start; text-decoration: underline; cursor: pointer; }
    button:focus-visible { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: var(--bm-size-2); border-radius: var(--bm-radius-2); }
  `;
  formId = '';
  heading = 'Correct the following errors';
  fallbackLabel = 'Field';
  protected _errors: ValidationSummaryError[] = [];
  #form?: HTMLFormElement;
  #observer = new MutationObserver(() => this.refresh());
  #queued = false;
  #invalid = (): void => this.#queueRefresh();
  #change = (): void => this.#queueRefresh();
  #reset = (): void => { requestAnimationFrame(() => this.refresh()); };

  override connectedCallback(): void { super.connectedCallback(); queueMicrotask(() => this.#bind()); }
  override disconnectedCallback(): void { this.#unbind(); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void { if (changed.has('formId')) queueMicrotask(() => this.#bind()); }
  get errors(): readonly ValidationSummaryError[] { return this._errors; }
  get form(): HTMLFormElement | undefined { return this.#form; }
  validate(options: { focus?: ValidationFocusTarget } = {}): boolean {
    const valid = this.#form?.checkValidity() ?? true;
    this.refresh();
    if (!valid) requestAnimationFrame(() => { if (options.focus === 'summary') this.focus(); else if (options.focus !== 'none') this.focusFirstInvalid(); });
    return valid;
  }
  refresh(): void {
    const previous = this._errors.length;
    this._errors = this.#controls().filter((control) => control.willValidate !== false && control.validity && !control.validity.valid).map((control, index) => ({ control, label: this.#label(control, index), message: control.validationMessage || this.fallbackLabel }));
    this.hidden = this._errors.length === 0;
    if (previous !== this._errors.length) this.dispatchEvent(new CustomEvent<ValidationSummaryChangeDetail>('bm-validation-summary-change', { detail: { valid: this._errors.length === 0, count: this._errors.length, errors: this._errors }, bubbles: true, composed: true }));
  }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector<HTMLElement>('.summary')?.focus(options); }
  focusFirstInvalid(options?: FocusOptions): void { this._errors[0]?.control.focus(options); this._errors[0]?.control.reportValidity?.(); }
  #controls(): ValidatableControl[] { return this.#form ? [...this.#form.elements].filter((element): element is ValidatableControl => element instanceof HTMLElement) : []; }
  #label(control: ValidatableControl, index: number): string {
    if (typeof control.label === 'string' && control.label.trim()) return control.label.trim();
    const aria = control.getAttribute('aria-label'); if (aria?.trim()) return aria.trim();
    if (control.id) { const label = this.#form?.querySelector<HTMLLabelElement>(`label[for="${CSS.escape(control.id)}"]`); if (label?.textContent?.trim()) return label.textContent.trim(); }
    const text = control.textContent?.replace(/\s+/g, ' ').trim(); if (text) return text;
    if (control.name) return control.name;
    return `${this.fallbackLabel} ${index + 1}`;
  }
  #bind(): void {
    const next = this.formId ? document.getElementById(this.formId) : undefined;
    if (next === this.#form) { this.refresh(); return; }
    this.#unbind();
    if (!(next instanceof HTMLFormElement)) { this.hidden = true; return; }
    this.#form = next;
    next.addEventListener('invalid', this.#invalid, true); next.addEventListener('input', this.#change); next.addEventListener('change', this.#change); next.addEventListener('reset', this.#reset);
    this.#observer.observe(next, { childList: true, subtree: true, attributes: true, attributeFilter: ['required', 'disabled', 'value', 'checked'] });
    this.refresh();
  }
  #unbind(): void {
    if (this.#form) { this.#form.removeEventListener('invalid', this.#invalid, true); this.#form.removeEventListener('input', this.#change); this.#form.removeEventListener('change', this.#change); this.#form.removeEventListener('reset', this.#reset); }
    this.#observer.disconnect(); this.#form = undefined;
  }
  #queueRefresh(): void { if (this.#queued) return; this.#queued = true; queueMicrotask(() => { this.#queued = false; this.refresh(); }); }
  #activate(error: ValidationSummaryError): void { error.control.focus(); error.control.reportValidity?.(); }
  override render() {
    if (!this._errors.length) return nothing;
    return html`<div class="summary" part="summary" role="alert" tabindex="-1"><div class="heading" part="heading"><slot name="heading">${this.heading}</slot></div><ul part="list">${this._errors.map((error) => html`<li part="item"><button part="link" type="button" @click=${() => this.#activate(error)}>${error.label}: ${error.message}</button></li>`)}</ul></div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-validation-summary': BoldMergeValidationSummary; }
  interface HTMLElementEventMap { 'bm-validation-summary-change': CustomEvent<ValidationSummaryChangeDetail>; }
}
