import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export type AvatarKind = 'person' | 'organization';
export type AvatarSize = 'small' | 'medium' | 'large' | 'xlarge';
export type AvatarAvailability = 'none' | 'online' | 'away' | 'busy' | 'offline';
export interface AvatarImageErrorDetail { src: string; originalEvent: Event; }

function initials(name: string): string {
  const parts = name.trim().split(/\s+/u).filter(Boolean);
  if (parts.length === 0) return '';
  const first = Array.from(parts[0] ?? '')[0] ?? '';
  const last = parts.length > 1 ? Array.from(parts.at(-1) ?? '')[0] ?? '' : '';
  return `${first}${last}`.toUpperCase();
}

export class BoldMergeAvatar extends LitElement {
  static properties = {
    name: { type: String }, src: { type: String }, label: { type: String },
    kind: { type: String, reflect: true }, size: { type: String, reflect: true },
    availability: { type: String, reflect: true }, availabilityLabel: { type: String, attribute: 'availability-label' },
    _imageLoaded: { state: true }, _imageFailed: { state: true },
  };
  static styles = css`
    :host { --_avatar-size: var(--bm-avatar-group-size, var(--bm-avatar-size-medium)); display: inline-block; flex: none; font-family: var(--bm-typography-family-ui); vertical-align: middle; }
    :host([size='small']) { --_avatar-size: var(--bm-avatar-size-small); }
    :host([size='medium']) { --_avatar-size: var(--bm-avatar-size-medium); }
    :host([size='large']) { --_avatar-size: var(--bm-avatar-size-large); }
    :host([size='xlarge']) { --_avatar-size: var(--bm-avatar-size-xlarge); }
    .avatar { position: relative; display: grid; inline-size: var(--_avatar-size); block-size: var(--_avatar-size); place-items: center; overflow: visible; color: var(--bm-color-accent-on-subtle); background: var(--bm-color-accent-subtle); border-radius: 50%; font-size: calc(var(--_avatar-size) * .36); font-weight: var(--bm-typography-weight-emphasis); line-height: 1; user-select: none; }
    :host([kind='organization']) .avatar { border-radius: var(--bm-avatar-organization-radius); }
    img, .fallback { inline-size: 100%; block-size: 100%; border-radius: inherit; }
    img { display: block; object-fit: cover; }
    img[hidden], .fallback[hidden] { display: none; }
    .fallback { display: grid; place-items: center; overflow: hidden; }
    .availability { position: absolute; inset-inline-end: -1px; inset-block-end: -1px; inline-size: max(var(--bm-status-dot-size), calc(var(--_avatar-size) * .24)); block-size: max(var(--bm-status-dot-size), calc(var(--_avatar-size) * .24)); background: var(--bm-color-text-muted); border: var(--bm-size-2) solid var(--bm-color-surface); border-radius: 50%; }
    :host([availability='online']) .availability { background: var(--bm-color-success); }
    :host([availability='away']) .availability { background: var(--bm-color-warning); }
    :host([availability='busy']) .availability { background: var(--bm-color-error); }
  `;

  name = '';
  src = '';
  label = '';
  kind: AvatarKind = 'person';
  size: AvatarSize = 'medium';
  availability: AvatarAvailability = 'none';
  availabilityLabel = '';
  protected _imageLoaded = false;
  protected _imageFailed = false;

  get fallback(): string { return initials(this.name); }

  protected override willUpdate(changed: PropertyValues<this>): void {
    if (changed.has('src')) { this._imageLoaded = false; this._imageFailed = false; }
  }

  protected override updated(): void {
    const base = this.label || this.name;
    const accessible = [base, this.availability !== 'none' ? this.availabilityLabel : ''].filter(Boolean).join(', ');
    if (accessible) { this.setAttribute('role', 'img'); this.setAttribute('aria-label', accessible); this.removeAttribute('aria-hidden'); }
    else { this.removeAttribute('role'); this.removeAttribute('aria-label'); this.setAttribute('aria-hidden', 'true'); }
  }

  #load(): void { this._imageLoaded = true; this._imageFailed = false; }
  #error(event: Event): void {
    this._imageLoaded = false; this._imageFailed = true;
    this.dispatchEvent(new CustomEvent<AvatarImageErrorDetail>('bm-avatar-image-error', { detail: { src: this.src, originalEvent: event }, bubbles: true, composed: true }));
  }

  override render() {
    const showImage = Boolean(this.src) && this._imageLoaded && !this._imageFailed;
    return html`<span class="avatar" part="avatar">
      ${this.src ? html`<img part="image" src=${this.src} alt="" ?hidden=${!showImage} @load=${this.#load} @error=${this.#error} />` : nothing}
      <span class="fallback" part="fallback" ?hidden=${showImage}><slot name="fallback">${this.fallback}</slot></span>
      ${this.availability !== 'none' ? html`<i class="availability" part="availability" aria-hidden="true"></i>` : nothing}
    </span>`;
  }
}

export class BoldMergeAvatarGroup extends LitElement {
  static properties = { label: { type: String }, size: { type: String, reflect: true }, overflowCount: { type: Number, attribute: 'overflow-count', reflect: true }, overflowLabel: { type: String, attribute: 'overflow-label' } };
  static styles = css`
    :host { --_group-size: var(--bm-avatar-size-medium); display: inline-block; font-family: var(--bm-typography-family-ui); }
    :host([size='small']) { --_group-size: var(--bm-avatar-size-small); }
    :host([size='large']) { --_group-size: var(--bm-avatar-size-large); }
    :host([size='xlarge']) { --_group-size: var(--bm-avatar-size-xlarge); }
    .group { display: inline-flex; align-items: center; padding-inline-start: var(--bm-size-8); }
    ::slotted(bm-avatar) { --bm-avatar-group-size: var(--_group-size); margin-inline-start: calc(-1 * var(--bm-size-8)); border-radius: 50%; outline: var(--bm-size-2) solid var(--bm-color-surface); }
    .overflow { display: grid; inline-size: var(--_group-size); block-size: var(--_group-size); place-items: center; margin-inline-start: calc(-1 * var(--bm-size-8)); color: var(--bm-color-text-secondary); background: var(--bm-color-surface-subtle); border: var(--bm-size-2) solid var(--bm-color-surface); border-radius: 50%; font-size: var(--bm-font-size-12); font-weight: var(--bm-typography-weight-emphasis); }
  `;
  label = 'Avatar group';
  size: AvatarSize = 'medium';
  overflowCount = 0;
  overflowLabel = '';
  override render() { const count = Math.max(0, Math.trunc(this.overflowCount) || 0); return html`<div class="group" part="group" role="group" aria-label=${this.label}><slot></slot>${count > 0 ? html`<span class="overflow" part="overflow" aria-label=${this.overflowLabel || `${count} more`}>+${count}</span>` : nothing}</div>`; }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-avatar': BoldMergeAvatar; 'bm-avatar-group': BoldMergeAvatarGroup; }
  interface HTMLElementEventMap { 'bm-avatar-image-error': CustomEvent<AvatarImageErrorDetail>; }
}
