# Font and icon policy

## Typography

Inter v20 is the UI family and JetBrains Mono v24 is the data family. Product
weights are 400 and 500; the variable files retain their upstream ranges so
browser interpolation is not synthesized. Both use SIL OFL 1.1.

Web self-hosts the normal variable WOFF2 subsets through pinned Fontsource
5.3.0 packages. `@boldmerge/web/styles.css` loads them automatically;
`@boldmerge/web/fonts.css` is available when consumers compose their own
document foundations. There are no CDN requests. The canonical stacks retain
`system-ui` and `ui-monospace` fallbacks for unsupported scripts and font-load
failure. Italic remains browser-synthesized only when consumer content asks
for it; BoldMerge components do not require italic.

GTK does not register process-global application fonts from a component
library. Native applications and the locked visual environment must install
Inter v20 and JetBrains Mono v24 through their application/runtime image.
Uncontrolled hosts fall back through Pango/fontconfig. Release screenshots are
valid only when the two required families are confirmed by `fc-match`.

## Icons

Lucide 1.73.0 is the single icon source. BoldMerge exposes a reviewed subset
with identical kebab-case names on Web and Native. Web imports geometry from
the pinned ESM dependency and Native generates it at source-update time; neither
fetches icons at runtime. Stroke width is normalized
to 1.5px, with round caps and joins; supported sizes are 16, 20, and 24px.

Web provides `<bm-icon>` and `BoldMergeIconName`. Native provides
`bm_icon_new()` and `bm_icon_name_is_supported()`. Decorative icons are hidden
from accessibility APIs by default; meaning belongs on the containing control
or must be supplied explicitly on Web. Adding or replacing an icon is a public
catalog change requiring API review, license review, and regenerated Native
resources via `node scripts/generate-lucide-icons.mjs`.

The pinned versions, copyright attributions, and redistribution locations are
recorded in `THIRD_PARTY_NOTICES.md`.

