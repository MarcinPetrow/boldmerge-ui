import { LitElement, css, html, nothing } from 'lit';

let emptyStateId = 0;

export class BoldMergeEmptyState extends LitElement {
  static properties = {
    heading: { type: String }, description: { type: String }, label: { type: String },
    headingLevel: { type: Number, attribute: 'heading-level' },
    _hasIcon: { state: true }, _hasDescription: { state: true }, _hasAction: { state: true },
  };
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .state { display: grid; justify-items: center; max-inline-size: 560px; margin-inline: auto; padding: var(--bm-size-32) var(--bm-size-24); text-align: center; }
    .icon { display: grid; inline-size: var(--bm-size-48); block-size: var(--bm-size-48); place-items: center; margin-block-end: var(--bm-size-16); color: var(--bm-color-text-secondary); background: var(--bm-color-surface-subtle); border-radius: var(--bm-radius-8); }
    .icon[hidden], .description[hidden], .action[hidden] { display: none; }
    .title { font-weight: var(--bm-typography-weight-emphasis); line-height: var(--bm-line-height-compact); }
    .description { max-inline-size: 44ch; margin-block-start: var(--bm-size-8); color: var(--bm-color-text-secondary); line-height: var(--bm-line-height-compact); }
    .action { margin-block-start: var(--bm-size-20); }
    ::slotted([slot='icon']) { max-inline-size: var(--bm-icon-size-large); max-block-size: var(--bm-icon-size-large); }
  `;

  readonly #titleId = `bm-empty-state-title-${++emptyStateId}`;
  heading = '';
  description = '';
  label = '';
  headingLevel = 2;
  protected _hasIcon = false;
  protected _hasDescription = false;
  protected _hasAction = false;

  #assigned(event: Event, property: '_hasIcon' | '_hasDescription' | '_hasAction'): void {
    const slot = event.currentTarget as HTMLSlotElement;
    this[property] = slot.assignedNodes({ flatten: true }).some((node) => node.nodeType !== Node.TEXT_NODE || Boolean(node.textContent?.trim()));
  }

  override render() {
    const level = Math.min(6, Math.max(1, Math.trunc(this.headingLevel) || 2));
    const hasDescription = Boolean(this.description) || this._hasDescription;
    return html`<section class="state" part="state" role="region" aria-label=${this.label || nothing} aria-labelledby=${!this.label && this.heading ? this.#titleId : nothing}>
      <div class="icon" part="icon" ?hidden=${!this._hasIcon}><slot name="icon" @slotchange=${(event: Event) => this.#assigned(event, '_hasIcon')}></slot></div>
      <div id=${this.#titleId} class="title" part="title" role="heading" aria-level=${level}>${this.heading}</div>
      <div class="description" part="description" ?hidden=${!hasDescription}><slot name="description" @slotchange=${(event: Event) => this.#assigned(event, '_hasDescription')}>${this.description}</slot></div>
      <div class="action" part="action" ?hidden=${!this._hasAction}><slot name="action" @slotchange=${(event: Event) => this.#assigned(event, '_hasAction')}></slot></div>
    </section>`;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-empty-state': BoldMergeEmptyState; } }
