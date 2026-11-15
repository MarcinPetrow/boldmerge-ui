import { LitElement, css, html, type PropertyValues } from 'lit';
import { ACCENT_TONES, createAccentPalette, isAccentPreset, type AccentPalette } from './accent.js';

export type ThemePreference = 'light' | 'dark' | 'system' | 'inherit';
export type DensityPreference = 'balanced' | 'compact' | 'inherit';
export type MotionPreference = 'full' | 'reduced' | 'inherit';

export class BoldMergeTheme extends LitElement {
  static properties = {
    theme: { type: String },
    density: { type: String },
    accent: { type: String },
    motion: { type: String },
  };

  static styles = css`
    :host {
      display: block;
      color: var(--bm-color-text-primary);
      background: var(--bm-color-canvas);
      font-family: var(--bm-typography-family-ui);
      font-weight: var(--bm-typography-weight-regular);
      color-scheme: light dark;
    }

    .surface {
      box-sizing: border-box;
      min-inline-size: 0;
      padding: var(--bm-layout-gap);
      color: var(--bm-color-text-primary);
      background: var(--bm-color-canvas);
      transition:
        color var(--bm-motion-duration-interaction) var(--bm-motion-easing-standard),
        background-color var(--bm-motion-duration-interaction) var(--bm-motion-easing-standard);
    }

    ::slotted(*) {
      box-sizing: border-box;
    }
  `;

  theme: ThemePreference = 'inherit';
  density: DensityPreference = 'inherit';
  accent = 'inherit';
  motion: MotionPreference = 'inherit';

  readonly #systemTheme = globalThis.matchMedia?.('(prefers-color-scheme: dark)');
  readonly #systemThemeChanged = (): void => this.#applyTheme();

  override connectedCallback(): void {
    super.connectedCallback();
    this.#systemTheme?.addEventListener('change', this.#systemThemeChanged);
  }

  override disconnectedCallback(): void {
    this.#systemTheme?.removeEventListener('change', this.#systemThemeChanged);
    super.disconnectedCallback();
  }

  override willUpdate(changed: PropertyValues<this>): void {
    if (changed.has('theme')) this.#applyTheme();
    if (changed.has('density')) this.#applyEnumerated('data-bm-density', this.density, ['balanced', 'compact']);
    if (changed.has('motion')) this.#applyEnumerated('data-bm-motion', this.motion, ['full', 'reduced']);
    if (changed.has('accent')) this.#applyAccent();
  }

  setCustomAccent(seed: string): AccentPalette {
    const palette = createAccentPalette(seed);
    this.accent = seed.toLowerCase();
    return palette;
  }

  #applyTheme(): void {
    if (this.theme === 'system') {
      const resolved = this.#systemTheme?.matches ? 'dark' : 'light';
      this.dataset.bmTheme = resolved;
      this.style.colorScheme = resolved;
    } else {
      this.#applyEnumerated('data-bm-theme', this.theme, ['light', 'dark']);
      if (this.theme === 'light' || this.theme === 'dark') this.style.colorScheme = this.theme;
      else this.style.removeProperty('color-scheme');
    }
  }

  #applyEnumerated(attribute: string, value: string, supported: readonly string[]): void {
    if (supported.includes(value)) this.setAttribute(attribute, value);
    else this.removeAttribute(attribute);
  }

  #clearCustomAccent(): void {
    this.style.removeProperty('--bm-accent-seed');
    for (const tone of ACCENT_TONES) this.style.removeProperty(`--bm-accent-tone-${tone}`);
  }

  #applyAccent(): void {
    this.#clearCustomAccent();
    if (isAccentPreset(this.accent)) {
      this.dataset.bmAccent = this.accent;
      return;
    }
    if (this.accent === 'inherit') {
      delete this.dataset.bmAccent;
      return;
    }
    try {
      const palette = createAccentPalette(this.accent);
      this.dataset.bmAccent = 'custom';
      this.style.setProperty('--bm-accent-seed', this.accent.toLowerCase());
      for (const tone of ACCENT_TONES) this.style.setProperty(`--bm-accent-tone-${tone}`, palette[tone]);
    } catch (error) {
      delete this.dataset.bmAccent;
      this.dispatchEvent(new CustomEvent('bm-theme-error', { detail: error, bubbles: true, composed: true }));
    }
  }

  override render() {
    return html`<div class="surface" part="surface"><slot></slot></div>`;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'bm-theme': BoldMergeTheme;
  }
}
