import { css, LitElement } from 'lit';
import {
  Bell, Check, CircleAlert, CircleCheck, CircleX, Diamond, Ellipsis, Folder,
  FolderPlus, Grid2X2, History, House, Minus, Pencil, Plus, RotateCw, Save,
  Search, Settings, Star, Trash2, TriangleAlert, X, createElement,
  type IconNode,
} from 'lucide';

export const BOLDMERGE_ICON_NAMES = [
  'bell', 'check', 'circle-alert', 'circle-check', 'circle-x', 'diamond',
  'ellipsis', 'folder', 'folder-plus', 'grid-2x2', 'history', 'house', 'minus',
  'pencil', 'plus', 'rotate-cw', 'save', 'search', 'settings', 'star',
  'trash-2', 'triangle-alert', 'x',
] as const;

export type BoldMergeIconName = typeof BOLDMERGE_ICON_NAMES[number];
export type BoldMergeIconSize = 'small' | 'default' | 'large';

const ICONS: Record<BoldMergeIconName, IconNode> = {
  bell: Bell, check: Check, 'circle-alert': CircleAlert, 'circle-check': CircleCheck,
  'circle-x': CircleX, diamond: Diamond, ellipsis: Ellipsis, folder: Folder,
  'folder-plus': FolderPlus, 'grid-2x2': Grid2X2, history: History, house: House,
  minus: Minus, pencil: Pencil, plus: Plus, 'rotate-cw': RotateCw, save: Save,
  search: Search, settings: Settings, star: Star, 'trash-2': Trash2,
  'triangle-alert': TriangleAlert, x: X,
};

export class BoldMergeIcon extends LitElement {
  static properties = {
    name: { type: String, reflect: true },
    size: { type: String, reflect: true },
    label: { type: String },
  };

  static styles = css`
    :host { display: inline-flex; flex: none; inline-size: var(--bm-icon-size-default); block-size: var(--bm-icon-size-default); color: inherit; vertical-align: middle; }
    :host([size='small']) { inline-size: var(--bm-icon-size-small); block-size: var(--bm-icon-size-small); }
    :host([size='large']) { inline-size: var(--bm-icon-size-large); block-size: var(--bm-icon-size-large); }
    svg { display: block; inline-size: 100%; block-size: 100%; }
  `;

  name: BoldMergeIconName = 'check';
  size: BoldMergeIconSize = 'default';
  label = '';

  render() {
    const icon = ICONS[this.name];
    this.toggleAttribute('data-invalid', !icon);
    if (!icon) return null;
    return createElement(icon, {
      width: '100%', height: '100%', 'stroke-width': '1.5',
      ...(this.label
        ? { 'aria-label': this.label, role: 'img' }
        : { 'aria-hidden': 'true' }),
      focusable: 'false',
    });
  }
}

