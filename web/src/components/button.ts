import { LitElement, css, html } from 'lit';

export type ButtonVariant = 'primary' | 'secondary' | 'outline' | 'ghost';
export type ButtonTreatment = 'default' | 'destructive';
export type ButtonType = 'button' | 'submit' | 'reset';

export interface ButtonActivateDetail {
  originalEvent: MouseEvent;
}

export class BoldMergeButton extends LitElement {
  static properties = {
    variant: { type: String, reflect: true },
    treatment: { type: String, reflect: true },
    type: { type: String },
    disabled: { type: Boolean, reflect: true },
    loading: { type: Boolean, reflect: true },
  };

  static styles = css`
    :host {
      display: inline-block;
      font-family: var(--bm-typography-family-ui);
    }

    button {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      box-sizing: border-box;
      block-size: var(--bm-button-height);
      min-inline-size: var(--bm-button-height);
      padding-inline: var(--bm-button-padding-inline);
      gap: var(--bm-button-content-gap);
      color: var(--bm-color-accent-on-solid);
      background: var(--bm-color-accent-solid);
      border: var(--bm-button-border-width) solid var(--bm-color-accent-solid);
      border-radius: var(--bm-button-radius);
      font: inherit;
      font-weight: var(--bm-typography-weight-emphasis);
      line-height: var(--bm-button-content-line-height);
      white-space: nowrap;
      cursor: pointer;
      transition:
        color var(--bm-button-transition-duration) var(--bm-motion-easing-standard),
        background-color var(--bm-button-transition-duration) var(--bm-motion-easing-standard),
        border-color var(--bm-button-transition-duration) var(--bm-motion-easing-standard);
    }

    button:hover:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-button-state-hover-mix));
      border-color: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-button-state-hover-mix));
    }

    button:active:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-button-state-press-mix));
      border-color: color-mix(in srgb, var(--bm-color-accent-solid), var(--bm-color-accent-on-solid) var(--bm-button-state-press-mix));
    }

    button:focus-visible {
      outline: var(--bm-size-2) solid var(--bm-color-focus);
      outline-offset: var(--bm-size-2);
    }

    :host([variant='secondary']) button {
      color: var(--bm-color-text-primary);
      background: var(--bm-color-surface-subtle);
      border-color: var(--bm-color-surface-subtle);
    }

    :host([variant='secondary']) button:hover:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-surface-subtle), var(--bm-color-text-primary) var(--bm-button-state-hover-mix));
      border-color: color-mix(in srgb, var(--bm-color-surface-subtle), var(--bm-color-text-primary) var(--bm-button-state-hover-mix));
    }

    :host([variant='secondary']) button:active:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-surface-subtle), var(--bm-color-text-primary) var(--bm-button-state-press-mix));
      border-color: color-mix(in srgb, var(--bm-color-surface-subtle), var(--bm-color-text-primary) var(--bm-button-state-press-mix));
    }

    :host([variant='outline']) button {
      color: var(--bm-color-accent-text);
      background: transparent;
      border-color: var(--bm-color-border-strong);
    }

    :host([variant='ghost']) button {
      color: var(--bm-color-accent-text);
      background: transparent;
      border-color: transparent;
    }

    :host([variant='outline']) button:hover:not(:disabled),
    :host([variant='ghost']) button:hover:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-accent-text) var(--bm-button-state-hover-mix), transparent);
    }

    :host([variant='outline']) button:active:not(:disabled),
    :host([variant='ghost']) button:active:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-accent-text) var(--bm-button-state-press-mix), transparent);
    }

    :host([treatment='destructive']) button {
      color: var(--bm-color-error);
    }

    :host([treatment='destructive'][variant='primary']) button {
      color: var(--bm-color-error-on-solid);
      background: var(--bm-color-error);
      border-color: var(--bm-color-error);
    }

    :host([treatment='destructive'][variant='primary']) button:hover:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-error), var(--bm-color-error-on-solid) var(--bm-button-state-hover-mix));
      border-color: color-mix(in srgb, var(--bm-color-error), var(--bm-color-error-on-solid) var(--bm-button-state-hover-mix));
    }

    :host([treatment='destructive'][variant='primary']) button:active:not(:disabled) {
      background: color-mix(in srgb, var(--bm-color-error), var(--bm-color-error-on-solid) var(--bm-button-state-press-mix));
      border-color: color-mix(in srgb, var(--bm-color-error), var(--bm-color-error-on-solid) var(--bm-button-state-press-mix));
    }

    :host([treatment='destructive'][variant='outline']) button {
      border-color: var(--bm-color-error);
    }

    :host([disabled]),
    :host([loading]) {
      cursor: not-allowed;
    }

    button:disabled {
      cursor: not-allowed;
      opacity: var(--bm-content-opacity-disabled);
    }

    .spinner {
      inline-size: var(--bm-button-loading-size);
      block-size: var(--bm-button-loading-size);
      border: var(--bm-size-2) solid currentColor;
      border-inline-end-color: transparent;
      border-radius: 50%;
      animation: spin var(--bm-button-loading-duration) linear infinite;
    }

    @keyframes spin {
      to { transform: rotate(1turn); }
    }

    @media (prefers-reduced-motion: reduce) {
      .spinner { animation: none; }
    }

    @media (pointer: coarse) {
      button { min-block-size: var(--bm-interaction-target-minimum); }
    }
  `;

  variant: ButtonVariant = 'primary';
  treatment: ButtonTreatment = 'default';
  type: ButtonType = 'button';
  disabled = false;
  loading = false;

  focus(options?: FocusOptions): void {
    this.renderRoot.querySelector('button')?.focus(options);
  }

  #activate(event: MouseEvent): void {
    if (this.disabled || this.loading) {
      event.preventDefault();
      event.stopImmediatePropagation();
      return;
    }
    this.dispatchEvent(new CustomEvent<ButtonActivateDetail>('bm-activate', {
      detail: { originalEvent: event },
      bubbles: true,
      composed: true,
    }));
    const form = this.closest('form');
    if (this.type === 'submit') form?.requestSubmit();
    else if (this.type === 'reset') form?.reset();
  }

  override render() {
    const unavailable = this.disabled || this.loading;
    return html`
      <button
        part="button"
        type="button"
        ?disabled=${unavailable}
        aria-disabled=${unavailable ? 'true' : 'false'}
        aria-busy=${this.loading ? 'true' : 'false'}
        @click=${this.#activate}
      >
        ${this.loading ? html`<span class="spinner" part="spinner" aria-hidden="true"></span>` : html`<slot name="prefix"></slot>`}
        <span part="label"><slot></slot></span>
        ${this.loading ? null : html`<slot name="suffix"></slot>`}
      </button>
    `;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'bm-button': BoldMergeButton;
  }

  interface HTMLElementEventMap {
    'bm-activate': CustomEvent<ButtonActivateDetail>;
  }
}
