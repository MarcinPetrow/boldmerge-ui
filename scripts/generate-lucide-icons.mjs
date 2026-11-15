import { mkdir, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const lucide = await import(pathToFileURL(path.join(root, 'web/node_modules/lucide/dist/esm/lucide.mjs')));
const icons = {
  bell: lucide.Bell, check: lucide.Check, 'circle-alert': lucide.CircleAlert,
  'circle-check': lucide.CircleCheck, 'circle-x': lucide.CircleX, diamond: lucide.Diamond,
  ellipsis: lucide.Ellipsis, folder: lucide.Folder, 'folder-plus': lucide.FolderPlus,
  'grid-2x2': lucide.Grid2X2, history: lucide.History, house: lucide.House,
  minus: lucide.Minus, pencil: lucide.Pencil, plus: lucide.Plus,
  'rotate-cw': lucide.RotateCw, save: lucide.Save, search: lucide.Search,
  settings: lucide.Settings, star: lucide.Star, 'trash-2': lucide.Trash2,
  'triangle-alert': lucide.TriangleAlert, x: lucide.X,
};

const escape = (value) => String(value).replaceAll('&', '&amp;').replaceAll('"', '&quot;');
const render = ([tag, attributes]) => `<${tag} ${Object.entries(attributes).map(([name, value]) => `${name}="${escape(value)}"`).join(' ')}/>`;
const output = path.join(root, 'native/data/icons');
await mkdir(output, { recursive: true });
for (const [name, nodes] of Object.entries(icons)) {
  const svg = [
    '<!-- Generated from Lucide 1.73.0 (ISC); do not edit directly. -->',
    '<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">',
    ...nodes.map(render),
    '</svg>',
    '',
  ].join('\n');
  await writeFile(path.join(output, `${name}.svg`), svg);
}
console.log(`Generated ${Object.keys(icons).length} Lucide SVG resources.`);

