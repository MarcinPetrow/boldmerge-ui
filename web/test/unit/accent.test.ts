import { describe, expect, it } from 'vitest';
import { contrastRatio, createAccentPalette } from '../../src/foundations/accent.js';

describe('custom accent generation', () => {
  it('matches the canonical deterministic Blue scale', () => {
    expect(createAccentPalette('#3B82F6')).toEqual({
      '50': '#f0f6ff', '100': '#dbe9ff', '200': '#b7d3ff', '300': '#82b2ff',
      '400': '#5092ff', '500': '#2d74e7', '600': '#115bcc', '700': '#0045a7',
      '800': '#00317d', '900': '#00225a', '950': '#00133a',
    });
  });

  it('rejects non-opaque and malformed seeds', () => {
    expect(() => createAccentPalette('#3b82f680')).toThrow(/opaque #RRGGBB/);
    expect(() => createAccentPalette('blue')).toThrow(/opaque #RRGGBB/);
  });

  it('meets canonical contrast for the mapped roles', () => {
    const palette = createAccentPalette('#c026d3');
    expect(contrastRatio(palette['700'], '#f5f7f9')).toBeGreaterThanOrEqual(4.5);
    expect(contrastRatio(palette['300'], '#0d0f13')).toBeGreaterThanOrEqual(4.5);
  });
});
