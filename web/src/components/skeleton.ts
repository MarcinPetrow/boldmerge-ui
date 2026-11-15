import { LitElement, css, html, nothing } from 'lit';

export type SkeletonShape = 'text' | 'rectangle' | 'circle';

export class BoldMergeSkeleton extends LitElement {
  static properties = {
    shape: { type: String, reflect: true },
    label: { type: String },
  };

  static styles = css`
    :host {
      display: block;
      inline-size: var(--bm-skeleton-width, 100%);
      block-size: var(--bm-skeleton-height, 1em);
      color: var(--bm-color-text-primary);
      font-family: var(--bm-typography-family-ui);
    }
    :host([shape='rectangle']) { --bm-skeleton-height: var(--bm-size-64); }
    :host([shape='circle']) {
      --bm-skeleton-width: var(--bm-size-40);
      --bm-skeleton-height: var(--bm-size-40);
    }
    .bone {
      inline-size: 100%;
      block-size: 100%;
      overflow: hidden;
      border-radius: var(--bm-radius-4);
      background: linear-gradient(90deg, var(--bm-color-surface-subtle) 25%, var(--bm-color-border) 50%, var(--bm-color-surface-subtle) 75%);
      background-size: 200% 100%;
      background-position: 100% 0;
      animation: bm-skeleton-shimmer var(--bm-skeleton-duration) var(--bm-motion-easing-standard) infinite;
    }
    :host([shape='circle']) .bone { border-radius: 50%; }
    .label {
      position: absolute;
      inline-size: 1px;
      block-size: 1px;
      padding: 0;
      margin: -1px;
      overflow: hidden;
      clip: rect(0, 0, 0, 0);
      white-space: nowrap;
      border: 0;
    }
    @keyframes bm-skeleton-shimmer {
      from { background-position: 100% 0; }
      to { background-position: -100% 0; }
    }
    @media (prefers-reduced-motion: reduce) {
      .bone { animation: none; background-position: 50% 0; }
      :host-context([data-bm-motion='full']) .bone {
        animation: bm-skeleton-shimmer var(--bm-skeleton-duration) var(--bm-motion-easing-standard) infinite;
      }
    }
    @media (forced-colors: active) {
      .bone { background: Canvas; border: 1px solid GrayText; forced-color-adjust: none; }
    }
  `;

  shape: SkeletonShape = 'text';
  label = '';

  override render() {
    const shape: SkeletonShape = ['rectangle', 'circle'].includes(this.shape) ? this.shape : 'text';
    return html`<div class="bone" part="skeleton ${shape}" aria-hidden="true"></div>${this.label ? html`<span class="label" role="status" aria-label=${this.label}>${this.label}</span>` : nothing}`;
  }
}

declare global { interface HTMLElementTagNameMap { 'bm-skeleton': BoldMergeSkeleton; } }
