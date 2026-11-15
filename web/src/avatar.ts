import { BoldMergeAvatar, BoldMergeAvatarGroup } from './components/avatar.js';
if (!customElements.get('bm-avatar')) customElements.define('bm-avatar', BoldMergeAvatar);
if (!customElements.get('bm-avatar-group')) customElements.define('bm-avatar-group', BoldMergeAvatarGroup);
export { BoldMergeAvatar, BoldMergeAvatarGroup };
export type { AvatarAvailability, AvatarImageErrorDetail, AvatarKind, AvatarSize } from './components/avatar.js';
