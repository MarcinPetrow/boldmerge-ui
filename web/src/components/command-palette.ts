import { LitElement, css, html, nothing, type PropertyValues } from 'lit';
import { lockDocumentScroll, unlockDocumentScroll } from '../utilities/modal-scroll-lock.js';

export type CommandPaletteCloseReason = 'api' | 'escape' | 'backdrop' | 'select';
export interface CommandPaletteShowOptions { trigger?: HTMLElement; originalEvent?: Event; }
export interface CommandPaletteSelectDetail { value: string; label: string; originalEvent: Event; }
export interface CommandPaletteCloseDetail { reason: CommandPaletteCloseReason; originalEvent?: Event; }
export interface CommandPaletteOpenChangeDetail { open: boolean; originalEvent?: Event; }

export class BoldMergeCommand extends LitElement {
  static properties = { value: { type: String, reflect: true }, keywords: { type: String }, disabled: { type: Boolean, reflect: true } };
  static styles = css`:host { display: none !important; }`;
  value = ''; keywords = ''; disabled = false;
  get label(): string { return this.textContent?.replace(/\s+/g, ' ').trim() ?? ''; }
  override render() { return html`<slot></slot>`; }
}

export class BoldMergeCommandGroup extends LitElement {
  static properties = { label: { type: String, reflect: true } };
  static styles = css`:host { display: none !important; }`;
  label = '';
  override render() { return html`<slot></slot>`; }
}

let paletteSequence = 0;

export class BoldMergeCommandPalette extends LitElement {
  static properties = {
    open: { type: Boolean, reflect: true }, label: { type: String }, placeholder: { type: String },
    emptyLabel: { type: String, attribute: 'empty-label' }, shortcut: { type: Boolean, reflect: true },
    _query: { state: true }, _activeIndex: { state: true }, _version: { state: true },
  };
  static styles = css`
    :host { color: var(--bm-color-text-primary); font-family: var(--bm-typography-family-ui); }
    dialog { box-sizing: border-box; inline-size: min(var(--bm-dialog-width-max), calc(100vw - var(--bm-size-32))); max-block-size: min(640px, calc(100dvh - var(--bm-size-32))); padding: 0; color: inherit; background: var(--bm-color-surface); border: 0; border-radius: var(--bm-dialog-radius); box-shadow: var(--bm-dialog-shadow); overflow: hidden; }
    dialog::backdrop { background: var(--bm-color-overlay); }
    dialog[open] { animation: reveal var(--bm-dialog-transition-duration) var(--bm-motion-easing-standard); }
    .search { display: grid; grid-template-columns: auto minmax(0, 1fr) auto; align-items: center; gap: var(--bm-size-8); padding: var(--bm-size-12) var(--bm-dialog-padding); border-block-end: var(--bm-size-1) solid var(--bm-color-border); }
    .icon { color: var(--bm-color-text-secondary); }
    input { min-inline-size: 0; padding: var(--bm-size-8) 0; color: inherit; background: transparent; border: 0; outline: 0; font: inherit; font-size: var(--bm-font-size-16); }
    kbd { padding: var(--bm-size-3) var(--bm-size-6); color: var(--bm-color-text-secondary); background: var(--bm-color-surface-subtle); border: var(--bm-size-1) solid var(--bm-color-border); border-radius: var(--bm-radius-4); font: inherit; font-size: var(--bm-font-size-12); }
    .results { max-block-size: 420px; overflow: auto; padding-block: var(--bm-dropdown-padding-block); }
    .group-label { padding: var(--bm-size-8) var(--bm-dialog-padding) var(--bm-size-4); color: var(--bm-color-text-secondary); font-size: var(--bm-font-size-12); font-weight: var(--bm-typography-weight-emphasis); text-transform: uppercase; letter-spacing: .05em; }
    .command { display: flex; align-items: center; min-block-size: var(--bm-menu-item-height); padding-inline: var(--bm-dialog-padding); cursor: pointer; }
    .command[data-active] { color: var(--bm-color-accent-on-subtle); background: var(--bm-color-accent-subtle); }
    .command[aria-disabled='true'] { opacity: var(--bm-content-opacity-disabled); cursor: not-allowed; }
    .empty { padding: var(--bm-dialog-padding); color: var(--bm-color-text-secondary); text-align: center; }
    @keyframes reveal { from { opacity: 0; transform: translateY(var(--bm-size-8)); } }
    @media (pointer: coarse) { .command { min-block-size: var(--bm-interaction-target-minimum); } }
    @media (prefers-reduced-motion: reduce) { dialog[open] { animation: none; } }
  `;

  open = false; label = 'Command palette'; placeholder = 'Search commands'; emptyLabel = 'No matching commands'; shortcut = false;
  protected _query = ''; protected _activeIndex = -1; protected _version = 0;
  readonly #listboxId = `bm-command-palette-${++paletteSequence}-listbox`;
  readonly #observer = new MutationObserver(() => { this._version += 1; this.#activateFirst(); });
  #restoreTarget?: HTMLElement; #locked = false; #composing = false;

  override connectedCallback(): void { super.connectedCallback(); this.#observer.observe(this, { subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: ['value', 'keywords', 'disabled', 'label'] }); document.addEventListener('keydown', this.#documentKeyDown); }
  override disconnectedCallback(): void { this.#observer.disconnect(); document.removeEventListener('keydown', this.#documentKeyDown); if (this.#locked) unlockDocumentScroll(); super.disconnectedCallback(); }
  override updated(changed: PropertyValues<this>): void { if (changed.has('open')) this.#syncOpen(); }

  show(options: CommandPaletteShowOptions = {}): void { if (this.open) return; this.#restoreTarget = options.trigger ?? (document.activeElement instanceof HTMLElement ? document.activeElement : undefined); this._query = ''; this.open = true; this.#activateFirst(); this.#emitOpen(options.originalEvent); }
  close(reason: CommandPaletteCloseReason = 'api', originalEvent?: Event): void { if (!this.open) return; this.open = false; this.dispatchEvent(new CustomEvent<CommandPaletteCloseDetail>('bm-command-palette-close', { detail: { reason, originalEvent }, bubbles: true, composed: true })); this.#emitOpen(originalEvent); }
  focus(options?: FocusOptions): void { this.renderRoot.querySelector('input')?.focus(options); }

  #dialog(): HTMLDialogElement | null { return this.renderRoot.querySelector('dialog'); }
  #syncOpen(): void { const dialog = this.#dialog(); if (!dialog) return; if (this.open) { if (!dialog.open) dialog.showModal(); if (!this.#locked) { lockDocumentScroll(); this.#locked = true; } requestAnimationFrame(() => this.focus()); } else { if (dialog.open) dialog.close(); if (this.#locked) { unlockDocumentScroll(); this.#locked = false; } const restore = this.#restoreTarget; this.#restoreTarget = undefined; if (restore?.isConnected) requestAnimationFrame(() => restore.focus()); } }
  #commands(): BoldMergeCommand[] { return [...this.querySelectorAll<BoldMergeCommand>('bm-command')]; }
  #visible(): BoldMergeCommand[] { const query = this._query.trim().toLocaleLowerCase(); return this.#commands().filter((command) => !query || `${command.label} ${command.keywords}`.toLocaleLowerCase().includes(query)); }
  #groups(commands: BoldMergeCommand[]): Array<{ label: string; commands: BoldMergeCommand[] }> { const groups = new Map<string, BoldMergeCommand[]>(); for (const command of commands) { const label = command.closest<BoldMergeCommandGroup>('bm-command-group')?.label ?? ''; const items = groups.get(label) ?? []; items.push(command); groups.set(label, items); } return [...groups].map(([label, items]) => ({ label, commands: items })); }
  #activateFirst(): void { this._activeIndex = this.#visible().findIndex((command) => !command.disabled); }
  #move(step: number): void { const commands = this.#visible(); if (!commands.length) return; let index = this._activeIndex; for (let count = 0; count < commands.length; count++) { index = (index + step + commands.length) % commands.length; if (!commands[index]?.disabled) { this._activeIndex = index; return; } } }
  #boundary(end: boolean): void { const commands = this.#visible(); if (!end) { this._activeIndex = commands.findIndex((command) => !command.disabled); return; } for (let index = commands.length - 1; index >= 0; index--) if (!commands[index]?.disabled) { this._activeIndex = index; return; } }
  #select(command: BoldMergeCommand, event: Event): void { if (command.disabled) return; this.dispatchEvent(new CustomEvent<CommandPaletteSelectDetail>('bm-command-palette-select', { detail: { value: command.value, label: command.label, originalEvent: event }, bubbles: true, composed: true })); this.close('select', event); }
  #input(event: InputEvent): void { this._query = (event.currentTarget as HTMLInputElement).value; this.#activateFirst(); }
  #keyDown(event: KeyboardEvent): void { if (this.#composing || event.isComposing) return; if (event.key === 'ArrowDown' || event.key === 'ArrowUp') { event.preventDefault(); this.#move(event.key === 'ArrowDown' ? 1 : -1); } else if (event.key === 'Home') { event.preventDefault(); this.#boundary(false); } else if (event.key === 'End') { event.preventDefault(); this.#boundary(true); } else if (event.key === 'Enter') { const command = this.#visible()[this._activeIndex]; if (command) { event.preventDefault(); this.#select(command, event); } } }
  #cancel(event: Event): void { event.preventDefault(); this.close('escape', event); }
  #backdrop(event: MouseEvent): void { if (event.target === event.currentTarget) this.close('backdrop', event); }
  #emitOpen(originalEvent?: Event): void { this.dispatchEvent(new CustomEvent<CommandPaletteOpenChangeDetail>('bm-command-palette-open-change', { detail: { open: this.open, originalEvent }, bubbles: true, composed: true })); }
  #documentKeyDown = (event: KeyboardEvent): void => { if (!this.shortcut || event.defaultPrevented || event.key.toLocaleLowerCase() !== 'k' || !(event.ctrlKey || event.metaKey)) return; const target = event.composedPath()[0]; if (target instanceof HTMLElement && (target.isContentEditable || /^(INPUT|TEXTAREA|SELECT)$/.test(target.tagName))) return; event.preventDefault(); this.open ? this.close('api', event) : this.show({ originalEvent: event }); };

  override render() {
    const commands = this.#visible(); const groups = this.#groups(commands); const active = commands[this._activeIndex]; const activeId = active ? `${this.#listboxId}-option-${this.#commands().indexOf(active)}` : undefined;
    return html`<dialog part="dialog" aria-label=${this.label || nothing} @cancel=${this.#cancel} @click=${this.#backdrop}><div class="search" part="search"><span class="icon" aria-hidden="true">⌕</span><input part="input" type="search" .value=${this._query} placeholder=${this.placeholder} role="combobox" aria-expanded="true" aria-controls=${this.#listboxId} aria-autocomplete="list" aria-activedescendant=${activeId || nothing} @input=${this.#input} @keydown=${this.#keyDown} @compositionstart=${() => { this.#composing = true; }} @compositionend=${(event: CompositionEvent) => { this.#composing = false; this._query = (event.currentTarget as HTMLInputElement).value; this.#activateFirst(); }}><kbd part="shortcut" aria-hidden="true">Ctrl/⌘ K</kbd></div><div id=${this.#listboxId} class="results" part="results" role="listbox" aria-label=${this.label}>${commands.length ? groups.map((group) => html`<div class="group" part="group" role="group" aria-label=${group.label || nothing}>${group.label ? html`<div class="group-label" part="group-label">${group.label}</div>` : nothing}${group.commands.map((command) => { const index = commands.indexOf(command); return html`<div id=${`${this.#listboxId}-option-${this.#commands().indexOf(command)}`} class="command" part="command" role="option" aria-selected=${index === this._activeIndex ? 'true' : 'false'} aria-disabled=${command.disabled ? 'true' : 'false'} ?data-active=${index === this._activeIndex} @pointerdown=${(event: PointerEvent) => event.preventDefault()} @click=${(event: MouseEvent) => this.#select(command, event)}>${command.label}</div>`; })}</div>`) : html`<div class="empty" part="empty"><slot name="empty">${this.emptyLabel}</slot></div>`}</div><div hidden aria-hidden="true"><slot></slot></div></dialog>`;
  }
}

declare global {
  interface HTMLElementTagNameMap { 'bm-command-palette': BoldMergeCommandPalette; 'bm-command-group': BoldMergeCommandGroup; 'bm-command': BoldMergeCommand; }
  interface HTMLElementEventMap { 'bm-command-palette-select': CustomEvent<CommandPaletteSelectDetail>; 'bm-command-palette-close': CustomEvent<CommandPaletteCloseDetail>; 'bm-command-palette-open-change': CustomEvent<CommandPaletteOpenChangeDetail>; }
}
