import { LitElement, css, html, nothing, type PropertyValues } from 'lit';

export type TooltipPlacement = 'auto' | 'top' | 'bottom' | 'left' | 'right';
export interface TooltipOpenChangeDetail { open: boolean; originalEvent?: Event; }

let tooltipSequence = 0;

export class BoldMergeTooltip extends LitElement {
  static properties = {
    content: { type: String },
    open: { type: Boolean, reflect: true },
    placement: { type: String, reflect: true },
  };

  static styles = css`
    :host { display: inline-block; font-family: var(--bm-typography-family-ui); }
    .tooltip { position: fixed; z-index: var(--bm-overlay-stack-level); box-sizing: border-box; max-inline-size: calc(100vw - var(--bm-size-16)); padding: var(--bm-tooltip-padding-block) var(--bm-tooltip-padding-inline); color: var(--bm-color-text-primary); background: var(--bm-color-surface); border: var(--bm-size-1) solid var(--bm-color-border-strong); border-radius: var(--bm-tooltip-radius); box-shadow: var(--bm-dropdown-shadow); font-size: var(--bm-font-size-12); line-height: var(--bm-line-height-compact); pointer-events: none; opacity: 0; visibility: hidden; transition: opacity var(--bm-tooltip-transition-duration) var(--bm-motion-easing-standard); }
    :host([open]) .tooltip { opacity: 1; visibility: visible; }
    .pointer { position: absolute; inline-size: 0; block-size: 0; border: var(--bm-tooltip-pointer-size) solid transparent; }
    .tooltip[data-placement='top'] .pointer { inset-block-start: 100%; inset-inline-start: 50%; border-block-start-color: var(--bm-color-border-strong); transform: translateX(-50%); }
    .tooltip[data-placement='bottom'] .pointer { inset-block-end: 100%; inset-inline-start: 50%; border-block-end-color: var(--bm-color-border-strong); transform: translateX(-50%); }
    .tooltip[data-placement='left'] .pointer { inset-block-start: 50%; inset-inline-start: 100%; border-inline-start-color: var(--bm-color-border-strong); transform: translateY(-50%); }
    .tooltip[data-placement='right'] .pointer { inset-block-start: 50%; inset-inline-end: 100%; border-inline-end-color: var(--bm-color-border-strong); transform: translateY(-50%); }
    @media (prefers-reduced-motion: reduce) { .tooltip { transition: none; } }
  `;

  content = '';
  open = false;
  placement: TooltipPlacement = 'auto';
  readonly #tooltipId = `bm-tooltip-${++tooltipSequence}`;
  #timer?: number;
  #describedTrigger?: HTMLElement;
  #managedDescription = false;
  #resizeObserver = new ResizeObserver(() => this.#position());

  constructor() {
    super();
    this.addEventListener('pointerenter', this.#pointerEnter);
    this.addEventListener('pointerleave', this.#pointerLeave);
    this.addEventListener('focusin', this.#focusIn);
    this.addEventListener('focusout', this.#focusOut);
    this.addEventListener('keydown', this.#keyDown);
  }

  override disconnectedCallback(): void {
    this.#clearTimer();
    this.#stopPositioning();
    this.#restoreDescription();
    super.disconnectedCallback();
  }

  override updated(changed: PropertyValues<this>): void {
    if (changed.has('content')) this.#syncDescription();
    if (changed.has('open') || changed.has('placement')) {
      if (this.open) { this.#startPositioning(); this.#position(); }
      else this.#stopPositioning();
    }
  }

  show(originalEvent?: Event): void { this.#setOpen(true, originalEvent); }
  hide(originalEvent?: Event): void { this.#setOpen(false, originalEvent); }

  #trigger(): HTMLElement | undefined {
    return (this.renderRoot.querySelector<HTMLSlotElement>('slot:not([name])')?.assignedElements({ flatten: true })[0] as HTMLElement | undefined);
  }

  #descriptionTarget(trigger: HTMLElement): HTMLElement {
    if (trigger.matches('button, a[href], input, select, textarea, [tabindex]:not([tabindex="-1"])')) return trigger;
    return trigger.shadowRoot?.querySelector<HTMLElement>('button, a[href], input, select, textarea, [tabindex]:not([tabindex="-1"])') ?? trigger;
  }

  #tooltip(): HTMLElement | null { return this.renderRoot.querySelector('.tooltip'); }

  #setOpen(open: boolean, originalEvent?: Event): void {
    this.#clearTimer();
    if (this.open === open || (open && !this.#trigger())) return;
    this.open = open;
    this.dispatchEvent(new CustomEvent<TooltipOpenChangeDetail>('bm-tooltip-open-change', { detail: { open, originalEvent }, bubbles: true, composed: true }));
  }

  #pointerEnter = (event: PointerEvent): void => {
    if (event.pointerType === 'touch' || this.open) return;
    this.#clearTimer();
    const delay = Number.parseFloat(getComputedStyle(this).getPropertyValue('--bm-tooltip-delay')) || 400;
    this.#timer = window.setTimeout(() => this.show(event), delay);
  };

  #pointerLeave = (event: PointerEvent): void => { this.#clearTimer(); this.hide(event); };
  #focusIn = (event: FocusEvent): void => { if (event.composedPath().includes(this.#trigger() as EventTarget)) this.show(event); };
  #focusOut = (event: FocusEvent): void => { if (!event.relatedTarget || !this.contains(event.relatedTarget as Node)) this.hide(event); };
  #keyDown = (event: KeyboardEvent): void => { if (event.key === 'Escape' && this.open) { event.preventDefault(); this.hide(event); } };
  #clearTimer(): void { if (this.#timer !== undefined) window.clearTimeout(this.#timer); this.#timer = undefined; }

  #slotChange(): void {
    const trigger = this.#trigger();
    const target = trigger ? this.#descriptionTarget(trigger) : undefined;
    if (target !== this.#describedTrigger) { this.#restoreDescription(); this.#describedTrigger = target; }
    this.#syncDescription();
    if (this.open) this.#position();
  }

  #descriptionText(): string {
    if (this.content.trim()) return this.content.trim();
    const slot = this.renderRoot.querySelector<HTMLSlotElement>('slot[name="content"]');
    return slot?.assignedNodes({ flatten: true }).map((node) => node.textContent ?? '').join(' ').replace(/\s+/g, ' ').trim() ?? '';
  }

  #syncDescription(): void {
    const trigger = this.#trigger();
    if (!trigger) return;
    const target = this.#descriptionTarget(trigger);
    if (target !== this.#describedTrigger) { this.#restoreDescription(); this.#describedTrigger = target; }
    const description = this.#descriptionText();
    if (!target.hasAttribute('aria-description') && description) {
      target.setAttribute('aria-description', description);
      this.#managedDescription = true;
    } else if (this.#managedDescription) {
      if (description) target.setAttribute('aria-description', description);
      else target.removeAttribute('aria-description');
    }
  }

  #restoreDescription(): void {
    if (this.#managedDescription) this.#describedTrigger?.removeAttribute('aria-description');
    this.#managedDescription = false;
    this.#describedTrigger = undefined;
  }

  #startPositioning(): void {
    window.addEventListener('resize', this.#position);
    window.addEventListener('scroll', this.#position, true);
    const trigger = this.#trigger();
    const tooltip = this.#tooltip();
    if (trigger) this.#resizeObserver.observe(trigger);
    if (tooltip) this.#resizeObserver.observe(tooltip);
    void this.updateComplete.then(() => this.#position());
  }

  #stopPositioning(): void {
    window.removeEventListener('resize', this.#position);
    window.removeEventListener('scroll', this.#position, true);
    this.#resizeObserver.disconnect();
  }

  #position = (): void => {
    if (!this.open) return;
    const trigger = this.#trigger();
    const tooltip = this.#tooltip();
    if (!trigger || !tooltip) return;
    const anchor = trigger.getBoundingClientRect();
    const popup = tooltip.getBoundingClientRect();
    const pointer = Number.parseFloat(getComputedStyle(this).getPropertyValue('--bm-tooltip-pointer-size')) || 6;
    const gap = pointer * 2;
    const edge = 8;
    const available = { top: anchor.top, bottom: innerHeight - anchor.bottom, left: anchor.left, right: innerWidth - anchor.right };
    let resolved: Exclude<TooltipPlacement, 'auto'> = this.placement === 'auto' ? 'top' : this.placement;
    if (this.placement === 'auto') {
      const required = { top: popup.height + gap, bottom: popup.height + gap, left: popup.width + gap, right: popup.width + gap };
      resolved = (['top', 'bottom', 'right', 'left'] as const).find((side) => available[side] >= required[side])
        ?? (Object.entries(available).sort((a, b) => b[1] - a[1])[0]?.[0] as Exclude<TooltipPlacement, 'auto'> ?? 'top');
    }
    let left = anchor.left + (anchor.width - popup.width) / 2;
    let top = anchor.top - popup.height - gap;
    if (resolved === 'bottom') top = anchor.bottom + gap;
    if (resolved === 'left') { left = anchor.left - popup.width - gap; top = anchor.top + (anchor.height - popup.height) / 2; }
    if (resolved === 'right') { left = anchor.right + gap; top = anchor.top + (anchor.height - popup.height) / 2; }
    left = Math.min(Math.max(edge, left), Math.max(edge, innerWidth - popup.width - edge));
    top = Math.min(Math.max(edge, top), Math.max(edge, innerHeight - popup.height - edge));
    tooltip.style.left = `${left}px`;
    tooltip.style.top = `${top}px`;
    tooltip.dataset.placement = resolved;
  };

  override render() {
    return html`<slot @slotchange=${this.#slotChange}></slot><div id=${this.#tooltipId} class="tooltip" part="tooltip" role="tooltip" aria-hidden=${this.open ? 'false' : 'true'} data-placement="top"><span class="pointer" part="pointer" aria-hidden="true"></span>${this.content ? this.content : html`<slot name="content" @slotchange=${this.#slotChange}></slot>`}</div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-tooltip': BoldMergeTooltip; }
  interface HTMLElementEventMap { 'bm-tooltip-open-change': CustomEvent<TooltipOpenChangeDetail>; }
}
