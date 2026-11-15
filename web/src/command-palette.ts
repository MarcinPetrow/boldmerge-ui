import { BoldMergeCommand, BoldMergeCommandGroup, BoldMergeCommandPalette } from './components/command-palette.js';
if (!customElements.get('bm-command')) customElements.define('bm-command', BoldMergeCommand);
if (!customElements.get('bm-command-group')) customElements.define('bm-command-group', BoldMergeCommandGroup);
if (!customElements.get('bm-command-palette')) customElements.define('bm-command-palette', BoldMergeCommandPalette);
export { BoldMergeCommand, BoldMergeCommandGroup, BoldMergeCommandPalette };
export type { CommandPaletteCloseDetail, CommandPaletteCloseReason, CommandPaletteOpenChangeDetail, CommandPaletteSelectDetail, CommandPaletteShowOptions } from './components/command-palette.js';
