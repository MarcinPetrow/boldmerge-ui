import { LitElement, css, html, type PropertyValues } from 'lit';

export interface TabChangeDetail { value: string; originalEvent: Event; }

let tabSequence = 0;

export class BoldMergeTab extends LitElement {
  static properties = { value: { type: String, reflect: true }, disabled: { type: Boolean, reflect: true } };
  static styles = css`
    :host { position: relative; display: inline-flex; align-items: center; box-sizing: border-box; block-size: var(--bm-tab-height); padding-inline: var(--bm-button-padding-inline); color: var(--bm-color-text-secondary); font-family: var(--bm-typography-family-ui); font-weight: var(--bm-typography-weight-emphasis); cursor: pointer; outline: none; user-select: none; }
    :host::after { position: absolute; inset-inline: 0; inset-block-end: 0; block-size: var(--bm-tab-active-indicator-width); background: transparent; content: ''; }
    :host([aria-selected='true']) { color: var(--bm-color-accent-text); }
    :host([aria-selected='true'])::after { background: var(--bm-color-accent-solid); }
    :host(:focus-visible) { outline: var(--bm-size-2) solid var(--bm-color-focus); outline-offset: calc(-1 * var(--bm-size-2)); border-radius: var(--bm-button-radius); }
    :host([disabled]) { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    @media (pointer: coarse) { :host { min-block-size: var(--bm-interaction-target-minimum); } }
  `;
  value = '';
  disabled = false;

  constructor() {
    super();
    this.addEventListener('click', (event) => {
      if (this.disabled) { event.preventDefault(); return; }
      this.dispatchEvent(new CustomEvent('bm-tab-request', { detail: { originalEvent: event }, bubbles: true, composed: true }));
    });
  }

  override connectedCallback(): void { super.connectedCallback(); if (!this.id) this.id = `bm-tab-${++tabSequence}`; this.setAttribute('role', 'tab'); }
  setSelection(selected: boolean, tabStop: boolean, panelId?: string): void {
    this.setAttribute('aria-selected', String(selected));
    this.setAttribute('aria-disabled', String(this.disabled));
    this.tabIndex = tabStop && !this.disabled ? 0 : -1;
    if (panelId) this.setAttribute('aria-controls', panelId); else this.removeAttribute('aria-controls');
  }
  override render() { return html`<slot></slot>`; }
}

export class BoldMergeTabPanel extends LitElement {
  static properties = { value: { type: String, reflect: true } };
  static styles = css`:host { display: block; min-inline-size: 0; } :host([hidden]) { display: none !important; }`;
  value = '';
  override connectedCallback(): void { super.connectedCallback(); if (!this.id) this.id = `bm-tab-panel-${++tabSequence}`; this.setAttribute('role', 'tabpanel'); }
  setSelection(selected: boolean, tabId?: string): void {
    this.hidden = !selected;
    this.tabIndex = selected ? 0 : -1;
    if (tabId) this.setAttribute('aria-labelledby', tabId); else this.removeAttribute('aria-labelledby');
  }
  override render() { return html`<slot></slot>`; }
}

export class BoldMergeTabs extends LitElement {
  static properties = { value: { type: String, reflect: true }, label: { type: String } };
  static styles = css`
    :host { display: block; color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    .tabs { display: flex; align-items: end; overflow-x: auto; border-block-end: var(--bm-size-1) solid var(--bm-color-border); scrollbar-width: thin; }
    .panels { padding-block-start: var(--bm-section-gap); }
  `;
  readonly #observer: MutationObserver;
  value = '';
  label = '';

  constructor() {
    super();
    this.#observer = new MutationObserver(() => this.#sync());
    this.addEventListener('bm-tab-request', this.#request as EventListener);
    this.addEventListener('keydown', this.#keydown);
  }
  override connectedCallback(): void { super.connectedCallback(); this.#observer.observe(this, { attributes: true, attributeFilter: ['value', 'disabled'], childList: true, subtree: true }); }
  override disconnectedCallback(): void { this.#observer.disconnect(); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void { if (changed.has('value')) this.#sync(); }
  #tabs(): BoldMergeTab[] { return [...this.querySelectorAll<BoldMergeTab>(':scope > bm-tab[slot="tab"]')]; }
  #panels(): BoldMergeTabPanel[] { return [...this.querySelectorAll<BoldMergeTabPanel>(':scope > bm-tab-panel')]; }
  #sync(): void {
    const tabs = this.#tabs();
    const enabled = tabs.filter((tab) => !tab.disabled);
    if (!enabled.some((tab) => tab.value === this.value)) this.value = enabled[0]?.value ?? '';
    const panels = this.#panels();
    tabs.forEach((tab) => {
      const panel = panels.find((candidate) => candidate.value === tab.value);
      tab.setSelection(tab.value === this.value, tab.value === this.value || (!this.value && tab === enabled[0]), panel?.id);
    });
    panels.forEach((panel) => panel.setSelection(panel.value === this.value, tabs.find((tab) => tab.value === panel.value)?.id));
  }
  #select(tab: BoldMergeTab, originalEvent: Event): void {
    if (tab.disabled || tab.value === this.value) return;
    this.value = tab.value;
    this.#sync();
    this.dispatchEvent(new CustomEvent<TabChangeDetail>('bm-tab-change', { detail: { value: this.value, originalEvent }, bubbles: true, composed: true }));
  }
  #request = (event: CustomEvent<{ originalEvent: Event }>): void => { const tab = event.target as BoldMergeTab; if (this.#tabs().includes(tab)) this.#select(tab, event.detail.originalEvent); };
  #keydown = (event: KeyboardEvent): void => {
    const current = event.composedPath().find((node) => node instanceof BoldMergeTab) as BoldMergeTab | undefined;
    if (!current) return;
    const tabs = this.#tabs().filter((tab) => !tab.disabled);
    let index = tabs.indexOf(current);
    const rtl = getComputedStyle(this).direction === 'rtl';
    if (event.key === 'Home') index = 0;
    else if (event.key === 'End') index = tabs.length - 1;
    else if (event.key === 'ArrowRight') index = (index + (rtl ? -1 : 1) + tabs.length) % tabs.length;
    else if (event.key === 'ArrowLeft') index = (index + (rtl ? 1 : -1) + tabs.length) % tabs.length;
    else return;
    event.preventDefault();
    const next = tabs[index];
    if (next) { this.#select(next, event); next.focus(); }
  };
  override render() { return html`<div class="tabs" part="tablist" role="tablist" aria-label=${this.label}><slot name="tab" @slotchange=${this.#sync}></slot></div><div class="panels" part="panels"><slot @slotchange=${this.#sync}></slot></div>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-tab': BoldMergeTab; 'bm-tab-panel': BoldMergeTabPanel; 'bm-tabs': BoldMergeTabs; } interface HTMLElementEventMap { 'bm-tab-change': CustomEvent<TabChangeDetail>; } }
