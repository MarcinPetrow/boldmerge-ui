import { LitElement, css, html, type PropertyValues } from 'lit';
import type { BoldMergeSidebar } from './sidebar.js';

export class BoldMergeAppShell extends LitElement {
  static properties = { sidebarCollapsed: { type: Boolean, attribute: 'sidebar-collapsed', reflect: true }, sidebarOpen: { type: Boolean, attribute: 'sidebar-open', reflect: true } };
  static styles = css`
    :host { display: block; min-block-size: 100vh; color: var(--bm-color-text-primary); background: var(--bm-color-canvas); font-family: var(--bm-typography-family-ui); }
    .shell { display: grid; grid-template-columns: var(--bm-sidebar-width-expanded) minmax(0, 1fr); min-block-size: 100vh; transition: grid-template-columns var(--bm-motion-duration-overlay) var(--bm-motion-easing-standard); }
    :host([sidebar-collapsed]) .shell { grid-template-columns: var(--bm-sidebar-width-collapsed) minmax(0, 1fr); }
    .sidebar { min-inline-size: 0; }
    .workspace { display: grid; grid-template-rows: auto minmax(0, 1fr); min-inline-size: 0; }
    .content { min-inline-size: 0; }
    @media (max-width: 768px) { /* mirrors app-shell.mobile.breakpoint */
      .shell, :host([sidebar-collapsed]) .shell { grid-template-columns: minmax(0, 1fr); }
      .sidebar { position: absolute; }
    }
    @media (prefers-reduced-motion: reduce) { .shell { transition: none; } }
  `;
  sidebarCollapsed = false;
  sidebarOpen = false;
  #trigger?: HTMLElement;
  constructor() { super(); this.addEventListener('bm-sidebar-dismiss', () => this.closeSidebar()); }
  override updated(changed: PropertyValues<this>): void { if (changed.has('sidebarCollapsed') || changed.has('sidebarOpen')) this.#syncSidebar(); }
  openSidebar(trigger?: HTMLElement): void { this.#trigger = trigger; this.sidebarOpen = true; void this.updateComplete.then(() => requestAnimationFrame(() => this.#sidebar()?.focus())); }
  closeSidebar(): void { const wasOpen = this.sidebarOpen; this.sidebarOpen = false; if (wasOpen) requestAnimationFrame(() => this.#trigger?.focus()); }
  toggleSidebar(trigger?: HTMLElement): void { if (this.sidebarOpen) this.closeSidebar(); else this.openSidebar(trigger); }
  #sidebar(): BoldMergeSidebar | undefined { return this.querySelector<BoldMergeSidebar>('bm-sidebar[slot="sidebar"]') ?? undefined; }
  #syncSidebar(): void { const sidebar = this.#sidebar(); if (sidebar) { sidebar.collapsed = this.sidebarCollapsed; sidebar.open = this.sidebarOpen; } }
  #slotChange(): void { this.#syncSidebar(); }
  override render() { return html`<div class="shell" part="shell"><div class="sidebar" part="sidebar"><slot name="sidebar" @slotchange=${this.#slotChange}></slot></div><div class="workspace" part="workspace"><div part="topbar"><slot name="topbar"></slot></div><div class="content" part="content"><slot></slot></div></div></div>`; }
}

declare global { interface HTMLElementTagNameMap { 'bm-app-shell': BoldMergeAppShell; } }
